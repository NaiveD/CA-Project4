#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include "parse.c"

/* Get the prompt */
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
    execvp(command->cmd_name,command->args);
    return 0;
}

int isBackgroundJob(cmd* command){
    int backgnd = 0;
    if (strcmp(command->args[command->num_args],"&")==0)
        backgnd =  1;
    return backgnd;
}

int main (int argc, char *argv[])
{
    assert(argc == 2);
    FILE* infile;
    infile = fopen(argv[1], "r");
    while (1) {
        /* int childPid;  Used later when executing command */
        char prompt[100]; /* the prompt */
        char cmdLine[1024+1]; /* the command line */
        cmd* command; /* the command (struct) */
        
        /* printPrompt(prompt); get the prompt */
      
        if (fgets(cmdLine, 1024, infile) == NULL) {
            break;
        }

        if (cmdLine[strlen(cmdLine)-1] == '\n')
            cmdLine[strlen(cmdLine)-1] = '\0';
        
        /* If there is nothing input, continue to next loop */
        if (strlen(cmdLine) == 0)
            continue;

        command = parseCommand(cmdLine); /* parse the command line */

        if (isBuiltInCommand(command)) {
            executeBuiltInCommand(command);
        } else {
            //printf("test");
            int childPid = 0;
            childPid = fork();
            if (childPid == 0){
                //printf("test");
                executeCommand(command);
            } else {
                if (isBackgroundJob(command)){
                    //record in list of background jobs
                    printf("success");
                } else {
                int stat = 0;
                waitpid (childPid,&stat,0);
                }
            }
        } 
        /* Free the memory of command */
        free(command);
    }
    fclose(infile);
}