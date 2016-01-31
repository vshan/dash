#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define REMOTE_PIPE "~|"
#define REMOTE_OUT  "~>"
#define REMOTE_IN   "<~"
#define PIPE "|"

struct dash_scmd {
  char *name;
  char **args;
};

typedef struct dash_scmd* dash_scmd_t;

struct dash_exec {
  int pipes;
  dash_scmd_t *commands;
  char *std_input;
};

typedef struct dash_exec* dash_exec_t;

// #include "util/util.h"
// #include "net/server.h"
// #include "sh/builtin.h"

// int dash_eval(char *);
// char *extract_host(char *);
// int dash_eval(char *, char *, char *);
char* dash_exec_scmd(char **, int, int, char *);
dash_exec_t create_exec_t(char **, int, int, char *);
int create_process(int, int, dash_scmd_t);
char *fork_pipe_exec(dash_exec_t);
char **strsplit(char *, const char *, int *);
void print_dec(dash_exec_t);

// char *dash_read_line()
// {
//   char *line = NULL;
//   ssize_t bufsize = 0; // have getline allocate a buffer for us
//   getline(&line, &bufsize, stdin);
//   return line;
// }

// char *extract_host(char *string)
// {
//   char **strings = str_split(string, ':');
//   return strings[0];
// }

// int dash_eval(char *line, char *std_input, char *origin)
// {
//   if (line == NULL)
//     return 1;
  
//   char **tokens = str_split(line, ' ');
//   char *token;
//   int i;
//   int num_tokens = no_of_sub_strings(tokens);
//   int remote_pipe_pos = -1;

//   for (i = 0; i < num_tokens; i++) {
//     if (strcmp(tokens[i], REMOTE_PIPE) == 0) {
//       remote_pipe_pos = i;
//       break;
//     }
//   }

//   if (remote_pipe_pos == -1) // No remote pipes
//   {
//     char *msg_data = dash_exec_scmd(tokens, remote_pipe_pos + 1, num_tokens, std_input);
//     if (std_input == NULL) {
//       printf("%s\n", msg_data);
//       return 1;
//     }
//     else { // Send to origin
//       char *fio_msg = dashp_fio(msg_data);
//       send_to_host(fio_msg, origin);
//       return 1;
//     }
//   }
//   else // Remote pipe at remote_pipe_pos
//   {
//     char *subcommand = join_strings(tokens, ' ', remote_pipe_pos+1, num_tokens);
//     char *remotehost = extract_host(subcommand);
//     char *maincommand = join_strings(tokens, ' ', 0, remote_pipe_pos);
//     char *msg_data = dash_exec_scmd(tokens, 0, remote_pipe_pos, std_input);
//     char *send_msg;
//     if (std_input == NULL) {
//       char *self_origin = get_my_ip_addr();
//       send_msg = dashp_pip(msg_data, self_origin, subcommand);
//     }
//     else {
//       send_msg = dashp_pip(msg_data, origin, subcommand);
//     }
//     send_to_host(send_msg, remotehost);
//     return 2;
//   }
// }

int main() {
  int num;
  char **tokens = strsplit("echo 2+5 | bc | grep 7 | cat", " ", &num);
  //dash_exec_scmd(tokens, 0, num, NULL);
  print_dec(create_exec_t(tokens, 0, num, NULL));
}

void print_dec(dash_exec_t det)
{
  int i;
  for (i = 0; i <= det->pipes; i++) {
    printf("Name of binary: %s, args passed: ", det->commands[i]->name);
    int j = 0;
    while(det->commands[i]->args[j]) {
      printf("%d: `%s` ", j, det->commands[i]->args[j++]);
    }
    printf("\n");
  }
}

char* dash_exec_scmd(char **tokens, int start, int fin, char *std_input)
{   
  char *std_output;
  //validate_command(tokens, num);
  dash_exec_t dash_cmd = create_exec_t(tokens, start, fin, std_input);

  std_output = fork_pipe_exec(dash_cmd);

  return std_output;
}

dash_exec_t create_exec_t(char **tokens, int start, int fin, char *std_input)
{
  int i, j, b, no_of_pipes = 0;
  for (i = start; i < fin; i++) {
    if (strcmp(tokens[i], PIPE) == 0)
      no_of_pipes++;
  }

  dash_exec_t dash_cmd = (dash_exec_t) malloc (sizeof(struct dash_exec));
  dash_cmd->pipes = no_of_pipes;
  dash_cmd->std_input = std_input;
  dash_cmd->commands = (dash_scmd_t *) malloc (sizeof(dash_scmd_t) * (no_of_pipes + 1));

  int count = 0;
  for (i = start; i < fin; ) {
    for (j = i, b = 0; ((j < fin) && (strcmp(tokens[j], PIPE) != 0)); j++, b++) {
      if (j == i) {
        //printf("%s\n", tokens[j]);

        //int size_d = sizeof((char *)dash_cmd->commands[count]->name);
        //printf("%d\n", size_d);
        dash_cmd->commands[count] = (dash_scmd_t) malloc (sizeof(struct dash_scmd));
        //strcpy(dash_cmd->commands[count]->name, tokens[j]);
        dash_cmd->commands[count]->name = tokens[j];
        dash_cmd->commands[count]->args = (char **) malloc(sizeof(char *) * 10);
        //strcpy(dash_cmd->commands[count]->name, tokens[j]);
        //dash_cmd->commands[count]->name = strdup("gaal");
        //printf("%s\n", dash_cmd->commands[count]->name);
      }
      dash_cmd->commands[count]->args[b] = tokens[j];
      dash_cmd->commands[count]->args[b+1] = NULL;
    }
    count++;
    i = j + 1;
  }

  return dash_cmd;
}

int create_process(int in, int out, dash_scmd_t scmd)
{
    pid_t pid;

    if ((pid = fork()) == 0) {
        if (in != 0) {
          dup2 (in, 0);
          close (in);
        }

        if (out != 1) {
          dup2 (out, 1);
          close (out);
        }

        return execvp(scmd->name, scmd->args);
    }

    return pid;
}

char *fork_pipe_exec(dash_exec_t cmds)
{
    int i;
    pid_t pid;
    int in, fd[2], fd2[2], fd3[2];
    char *std_output;

    if (cmds->std_input == NULL) {
      in = STDIN_FILENO;
    }
    else {
      pipe(fd2);
      write(fd2[1], cmds->std_input, strlen(cmds->std_input));
      close(fd2[1]);
      in = fd2[0];
    }

    for (i = 0; i < cmds->pipes; ++i) {
        pipe(fd);
        create_process(in, fd[1], cmds->commands[i]);
        //close(fd[1]);
        in = fd[0];
    }
    

    if (!fork()) { //child process
        dup2(fd3[1], 1);
        if (in != 0)
          dup2(in, 0);
        //close(fd3[1]);
        //close(in);
        //open("/var/tmp/output", "rb");
        //if (is_builtin(cmds)) {
        //    run_builtins(cmds);
        //    exit(0);
        //} else {
        execvp(cmds->commands[i]->name, cmds->commands[i]->args);
        //}
    } else wait(NULL);

    // or instead of files, use pipe and read pipe to
    // the buffer std_output

    // FILE *f = fopen("/var/tmp/output", "rb");
    // fseek(f, 0, SEEK_END);
    // long fsize = ftell(f);
    // fseek(f, 0, SEEK_SET);

    std_output = malloc(1000);
    // fread(std_output, fsize, 1, f);
    // fclose(f);
    
    int num_read = read(fd3[0], std_output, 1000);
    std_output[num_read] = '\0';

    return std_output;
}

char **
strsplit(char *string, const char *delim, int *num_split)
{
  int str_len = strlen(string);
  int flag = 1;
  int i;
  int num_sub_strings = 0;
  char **substrings = (char **) malloc (sizeof(char *) * str_len);
  char *start = string;
  char *end = strstr(string, delim);

  if (start == end) {
    start += strlen(delim);
    end += strlen(delim);
    end = strstr(end, delim);
  }

  while (end != NULL)
  {
    if (flag) {
      substrings[num_sub_strings] = (char *) malloc (sizeof(char) * str_len);
      i = 0;
      flag = 0;
    }

    substrings[num_sub_strings][i++] = *start;
    start++;

    if (start == end) {
      start += strlen(delim);
      end += strlen(delim);
      end = strstr(end, delim);
      flag = 1;
      substrings[num_sub_strings][i] = '\0';
      num_sub_strings++;
      if (end == NULL) {
        substrings[num_sub_strings++] = strdup(start);
        break;
      }
    }

  }
  *num_split = num_sub_strings;
  return substrings;
}