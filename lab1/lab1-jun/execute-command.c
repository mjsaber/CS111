// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "alloc.h"
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
//extern int errno;

/*list of cmd_node*/
cmd_list_t cmd_list_to_excute; //execution list for parallel
cmd_list_t cmd_list_tail;
int k;//swith for initlize cmd_list_to_excute
int unit;//test purpose
int fileindex;//use to track file index when add files


int
command_status (command_t c)
{
  return c->status;
}

void 
exec_simple_cmd(command_t c)
{
	int status;
	pid_t pid = fork();
	if (pid == -1)
	{
		fprintf(stderr,"fork: %s\n", strerror(errno));
		exit(1);
	}

	if (pid ==0)
	{
		if (c->input != NULL)
		{
			int fd_in = open (c->input, O_RDWR, 0644);
			if (fd_in < 0)
			{
				fprintf(stderr,"%s: %s\n",c->input, strerror(errno));
				exit(1);
			}
			if (dup2(fd_in,0)<0)
			{
				fprintf(stderr,"timetrash: %s\n",strerror(errno));
				exit(1);
			}
			close (fd_in);

		}	
		if (c->output != NULL)
		{
			int fd_out = open (c->output, O_RDWR|O_TRUNC|O_CREAT, 0644);
			if (fd_out <0)
			{
				fprintf(stderr,"%s: %s\n",c->output, strerror(errno));
				exit(1);

			}
			if (dup2(fd_out,1)<0)
			{
				fprintf(stderr,"timetrash: %s\n",strerror(errno));
				exit(1);
			}
			close (fd_out);
		}
		char* file = c->u.word[0];
		char** parameter = c->u.word;
		if (execvp(file, parameter) == -1)
		{
			fprintf(stderr,"%s: %s\n",file, strerror(errno));
			exit(1);
		}
		exit(0);
	}
	else if (pid > 0)
	{
		waitpid (-1, &status, 0);
		c->status = status;
	}

}

void exec_cmd (command_t c)
{
		enum command_type type = c->type;
		int o_sin  = dup(STDIN_FILENO);
		int o_sout = dup(STDOUT_FILENO);

	switch (type)
	{
		case SIMPLE_COMMAND:
			exec_simple_cmd(c);
			//printf ("are you hanging there?\n");
			break;
		case AND_COMMAND:
		{         // A && B
			
			command_t left_cmd = c->u.command[0];
			command_t right_cmd = c->u.command[1];
			exec_cmd(left_cmd);
			if (command_status(left_cmd) != 0)
			{				
				c->status = -1;
				break;
			}
			else
				exec_cmd (right_cmd);
			if (command_status(right_cmd) != 0)
				c->status = -1;
			else
				c->status = 0;
			break;
		}
		case OR_COMMAND:  // A || B
		{
			command_t left_cmd = c->u.command[0];
			command_t right_cmd = c->u.command[1];
			exec_cmd(left_cmd);
			if (command_status(left_cmd) == 0)
				{
					c->status = 0;
					break;
				}
			else
				exec_cmd (right_cmd);
			if (command_status(right_cmd) != 0)
				c->status = -1;
			else
				c->status = 0;
			break;
		}     
		case PIPE_COMMAND:        // A | B
		{
			command_t left_cmd = c->u.command[0];
			command_t right_cmd = c->u.command[1];
			int pipefd[2];
			pipe (pipefd);
			pid_t pid = fork();
			if (pid == -1)
			{
				fprintf(stderr,"fork: %s\n", strerror(errno));
				exit(1);
			}
			if (pid == 0)//child handle left side of pipe
			{
					close (pipefd[0]);
				if (dup2(pipefd[1],1)<0)
				{
				fprintf(stderr,"%d: %s\n",pipefd[1], strerror(errno));
				exit(1);
				}
				close (pipefd[1]);
				exec_cmd(left_cmd);
					exit(0);
			}
			else if (pid > 0)//parent handle right side of pipe
			{
				waitpid(-1, &(c->status), 0);
				close (pipefd[1]);
				if (dup2(pipefd[0],0)<0)
				{
				fprintf(stderr,"%d: %s\n",pipefd[0], strerror(errno));
				exit(1);
				}
				close (pipefd[0]);
				exec_cmd (right_cmd);
				//close (1);
//				printf ("11111\n");
				c->status = 0;
			}
		 
		 				break;
		}
   		case SEQUENCE_COMMAND:    // A ; B
    	{
    	//	printf ("seq\n");
    		command_t left_cmd = c->u.command[0];
			command_t right_cmd = c->u.command[1];
			exec_cmd(left_cmd);
			exec_cmd(right_cmd);
			if (command_status(right_cmd)!=0)
				c->status = -1;
			else
				c->status = 0;
			break;

    	}
 	    case SUBSHELL_COMMAND:    // ( A )
 	   { 
 	   	if (c->input != NULL)
		{
			int fd_in = open (c->input, O_RDWR, 0644);
			if (fd_in < 0)
			{
				fprintf(stderr,"%s: %s\n",c->input, strerror(errno));
				exit(1);
			}
			if (dup2(fd_in,0)<0)
			{
				fprintf(stderr,"timetrash: %s\n",strerror(errno));
				exit(1);
			}
			close (fd_in);

		}	
		if (c->output != NULL)
		{
			int fd_out = open (c->output, O_RDWR|O_TRUNC|O_CREAT, 0644);
			if (fd_out <0)
			{
				fprintf(stderr,"%s: %s\n",c->output, strerror(errno));
				exit(1);

			}
			if (dup2(fd_out,1)<0)
			{
				fprintf(stderr,"timetrash: %s\n",strerror(errno));
				exit(1);
			}
			close (fd_out);
		}

 	    exec_cmd(c->u.subshell_command);
 	    if (command_status(c->u.subshell_command) != 0)
				c->status = -1;
			else
				c->status = 0;

 	    break;
 	}
 	    default:
 	    break;
	}
     //reset fd
	if (c->input == NULL)
	{
		dup2(o_sin,STDIN_FILENO);
		close(o_sout);
	}
	if (c->output == NULL)
    {
    	dup2(o_sout, STDOUT_FILENO);
    	close(o_sin);
    }
}


/*Dependency Graph */




/*add filename into file list of a cmd_node*/
char** add_a_file_to_list(char **file, char* filename){
	int newlen = fileindex +1 ;
	//printf("%d\n", fileindex );
	//printf("%s\n", filename);
	file = checked_realloc(file, sizeof(char*) * (newlen+1) );
	file[fileindex] = checked_malloc(sizeof(char*));
	//printf("success aloc\n");
	strcpy(file[fileindex], filename);
    file[newlen] = NULL;
	return file;
}

/*length of file list,include null pointer*/
int files_size(char **file){
	int length;
	length = 0;
	char **pos = file;
	while( *pos != NULL){
		length++;
		pos++;
	}
	return length+1;
}

int tc;

/*add all files into a command node, use recurrsion*/
char** extract_files(command_t c, char **result)
{
    switch(c->type){
   	case SIMPLE_COMMAND: // base case
   	{
   			if(c->input != NULL){
   				result = add_a_file_to_list(result,c->input);
   				fileindex++;
   			}
   				
   			if(c->output != NULL){
   			   	result = add_a_file_to_list(result,c->output);
   			   	fileindex++;
   			}
   			break;   			
   	}
   	/*pretty simple*/
   	case AND_COMMAND:  
    case SEQUENCE_COMMAND:    // A ; B
    case OR_COMMAND: 
    case PIPE_COMMAND: 
		result = extract_files(c->u.command[0],result);
		result = extract_files(c->u.command[1],result);
		break; 
    case SUBSHELL_COMMAND: 
        result = extract_files(c->u.subshell_command,result);  
    	break;
   }
   return result;
}


void add_a_cmd_to_list(cmd_node_t cmd_n){
	cmd_list_t new_node = checked_malloc(sizeof(struct cmd_node_list));
        new_node->node = cmd_n;
        new_node->next = NULL;
        new_node->size = unit;

	if(cmd_list_to_excute == NULL) //if the whole command link is null
	{
		cmd_list_to_excute = new_node;
		cmd_list_tail = cmd_list_to_excute;
	}
	else
	{
		cmd_list_tail->next = new_node;
		cmd_list_tail = cmd_list_tail->next;
	}
}

/*create a command list by using execute_command function*/    
void add_cmd_node_to_execution_list(command_t c){
    /*initialize a cmd_node*/
	cmd_node_t temp =  checked_malloc(sizeof(struct cmd_node));
	temp->cmd = c;
	temp->child = NULL;
	temp->child_tail = NULL;
	temp->parent_num = 0;
	temp->files = NULL;

	/*initialize space for file array, add all relate flies into file array of a command node*/
	temp->files = checked_malloc(sizeof(char*));
	temp->files = extract_files(c,temp->files);

	/*add a new cmd_node into list*/
	add_a_cmd_to_list(temp);
}



/*compare whether file array has any same file names, 1 means dependent, 0 means not dependent*/
int isdependent(char **file1, char **file2){
	int i;
	i = 0;
	while(file1[i] != NULL){
		int k;
		k = 0;
		while(file2[k] != NULL){
			if(strcmp(file1[i],file2[k]) == 0)
				return 1;
			k++;
		}
		i++;
	}
	return 0;
}


void
execute_command (command_t c, int time_travel)
{

	if (time_travel != 1)
		{
			exec_cmd(c);
			//print_command (c);
		}
	else
	{
	    add_cmd_node_to_execution_list(c);//construct list
	    unit++;//count how many commands in list
	    fileindex = 0;//reset

	}
}

void build_dependency()
{
	int len = unit;
	//printf("command list is %d long\n",len );
	cmd_list_t ptr = cmd_list_to_excute;
	int child_num;
	child_num = 0;
	int i;
	for(i = 0;i < len;i++)
	{
		int k;
		char ** file1 = ptr->node->files;
		cmd_list_t ptr2 = ptr->next;
		//if a command node has files, then compare it with rest of the command list*/

		if(file1 != NULL){
			for(k = i+1; k < len; k++){

				char ** file2 = ptr2->node->files;
				/*if any command has files and any of its file name equal to 
				any file name in file1, then start to build denpendency*/
				
				if(file2 != NULL && isdependent(file1,file2)){
					cmd_list_t new_node = checked_malloc(sizeof(struct cmd_node_list));
        			new_node->node = ptr2->node;
        			new_node->next = NULL;
        			new_node->size = child_num;

					if(ptr->node->child == NULL) //if the whole command link is null
					{
						ptr->node->child = new_node;
						ptr->node->child_tail = ptr->node->child;
					}
					else
					{
						ptr->node->child_tail->next = new_node;
						ptr->node->child_tail = ptr->node->child_tail->next;
					}
					child_num++;
					ptr2->node->parent_num++;
					
				}	
				ptr2 = ptr2->next;			

			}
		}
		ptr = ptr->next;
	}

}






void execute_parallel()
{
		cmd_list_t ptr = cmd_list_to_excute;

		while(ptr!=NULL)
		{
			printf ("num of parents %d, w8 for %s\n",ptr->node->parent_num, *ptr->node->files);
			if(ptr->node->parent_num == 0 )
			{
				pid_t pid = fork();
				if (pid == -1)
				{
					fprintf(stderr,"fork: %s\n", strerror(errno));
					exit(1);
				}
				if (pid == 0)
				{
					exec_cmd(ptr->node->cmd);
					cmd_list_t ptr_child = ptr->node->child;
					while(ptr_child != NULL)
					{
						ptr_child->node->parent_num--;
						if (ptr_child->node->parent_num == 0)
							exec_cmd(ptr_child->node->cmd);
						ptr_child = ptr_child->next;
					}
					exit(0);
				}
				else
					ptr->node->cpid = pid;
			}
			ptr=ptr->next;
		}
		ptr = cmd_list_to_excute;
		while (ptr != NULL)
		{
			while (ptr->node->cpid == wait(NULL))
				break;
			ptr = ptr->next;
		}
		free(cmd_list_to_excute);
}
/*
		cmd_list_t queue_ptr = cmd_list_to_excute;
		cmd_list_t i = queue_ptr->node->child;
		while(i != NULL){
			printf("child name is %s\n", i->node->files[0]);
		//	printf("child_nume is %d\n", i->node->child->size);
			i = i->next;
		}*/