#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "parse.c"

/* Print the prompt */
void printPrompt(void);

int
main (int argc, char **argv)
{
    while (1){
        int childPid;
        char * cmdLine;

        printPrompt();

        cmdLine= readCommandLine(); //or GNU readline("");

        cmd = parseCommand(cmdLine);

        record command in history list (GNU readline history ?)

        if ( isBuiltInCommand(cmd)){
            executeBuiltInCommand(cmd);
        } else {
            childPid = fork();
            if (childPid == 0){
                executeCommand(cmd); //calls execvp

            } else {
                if (isBackgroundJob(cmd)){
                    record in list of background jobs
                } else {
                    waitpid (childPid);

                }
            }
        }
    }
}


/* Print the prompt */
void printPrompt(void)
{
    char hostname[100];
    char cwd[100];
    struct passwd *my_info;

    /* get host name */
    gethostname(hostname, sizeof(hostname));

    /* get current working directory */
    getcwd(cwd, sizeof(cwd));

    /* get the struct */
    my_info = getpwuid(geteuid());

    /* login name@host name:current directory$ */
    printf("%s@%s:~%s$ ", my_info->pw_name, hostname, cwd+strlen(my_info->pw_dir));

}