#ifndef PROJ4_PARSE_H
#define PROJ4_PARSE_H

#define MAX_ARGS 1024
//the amount of command
#define PIPELINE 10

/* define a type "cmd" for every command */
typedef struct cmd
{
    /* the name of the command */
    char* cmd_name;

    /* argument array and argument number */
    char* args[MAX_ARGS];
    int num_args;

    // Redirection
    int infd;  //Redirection in file descriptor
    int outfd; //Redirection out file descriptor
} cmd;

/* the parser function
 * input a line of command, and
 * output a pointer to the struct "cmd" which contains the command name and arguments
 *
 * */
void parseCommand(char* cmdLine);

// Global variables
extern cmd* command_arr[PIPELINE]; // the command array
extern char infile[1024+1]; // the redirection in file name
extern char outfile[1024+1]; // the redirection out file name
extern int append; // if redirection out is >>
extern int backgnd; // if background command, 1 for background, 0 for non-background
extern int cmd_count; // the number of commands in cmdLine

#endif
