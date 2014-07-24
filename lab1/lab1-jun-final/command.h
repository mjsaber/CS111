// UCLA CS 111 Lab 1 command interface
#include <sys/types.h>
typedef struct command *command_t;
typedef struct command_stream *command_stream_t;
typedef struct token *token_t;
typedef struct cmd_node *cmd_node_t;
typedef struct cmd_node_list *cmd_list_t;

/*Struct for parallelism*/
struct cmd_node  //base unit for parallelism execuction
{
  pid_t cpid;
  command_t cmd;//the command
    int parent_num;//number of parents
    cmd_list_t child;//list to hold children
    cmd_list_t child_tail;
    char **files;//file array thta contains all related files in that cmd_node
    int exe;//wheter exe;

};

/*cmd_node_list*/

struct cmd_node_list
{
  cmd_node_t node;
  struct cmd_node_list *next;
  int size;
  /* data */
};


/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the integer flag is
   nonzero.  */
void execute_command (command_t, int);

/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);

void execute_parallel(void);
void build_dependency();
void separate_list();
