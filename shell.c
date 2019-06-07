#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <assert.h>
#include "parse.c"

// Global variables
cmd* command_arr[PIPELINE]; // the command array
NODE* head;
char infile[1024+1]; /* the redirection in file name*/
char outfile[1024+1]; /* the redirection out file name */
int append = 0; // if redirection out is >>
int backgnd = 0; /* if background command, 1 for background, 0 for non-background */
int cmd_count = 0; // the number of commands in cmdLine
int lastpid;

/* Get the prompt, but do not print */
void printPrompt(char* prompt)
{
    char hostname[100]; /* host name */
    char cwd[100]; /* current working directory */
    struct passwd *my_info; /* used for getpwuid() */
    char* root; /* $ or # */

    /* get host name */
    gethostname(hostname, sizeof(hostname));

    /* get current working directory */
    getcwd(cwd, sizeof(cwd));

    /* get the struct */
    my_info = getpwuid(geteuid());

    /* get the prompt */
    if (geteuid() == 0) /* if rooted */
        root = "#";
    else /* if not rooted */
        root = "$";

    /* login name: my_info->pw_name */
    /* home (~): my_info->pw_dir */
    /* abbreviate as ~ */
    if (strncmp(cwd, my_info->pw_dir, strlen(my_info->pw_dir)) == 0) /* If the cwd contains home (~), abbreviate it as ~ */
        printf("%s@%s:~%s%s ", my_info->pw_name, hostname, cwd+strlen(my_info->pw_dir), root);
    else
        printf("%s@%s:%s%s ", my_info->pw_name, hostname, cwd, root);
}

/* Input a command,
 * return 1 if it's a built-in command
 * otherwise return 0
 *
 * */
int isBuiltInCommand(cmd* command) {
    /* 4 built-in commands: cd, jobs, exit, kill */
    if ((strcmp(command->cmd_name, "cd")==0) || (strcmp(command->cmd_name, "jobs")==0) || (strcmp(command->cmd_name, "exit")==0) || (strcmp(command->cmd_name, "kill")==0))
        return 1;
    else
        return 0;
}

/* execute the built-in command */
void executeBuiltInCommand(cmd* command)
{
    if (strcmp(command->cmd_name, "cd")==0) {
        struct passwd *my_info; /* used for getpwuid() */
        char dir[100];
        /* get the struct */
        my_info = getpwuid(geteuid());

        if (command->num_args == 0)
            chdir(my_info->pw_dir);

        else if (command->num_args == 1) {
            if (command->args[1][0] == '~') {
                sprintf(dir, "%s%s", my_info->pw_dir, command->args[1]+1);
                chdir(dir);
            }
            else
                chdir(command->args[1]);
        }
    }
    else if (strcmp(command->cmd_name, "jobs")==0) {
        NODE* printnode = head->next;
        while (printnode != NULL)
        {
            printf("%d %s\n", printnode->npid, printnode->cmdname);
            printnode = printnode->next;
        }
        
    }
    else if (strcmp(command->cmd_name, "exit")==0) {
        exit(0);
    }
    else if (strcmp(command->cmd_name, "kill")==0) {

    }
}

/* calls execvp */
void executeCommand(cmd* command){
    execvp(command->cmd_name,command->args);
}

int isBackgroundJob(cmd* command){
    return backgnd;
}

int main (int argc, char *argv[])
{
    //For jobs
    head = (NODE*)malloc(sizeof(NODE));
    head->next = NULL;
    assert(argc == 2);
    FILE* test_file;
    test_file = fopen(argv[1], "r");
    int loop = 0;
    while (1) {
        /* int childPid;  Used later when executing command */
        char prompt[100]; /* the prompt */
        char cmdLine[1024+1]; /* the command line */
    
        /* printPrompt(prompt); get the prompt */
      
        if (fgets(cmdLine, 1024, test_file) == NULL) {
            break;
        }

        if (cmdLine[strlen(cmdLine)-1] == '\n')
            cmdLine[strlen(cmdLine)-1] = '\0';
        
        /* If there is nothing input, continue to next loop */
        if (strlen(cmdLine) == 0)
            continue;

        parseCommand(cmdLine); /* parse the command line, get the command array */

        // Execute the commands in cmdLine
        if (isBuiltInCommand(command_arr[0])) {
            executeBuiltInCommand(command_arr[0]);
        } else {
            int childPid = 0;
            // childPid = fork();
            if (childPid == 0) { // child process, call process A
                // printf("The first child process\n");
                // Single command, no pipe
                if (cmd_count == 1) {
                    // Execute the command (including redirection)
                    if (infile[0] != '\0'){
                        int fdin = open(infile, O_RDONLY);
                        dup2(fdin,STDIN_FILENO);
                        close(fdin);
                    }

                    if (outfile[0] != '\0') {
                        int fdout;
                        if (append = 1)
                            fdout = open(outfile, O_RDWR|O_CREAT|O_APPEND,0777);
                        else
                            fdout = open(outfile, O_RDWR|O_CREAT|O_TRUNC,0777);
                        dup2(fdout, STDOUT_FILENO)==-1;
                        close(fdout);
                        executeCommand(command_arr[0]);
                    }else{
                        executeCommand(command_arr[0]);
                    }
                }

                // handle pipe (only 2 commands)
                else if (cmd_count == 2) {
                    // cmd1 | cmd2
                    // Create a pipe
                    int pfd[2];
                    if (pipe(pfd) == -1) {
                        perror("pipe");
                        exit(EXIT_FAILURE);
                    }

                    // Create process B1, execute cmd1
                    int B1Pid = 0;
                    B1Pid = fork();
                    if (B1Pid == 0) {
                        close(pfd[0]); // close the read end of the pipe
                        dup2(pfd[1], STDOUT_FILENO); // Redirect stdout as the write end
                        close(pfd[1]);

                        // Execute cmd1, write the output to the pipe
                        executeCommand(command_arr[0]);
                    }

                    // Create process B2, execute cmd2
                    // int B2Pid = 0;
                    // B2Pid = fork();
                    // if (B2Pid == 0)
                    else {
                        close(pfd[1]); // close the write end of the pipe
                        dup2(pfd[0], STDIN_FILENO); // Redirect stdout as the write end
                        close(pfd[0]);

                        // Execute cmd2, take input from the pipe
                        executeCommand(command_arr[1]);

                        // In process A
                        close(pfd[0]); // close the read end
                        close(pfd[1]); // close the write end

                        // wait for the two child process B1 and B2
                        int stat = 0;
                        waitpid (B1Pid, &stat, 0);
                        // waitpid (B2Pid, &stat, 0);
                    }
                }
                
                // handle multiple pipes
                else if (cmd_count > 2) { // at least 3 command
                    // cmd1 | cmd2 | cmd3
                    // cmd1 | cmd2 | cmd3 | cmd4 | cmd5
                    
                    int fd; // file descriptor
                    int fds[2]; // pipe(fds)
                    for (int i = 0; i < cmd_count; i++) {
                        //printf("222\n");
                        //sleep(1);
                        // if not the last command, we need to create a pipe for it
                        if (i < cmd_count - 1)
                        {
                            pipe(fds); // create a pipe
                            command_arr[i]->outfd = fds[1]; // set cmd[i]'s out file descriptor
                            command_arr[i+1]->infd = fds[0]; // in file descriptor
                        }

                        // execute the ith command
                        // Create process B1, execute cmd1
                        int B1Pid;
                        B1Pid = fork();
                        if (B1Pid == 0) { // if in this child process B1
                            // set read end
                            if (command_arr[i]->infd != 0)
                            {
                                close(0);
                                dup2(command_arr[i]->infd, STDIN_FILENO); // Redirect stdin as the read end
                            }
                            if (command_arr[i]->outfd != 1)
                            {
                                close(1);
                                dup2(command_arr[i]->outfd, STDOUT_FILENO); // Redirect stdout as the write end
                            }
                            //printf("ssss\n");
                            // close every other file descriptors
                            for (int j = 3; j < 1024; j++)
                                close(j);

                            // Execute cmd i, write the output to the pipe
                            executeCommand(command_arr[i]);
                            sleep(5);
                        }
                        else if (B1Pid > 0)
                        { // Parent process
                            lastpid = B1Pid;
                            signal(SIGCHLD, SIG_DFL);
                        }

                        if ((fd = command_arr[i]->infd) != 0) // if not stdin
                            close(fd);

                        if ((fd = command_arr[i]->outfd) != 1) // if not stdout
                            close(fd);
                    }             
                    while (wait(NULL) != lastpid);
                }
            } 
                if (isBackgroundJob(command_arr[0])){
                    // record in list of background jobs
                    NODE* p = malloc(sizeof(NODE));
                    p->npid = childPid;
                    strcpy(p->cmdname, command_arr[0]->cmd_name);

                    NODE* tmp = head->next;
                    head->next = p;
                    p->next = tmp;

                    signal(SIGCHLD, SIG_IGN);
                } else {
                    // printf("222\n");
                    int stat = 0;
                    wait(NULL);
                }
            
        }

        /* Free the memory of command */
        for (int i = 0; i < PIPELINE; i++) {
            free(command_arr[i]);
        }
    }

    fclose(test_file);
}