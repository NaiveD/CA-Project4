#include "parse.h"

/* cat < test.sh | grep -n public > test2.txt & */
/* cat \0 test.sh\0 grep -n public \0 test2.txt \0 */

void parseCommand(char* cmdLine)
{
    char *p, *q; // A pointer used when parsing
    char* cmdstr_arr[PIPELINE]; //store the command in pipe;

    /* Initializations */
    memset(cmdstr_arr, 0, PIPELINE);
    memset(infile, 0, 1024+1);
    memset(outfile, 0, 1024+1);
    append = 0;
    backgnd = 0;
    cmd_count = 0;
    // Initialization of command_arr
    for (int i = 0; i < PIPELINE; i++) {
        // malloc for each cmd struct
        command_arr[i] = (cmd*) malloc(sizeof(cmd)); // Remember to free !!! 
        // initialize each argument array
        memset(command_arr[i]->args, 0, MAX_ARGS);
        // Set number of arguments
        command_arr[i]->num_args = 0;
        command_arr[i]->infd = 0; // stdin
        command_arr[i]->outfd = 1; //stout
    }

    /* Get each command */
    p = strtok(cmdLine, "|"); // Get the first command
    cmdstr_arr[cmd_count++] = p;  // store the first command

    // Get following commands
    p = strtok(NULL, "|");
    while (p != NULL) {
        cmdstr_arr[cmd_count++] = p;
        p = strtok(NULL, "|");
    }
    
    /* Parse each command and store the struct in command_arr */    
    for (int i = 0; i < cmd_count; i++)
    {
        q = strtok(cmdstr_arr[i], " "); // Get the first token of the command, i.e. the command name
        command_arr[i]->cmd_name = q; // Set the command name
        command_arr[i]->args[0] = q; // append in args array

        // Get following arguments
        q = strtok(NULL, " "); // Get the first argument
        while (q != NULL){
            command_arr[i]->num_args++;
            command_arr[i]->args[command_arr[i]->num_args] = q;
            q = strtok(NULL," ");
        }
    }

    // For redirection (<, >, >>) and background process (&)
    for (int i = 0; i < cmd_count; i++) { // for every command in cmdLine
        for (int j = 1; j <= command_arr[i]->num_args; j++) {// for every argument in this command
            if (strcmp(command_arr[i]->args[j], "<") == 0) {
                // set infile[1024+1] as command_arr[i]->args[j+1]
                // printf("command_arr[i]->args[j+1] = %s\n", command_arr[i]->args[j+1]);
                strcpy(infile, command_arr[i]->args[j+1]); // set the infile name
                // printf("after strcpy: infile = %s\n", infile);
                command_arr[i]->num_args--;
                command_arr[i]->args[j] = NULL;
                continue;
            }
            else if (strcmp(command_arr[i]->args[j], ">") == 0) {
                strcpy(outfile, command_arr[i]->args[j+1]); // set the outfile name
                append = 0;
                command_arr[i]->num_args--;
                command_arr[i]->args[j] = NULL;
                continue;
            }
            else if (strcmp(command_arr[i]->args[j], ">>") == 0) {
                strcpy(outfile, command_arr[i]->args[j+1]); // set the outfile name
                append = 1;
                command_arr[i]->num_args--;
                command_arr[i]->args[j] = NULL;
                continue;
            }
            else if (strcmp(command_arr[i]->args[j], "&") == 0) {
                backgnd = 1;
                command_arr[i]->num_args--;
                command_arr[i]->args[j] = NULL;
                continue;
            }
        }
    }
}