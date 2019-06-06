#include "parse.h"

cmd* parseCommand(char* cmdLine)
{
    cmd* command; /* the pointer to the struct "cmd" to be returned*/
    char* p; /* A pointer used when parsing */

    /* cmd is on stack, this can't be returned
     * malloc so it's on heap and can be returned
     *
     * */
    command = (cmd*) malloc(sizeof(cmd)); /* Remember to free !!! */
    command->num_args = 0;
    for (int i = 0; i < MAX_ARGS; i++)
    {
        command->args[i] = NULL;
    }
    
    p = strtok(cmdLine, " "); /* get the first token of cmdLine, i.e. the command name */
    command->cmd_name = p; /* set the command name */
    command->args[command->num_args] = p;
    /* continue parsing. get the arguments */
    p = strtok(NULL, " "); /* the first argument */
    while (p != NULL && *p!='\n') {
        command->num_args++;
        command->args[command->num_args] = p;
        
        p = strtok(NULL, " ");
    }

    return command;
}