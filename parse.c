#include "parse.h"

/* cat < test.sh | grep -n public > test2.txt & */
/* cat \0 test.sh\0 grep -n public \0 test2.txt \0 */

void parseCommand(char* cmdLine)
{
    char *p, *q; // A pointer used when parsing
    int cmd_count = 0; // the number of commands in cmdLine
    char* cmdstr_arr[PIPELINE]; //store the command in pipe;

    /* Initializations */
    memset(cmdstr_arr, 0, PIPELINE);
    memset(infile, 0, 1024+1);
    memset(outfile, 0, 1024+1);
    append = 0;
    backgnd = 0;
    // Initialization of command_arr
    for (int i = 0; i < PIPELINE; i++) {
        // malloc for each cmd struct
        command_arr[i] = (cmd*) malloc(sizeof(cmd)); // Remember to free !!! 
        // initialize each argument array
        memset(command_arr[i]->args, 0, MAX_ARGS);
        // Set number of arguments
        command_arr[i]->num_args = 0;
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

    // printf("In parse function: infile = %s\n", infile);
    // printf("In parse function: outfile = %s\n", outfile);
    //q = strtok(p, " "); /* get the first token of p, i.e. the command name */
    //command_arr[cmd_count]->cmd_name = q; /* set the command name */
    //command_arr[cmd_count]->args[0] = q;
    
    /* continue parsing. get the arguments */
    //q = strtok(NULL, " "); /* the first argument */
    //while (q != NULL) {
     //   command_arr[cmd_count]->num_args++;
     //   command_arr[cmd_count]->args[command_arr[cmd_count]->num_args] = q;
        
     //   q = strtok(NULL, " ");
   // }

   // cmd_count++;

    /* Get later commands */
    //p = strtok(NULL, " "); /* the first argument */
    //while (p != NULL) {
        /* Parse each command */    
        //q = strtok(p, " "); /* get the first token of p, i.e. the command name */
        //command_arr[cmd_count]->cmd_name = q; /* set the command name */
        //command_arr[cmd_count]->args[command_arr[cmd_count]->num_args] = q;
        
        /* continue parsing. get the arguments */
       // q = strtok(NULL, " "); /* the first argument */
       // while (q != NULL) {
            //command_arr[cmd_count]->num_args++;
            //command_arr[cmd_count]->args[command_arr[cmd_count]->num_args] = q;
            
            //q = strtok(NULL, " ");
        //}

        //cmd_count++;    
        
        //p = strtok(NULL, " ");
    //}
}

/* void get_command(int i); Get the ith command
int check(const char* str);
void getname(char* name); Get the file name of redirection */

/* Return an array of commands in cmdLine 
cmd** parseCommand(char* cmdLine) {
    cmd* command[PIPELINE]; the array of commands in cmdLine to be returned

    for (int i = 0; i < PIPELINE; i++) {
        comman[i] = (cmd*) malloc(sizeof(cmd)); Remember to free !!! 
    }

    Initialization of the command array to be returned
    memset()



    Get the command name (cat/mkdir/echo)
    get_command(0);
    // check if there are redirection in, and get the input file name
    if(check("<"))
    // get the input file name
        getname(infile);
    
    // check if there are pipes
    int i；
    for (int i = 1; i < PIPELINE; i++)
    {
        if(check("|")){
            //get ith command
            get_command(i);
            //i++;
        }else{
            break;
        }
    }
    // Check if there are redirection out
    if(check(">"){
        if (check(">"))
            append = 1;
        // Get the output file name
        getname(outfile);
    }
    // Check if there are background command (&)
    if (check("&"))
        backgnd = 1;

    // omit check of '\n'
} */

