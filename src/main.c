#include <net/server.h>
#include <sh/shell.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
   
  char *prompt = "~$";
  char *reg_msg;
  pid_t pid;
  
  // Ensure no. of arguments are correct
  if (argc != 3) {
    fprintf(stderr, "usage: dash <IP address> <hostname>\n");
    return 0;
  }

  int fd[2];

  pipe(fd);

  pid = fork();

  if (pid == 0) { // child process
    // listening process
    int sockfd = start_listening_on(PORT);
    while (1) {
      protocol_msg = receive_msg(sockd);
      dashp_msg dmsg = extract_dashp(protocol_msg);
      if (dmsg->dps == DASHP_FIO) { 
        write(fd[0]); // write dmsg->payload to fd[0]
      }
      else if (dmsg->dsp == DASHP_PIP) {
        char *command = dmsg->command;
        dash_eval(command, dmg->payload);
      }
    }

  }
  else { // parent process
    while (1) {
      printf("%s ", promt);
      char *line = dash_read_line();
      rv = dash_eval(line, NULL);
      if (rv == INVOLVED_NETWORKING)
        read(fd[1]);
    }
  }

  return EXIT_SUCCESS;
} 