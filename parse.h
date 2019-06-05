#ifndef PROJ4_PARSE_H
#define PROJ4_PARSE_H

#define MAX_ARGS 100

/* define a type "cmd" for every command */
typedef struct cmd
{
    /* the name of the command */
    char* cmd_name;

    /* argument array and argument number */
    char* args[MAX_ARGS];
    int num_args;

} cmd;

/* the parser function
 * input a line of command, and
 * output a pointer to the struct "cmd" which contains the command name and arguments
 *
 * */
cmd* parseCommand(char* cmdLine);

#endif
