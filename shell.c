#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "parse.c"

/* Print the prompt */
void printPrompt(void);
char* readCommandLine(int argc, char **argv);

int
main (int argc, char **argv)
{
    while (1){
        int childPid;
        char* cmdLine;

        printPrompt();

        cmdLine = readCommandLine(); //or GNU readline("");

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
    char hostname[100]; /* host name */
    char cwd[100]; /* current working directory */
    char cwd2[100]; /* current working directory */
    struct passwd *my_info; /* used for getpwuid */
    char* prompt; /* $ or # */

    /* get host name */
    gethostname(hostname, sizeof(hostname));

    /* get current working directory */
    getcwd(cwd, sizeof(cwd));

    /* get the struct */
    my_info = getpwuid(geteuid());

    /* get the prompt */
    if (geteuid() == 0) /* if rooted */
        prompt = "#";
    else /* if not rooted */
        prompt = "$";

    /* login name: my_info->pw_name */
    /* home (~): my_info->pw_dir */
    /* abbreviate as ~ */
    if (strncmp(cwd, my_info->pw_dir, strlen(my_info->pw_dir)) == 0) /* If the cwd contains home (~), abbreviate it as ~ */
        printf("%s@%s:~%s%s ", my_info->pw_name, hostname, cwd+strlen(my_info->pw_dir), prompt);
    else
        printf("%s@%s:%s%s ", my_info->pw_name, hostname, cwd, prompt);
}