// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
int isword (char c)
{
  if (isalpha(c) || isdigit(c) ||
       c == '!' ||c == '%' ||c == '+' ||c == ',' || c == '-' ||
        c == '.' ||c == '/' ||c == ':' ||c == '@' ||c == '^' || c == '_' || c == '`')
    return 1;
  else
    return 0;
}

token_t add_token(char* w)
{
  token_t p = (token_t) checked_malloc (sizeof(struct token));
  p->word = w;
  p->next = NULL;
  return p;
}

command_stream_t
read_buffer (char* buffer, size_t size)
{
 // printf ("%s\n", buffer);
 // printf ("%zd\n", size);
//  size_t word_max = 200;
  size_t count = 0;
  command_stream_t stream = (command_stream_t) checked_malloc (sizeof(struct command_stream));
  stream->wc = 0;
  stream->cur_token = NULL;
  token_t head = NULL;
  
  while (count < size)
  {
    char c = *buffer;
    if (c == '#')
    {
      do
      {
        buffer++;
        count++;
        c = *buffer;  
        if (c ==EOF || c == '\0')
        {
          stream->cur_token = head;
          return stream;
        }
      } while (c!='\n');
    }

    if (isspace(c) && c != '\n')
    {
 //     printf ("space, break\n");
      buffer++;
      count++;

    }

    else if (c == '\n'||c == ';' ||
        c == '(' ||c == ')' ||c == '<' ||c == '>')
    {
      char *t = (char*) checked_malloc (2*sizeof(char));
      memset (t, 0, 2);
      t[0] = c;
      t[1] = '\0';
 
      stream->wc++;
      if (head ==NULL)
      {
        stream->cur_token = add_token(t);
        head = stream->cur_token;
      }
      else
      {
        stream->cur_token->next = add_token(t);
        stream->cur_token= stream->cur_token->next;
      }

//      printf ("huiche:%cle", **(stream->token-1));
      buffer++;
      count++;
    }
    else if (c == '&')
    {
      if (*(++buffer) == '&')
      {
        char *t =(char*) checked_malloc (3*sizeof(char));
        memset (t, 0, 3);
        t[0] = '&';
        t[1] = '&';
        t[2] = '\0';
        if (head ==NULL)
        {
         stream->cur_token = add_token(t);
         head = stream->cur_token;
        }
        else
        {
          stream->cur_token->next = add_token(t);
          stream->cur_token = stream->cur_token->next;
        }
      stream->wc++;

      buffer++;
      count += 2;
      }
      else
      {
        char *t =(char*) checked_malloc (2*sizeof(char));
        memset (t, 0, 2);
        t[0] = '&';
        t[1] = '\0';
          if (head ==NULL)
         {
           stream->cur_token = add_token(t);
           head = stream->cur_token;
          }
          else
          {
           stream->cur_token->next = add_token(t);
            stream->cur_token = stream->cur_token->next;
          }
        stream->wc++;
      count++;
      }
    }
    else if (c == '|')
    {
      if (*(++buffer) == '|')
      {
        char *t = (char*) checked_malloc (3*sizeof(char));
        memset (t, 0, 3);
        t[0] = '|';
        t[1] = '|';
        t[2] = '\0';
        if (head ==NULL)
          {
          stream->cur_token = add_token(t);
          head = stream->cur_token;
         }
         else
         {
           stream->cur_token->next = add_token(t);
            stream->cur_token = stream->cur_token->next;
         }
      stream->wc++;
      buffer++;
      count += 2;
      }
      else
      {
        char *t =(char*) checked_malloc (2*sizeof(char));
        memset (t, 0, 2);
        t[0] = '|';
        t[1] = '\0';
        if (head ==NULL)
          {
          stream->cur_token = add_token(t);
          head = stream->cur_token;
         }
         else
         {
           stream->cur_token->next = add_token(t);
            stream->cur_token = stream->cur_token->next;
         }
      stream->wc++;
      count++;
      }
    }
    else if (isword(c))
    {
      size_t word_size = 10;
      size_t n = 0;
      char *t = (char*) checked_malloc (word_size*sizeof(char));
      memset (t, 0, 10);
      do
      {

        t[n] = c;
        n++;
        if (n >= word_size)
          t = checked_grow_alloc (t, &word_size);
        buffer++;
        count++;
        c = *buffer;
      
      } while (isword(c));
      t[n] ='\0';
          if (head ==NULL)
          {
          stream->cur_token = add_token(t);
          head = stream->cur_token;
         }
         else
         {
           stream->cur_token->next = add_token(t);
            stream->cur_token = stream->cur_token->next;
         }
      stream->wc++;
    }
  }

stream->cur_token = head;
stream->cur_line_num = 1;

if (head == NULL)
  return NULL;
  return stream;
}



command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
   //size_t buffer_size = 512;
   size_t count = 0;
   char *buffer = (char*) checked_malloc (sizeof(char));
   int c = get_next_byte(get_next_byte_argument);
   while (c != EOF)
   {
     buffer[count++] = c;
     c = get_next_byte(get_next_byte_argument);
    buffer = checked_realloc (buffer, sizeof(char)*(count+1));
    }
    buffer[count]='\0';

 // printf ("there are %zd char in total\n", count);

  command_stream_t stream = read_buffer (buffer, count);
  free (buffer);
  
  return stream;
}


int isEqual(char* a, char* b)
{
  return strcmp(a,b)?0:1;
}

int 
isConnToken(char* token)
{
  return (isEqual(token,"|") || isEqual(token,"&&"))
   || isEqual(token,"||");
}

int 
isWordToken(char* token)
{
  if (!isConnToken(token) && !isEqual(token,">") 
    && !isEqual(token,"<")  && !isEqual(token,"(") 
    && !isEqual(token,")")&& !isEqual(token,";") 
    && !isEqual(token,"\n") && !isEqual(token, "&") 
    && !isEqual(token, "`")) 
  return 1;
  else return 0;   
}


enum command_state
{
  INIT_STATUS,
   SIMPLE_CMD_STATUS,
   IN_CMD_STATUS,
   IN_CMD_STATUS_FINISH,
   OUT_CMD_STATUS,
   OUT_CMD_STATUS_FINISH,
  SUBSHELL_CMD,
};

command_t
switch_cmd_order(command_t cur_cmd, command_t *temp_cmd)
{
  
  int priority[4] = {2,1,2,3};
  (*temp_cmd)->u.command[1] = cur_cmd;

  command_t cmd_left = (*temp_cmd)->u.command[0];
  command_t final_cmd = *temp_cmd;

  if(cmd_left->type != SIMPLE_COMMAND && cmd_left->type != SUBSHELL_COMMAND)  
  {

    if(priority[(*temp_cmd)->type] > priority[cmd_left->type])
    {

      //switch order
      (*temp_cmd)->u.command[0] = cmd_left->u.command[1];
      cmd_left->u.command[1] = *temp_cmd;
      final_cmd = cmd_left;
    }

  }
  
  *temp_cmd = NULL;
  
  return final_cmd;
}

command_t
init_command(void)
{
  command_t cmd = (command_t) checked_malloc(sizeof(struct command));
  cmd->type = SIMPLE_COMMAND;
  cmd->input = NULL;
  cmd->output = NULL;
  return cmd;
}

command_t 
attach_cmd(command_t cur_cmd, char* token){
  command_t tempcmd =  (command_t) checked_malloc(sizeof(struct command));
  tempcmd->input = NULL;
  tempcmd->output = NULL;
  if(isEqual(token,";"))
    tempcmd->type = SEQUENCE_COMMAND;
  else if(isEqual(token,"&&"))
    tempcmd->type = AND_COMMAND;
  else if(isEqual(token,"||"))
    tempcmd->type = OR_COMMAND;
  else if(isEqual(token,"|"))
    tempcmd->type = PIPE_COMMAND;

  else
  tempcmd->type = -1;

  tempcmd->u.command[0]=cur_cmd;
  tempcmd->u.command[1] =NULL;
  return tempcmd;
}


command_t 
parse_command_stream (command_stream_t s, int is_subshell)
{
  if (s ==NULL)
    return NULL;
  size_t count = 0;
  char* cur_word = NULL;

  command_t cur_cmd = init_command();
  command_t temp_cmd = NULL;


  size_t cur_cmd_word_count;
  size_t init_size = 2;

  enum command_state state = INIT_STATUS; 

  while (s->cur_token != NULL)
  {

    cur_word = s->cur_token->word;
    size_t i = 0;
    // while(i < sizeof(cur_word)+1)
    // {
    //   printf("%c", cur_word[i]);
    //   i++;
    // }
    s->cur_token = s->cur_token->next;

    if(isEqual(cur_word, "\n"))
      s->cur_line_num++;

    switch(state)
    {
      printf ("%s\n", cur_word);
      case INIT_STATUS:
      {
      if(isEqual(cur_word, "("))
      {
        state = SUBSHELL_CMD;
      }
      else if (isEqual(cur_word, ")"))
      {
 
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }

      else if(isEqual(cur_word, "\n"))
      {
        state = INIT_STATUS;
      }
      else if (isWordToken(cur_word))
      {

        cur_cmd_word_count = 0;
        cur_cmd->u.word = checked_malloc(sizeof(char*)*init_size); 
        cur_cmd->u.word[cur_cmd_word_count] = (char*) checked_malloc(strlen(cur_word)+1);     
        strcpy(cur_cmd->u.word[cur_cmd_word_count], cur_word);
        cur_cmd_word_count += 1;
        cur_cmd->u.word[cur_cmd_word_count] = NULL;

        state = SIMPLE_CMD_STATUS;
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }

      break;
    }
      case SIMPLE_CMD_STATUS:
      {
    //      printf("%s\n", cur_word);
      if(isEqual(cur_word,"\n")||isEqual(cur_word,";"))
      {
        
        if (!is_subshell)
        {
          if(temp_cmd != NULL)  
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
        }
        else
        {
          if(temp_cmd != NULL)  
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          temp_cmd = attach_cmd (cur_cmd, ";");

          cur_cmd = init_command();
          state = INIT_STATUS;

        }
      }
      else if(isEqual(cur_word,"<"))
      {
        state = IN_CMD_STATUS;
      }
      else if(isEqual(cur_word, ">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isWordToken(cur_word))
      {
        state = SIMPLE_CMD_STATUS;
        cur_cmd->u.word = checked_realloc(cur_cmd->u.word,sizeof(char*)*(cur_cmd_word_count+init_size));
        cur_cmd->u.word[cur_cmd_word_count] = (char*) checked_malloc(strlen(cur_word)+1);     
        strcpy(cur_cmd->u.word[cur_cmd_word_count], cur_word);
        cur_cmd_word_count += 1;
        cur_cmd->u.word[cur_cmd_word_count] = NULL;

      } 
      else if(isConnToken(cur_word))
      {
        if(temp_cmd != NULL)  
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = attach_cmd(cur_cmd, cur_word);
        cur_cmd = init_command();
        state = INIT_STATUS;
    //             print_command(cur_cmd);
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }
      break;
}
      case IN_CMD_STATUS:
      if(isWordToken(cur_word))
      {
        size_t len = strlen(cur_word)+1;
        cur_cmd->input = (char*) checked_malloc (len*sizeof(char));
        memset (cur_cmd->input, 0, len);
        strcpy(cur_cmd->input, cur_word);
        state = IN_CMD_STATUS_FINISH;  
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }
      break;

      case OUT_CMD_STATUS:
      if(isWordToken(cur_word))
      {
        size_t len = strlen(cur_word)+1;
        cur_cmd->output = (char*) checked_malloc (len*sizeof(char));
        memset (cur_cmd->output, 0, len);
        strcpy(cur_cmd->output, cur_word);
        state = OUT_CMD_STATUS_FINISH;
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }
      break;

      case IN_CMD_STATUS_FINISH:

      if(isEqual(cur_word,")"))
      {
        if(temp_cmd != NULL)  
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);

        return cur_cmd; 
      }
      else if(isEqual(cur_word,">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isEqual(cur_word,"\n")||isEqual(cur_word,";"))
      {
          if(temp_cmd != NULL)  
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
          
        
      }
      else if(isConnToken(cur_word))
      {
        if(temp_cmd != NULL)  
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = attach_cmd(cur_cmd, cur_word);     
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }
      break;

      case OUT_CMD_STATUS_FINISH:              
      if(isEqual(cur_word,")"))
      {
        if(temp_cmd != NULL)  
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);

        return cur_cmd; 
      }
      else if(isEqual(cur_word,"\n")||isEqual(cur_word,";"))
      {
        if(temp_cmd != NULL)  
         cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
  

      }
      else if(isConnToken(cur_word))
      {
        if(temp_cmd != NULL)  
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = attach_cmd(cur_cmd, cur_word);                 
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
      }
        break;

      case SUBSHELL_CMD:
       {
        command_stream_t sub_s = (command_stream_t) checked_malloc (sizeof(struct command_stream));
        sub_s->wc = 0;
        sub_s->cur_token = NULL;
        token_t head = NULL;
        while (!isEqual (cur_word, ")"))
        {
         // printf ("%s\n", cur_word);
          char* t = (char*) checked_malloc (strlen(cur_word)+1);
          strcpy(t, cur_word);
          if (head ==NULL)
          {
            sub_s->cur_token = add_token(t);
            head = sub_s->cur_token;
          }
          else
          {
            sub_s->cur_token->next = add_token(t);
            sub_s->cur_token= sub_s->cur_token->next;
          }
          if (isEqual(cur_word, "\n" ))
            s->cur_line_num += 1;
          
           if (s->cur_token == NULL)
             error(1,0, "%d: subshell format error\n", s->cur_line_num);
          cur_word = s->cur_token->word;

           s->cur_token = s->cur_token->next;
    //       printf ("%s\n", s->cur_token->word);
        }
          sub_s->cur_token = head;
          cur_cmd->u.subshell_command  = parse_command_stream (sub_s, 1);
          cur_cmd->type = SUBSHELL_COMMAND;
    //      print_command(cur_cmd);
          state = SIMPLE_CMD_STATUS;
          break;
       }

       default:
        error(1,0, "%d: format error\n", s->cur_line_num);
      }


  }

  s->cur_line_num -= 1; // coz got additional \n from getc func

  if((state == IN_CMD_STATUS_FINISH || state == OUT_CMD_STATUS_FINISH ||
    state == SIMPLE_CMD_STATUS  || state == SUBSHELL_CMD))
  {
    if(temp_cmd != NULL)  
    cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
  //print_command (cur_cmd);
    return cur_cmd;
  }
  else if(state == INIT_STATUS)
  {
    if(temp_cmd!=NULL)
    { 
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
    }
    else
      return NULL; 
  }
  else
  {
        fprintf(stderr, "%d: format error\n", s->cur_line_num);
        exit(1);
  } 
  return NULL;
}

command_t
read_command_stream (command_stream_t s)
{

  return parse_command_stream(s,0);

}

