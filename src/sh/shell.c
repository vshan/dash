#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "util/util.h"
#include "net/server.h"
#include "sh/shell.h"

char *dash_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  getline(&line, &bufsize, stdin);
  return line;
}

int dash_eval(char *line, char *std_input, char *origin)
{
  if (line == NULL)
    return 1;

  int num_tokens;
  char **tokens = str_split(line, " ", &num_tokens);
  char *token;
  int i;
  int remote_pipe_pos = -1;

  for (i = 0; i < num_tokens; i++) {
    if (strcmp(tokens[i], REMOTE_PIPE) == 0) {
      remote_pipe_pos = i;
      break;
    }
  }

  if (remote_pipe_pos == -1) // No remote pipes
  {
    char *msg_data = dash_exec_scmd(tokens, remote_pipe_pos + 1, num_tokens, std_input);
    if (std_input == NULL) {
      printf("%s\n", msg_data);
      return 1;
    }
    else { // Send to origin
      char *fio_msg = dashp_fio(msg_data);
      send_to_host(fio_msg, origin);
      return 1;
    }
  }
  else // Remote pipe at remote_pipe_pos
  {
    char *subcommand = join_strings(tokens, " ", remote_pipe_pos+1, num_tokens);
    char *remotehost = extract_host(subcommand);
    char *maincommand = join_strings(tokens, " ", 0, remote_pipe_pos);
    char *msg_data = dash_exec_scmd(tokens, 0, remote_pipe_pos, std_input);
    char *send_msg;
    if (std_input == NULL) {
      char *self_origin = get_my_ip_addr();
      send_msg = dashp_pip(msg_data, self_origin, subcommand);
    }
    else {
      send_msg = dashp_pip(msg_data, origin, subcommand);
    }
    send_to_host(send_msg, remotehost);
    return 2;
  }
}

char *dash_exec_scmd(char **tokens, int start, int fin, char *std_input)
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
  for (i = start; i < fin; i++) {
    for (j = i, b = 0; strcmp(tokens[j], PIPE) != 0; j++, b++) {
      if (j == i) {
        dash_cmd->commands[count] = (dash_scmd_t) malloc (sizeof(struct dash_scmd));
        dash_cmd->commands[count]->name = tokens[j];
        dash_cmd->commands[count]->args = (char **) malloc(sizeof(char *) * 10);
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
      in = 0;
    }
    else {
      pipe(fd);
      write(fd[1], cmds->std_input, strlen(cmds->std_input));
      close(fd[1]);
      in = fd[0];
    }

    for (i = 0; i < cmds->pipes; ++i) {
        pipe(fd);
        create_process(in, fd[1], cmds->commands[i]);
        close(fd[1]);
        in = fd[0];
    }
    
    if (in != 0)
        dup2(in, 0);

    if (!fork()) { //child process
      dup2(fd3[1], 1);
      if (in != 0)
        dup2(in, 0);
        //if (is_builtin(cmds)) {
        //  run_builtins(cmds);
        //    exit(0);
        //} else {
      execvp(cmds->commands[i]->name, cmds->commands[i]->args);
    } else wait(NULL);

    std_output = malloc(1000);

    int num_read = read(fd3[0], std_output, 1000);
    std_output[num_read] = '\0';

    return std_output;
}