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
char infile[1024+1]; /* the redirection in file name*/
char outfile[1024+1]; /* the redirection out file name */
int append = 0; // if redirection out is >>
int backgnd = 0; /* if background command, 1 for background, 0 for non-background */

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
        
    }
    else if (strcmp(command->cmd_name, "exit")==0) {
        exit(0);
    }
    else if (strcmp(command->cmd_name, "kill")==0) {

    }
}

/* calls execvp */
void executeCommand(cmd* command){
    //int i;
    //for (int i = 0; i < cmd_count; i++)
    //{
        /* code */
    //}
    
    execvp(command->cmd_name,command->args);
}

int isBackgroundJob(cmd* command){
    return backgnd;
}

int main (int argc, char *argv[])
{
    assert(argc == 2);
    FILE* test_file;
    test_file = fopen(argv[1], "r");
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

        int fdout, size;
        /*char buffer[80];
        fd = open(infile, O_RDONLY);
        size = read(fd, buffer, sizeof(buffer));
        close(fd);

        printf("%s\n", buffer);*/

        /* printf("command1 name = %s\n", command_arr[0]->cmd_name);
        printf("command1 arguments: %s, %s\n", command_arr[0]->args[0], command_arr[0]->args[1]);

        printf("command2 name = %s\n", command_arr[1]->cmd_name);

        printf("infile name = %s\n", infile);
        printf("outfile name = %s\n", outfile);
        printf("backgnd = %d\n", backgnd);
        printf("append = %d\n", append); */

        // Execute the commands in cmdLine
        if (isBuiltInCommand(command_arr[0])) {
            executeBuiltInCommand(command_arr[0]);
        } else {
            //printf("test");
            int childPid = 0;
            childPid = fork();
            if (childPid == 0){
                //printf("test");
                //close(1);
                //printf("output is %s:",outfile);
                //printf("parameter is %s:", command_arr[0]->args[2]);
                if (infile[0] != '\0'){
                    int fdin = open(infile, O_RDONLY);
                    dup2(fdin,STDIN_FILENO);
                    close(fdin);
                }

                if (outfile[0] != '\0'){
                    if (append = 1)
                        fdout = open(outfile, O_RDWR|O_CREAT|O_APPEND,0777);
                    else
                        fdout = open(outfile, O_RDWR|O_CREAT|O_TRUNC,0777);
                    //printf("output is %s:",outfile);
                   // printf("parameter is %s:", command_arr[0]->args[2]);
                    dup2(fdout, STDOUT_FILENO)==-1;
                    close(fdout);
                    executeCommand(command_arr[0]);
                }else{
                    executeCommand(command_arr[0]);
                }

            } else {
                if (isBackgroundJob(command_arr[0])){
                    //record in list of background jobs
                    printf("success");
                } else {
                int stat = 0;
                waitpid (childPid,&stat,0);
                }
            }
        } 

        /* Free the memory of command */
        for (int i = 0; i < PIPELINE; i++) {
            free(command_arr[i]);
        }
    }

    fclose(test_file);
}