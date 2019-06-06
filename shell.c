#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parse.c"

/* Print the prompt */
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

    }
    else if (strcmp(command->cmd_name, "kill")==0) {

    }
}

/* calls execvp */
void executeCommand(cmd* command){
    pid_t pid = fork();
    if (pid == 0){
        execvp(command->cmd_name,command->args);
    }else
    //{
    //    printf("%d",childPid);
    //}
    return 0;
}

int main (int argc, char **argv)
{
    while (1) {
        /* int childPid;  Used later when executing command */
        char prompt[100]; /* the prompt */
        char cmdLine[100]; /* the command line */
        char* cmdlineptr = NULL;
        cmd* command; /* the command (struct) */

        printPrompt(prompt); /* get the prompt */

        /* If meet EOF */
        if (gets(cmdLine) == NULL) {
            printf("\n");
            break;
        }

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
                // printf("test");
                executeCommand(command);
            } else {
                // printf("%d",childPid);
                wait(NULL);
                //wait(NULL);
                /*if (isBackgroundJob(command)){
                     record in list of background jobs
                } else {
                    waitpid (childPisd);*/
                //}
            }
        } 
        /* Free the memory of command */
        free(command);
    }
}