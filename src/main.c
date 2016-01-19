#include <net/server.h>
#include <sh/shell.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1000000

int main(int argc, char *argv[]) {
   
  char *prompt = "~$";
  char *reg_msg;
  pid_t pid;
  char buffer[MAX_BUFFER_SIZE];
  
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
        write(fd[1], dmsg->payload, strlen(dmsg->payload)); // write dmsg->payload to fd[1]
      }
      else if (dmsg->dsp == DASHP_PIP) {
        char *command = dmsg->command;
        dash_eval(command, dmg->payload, dmg->origin);
      }
    }

  }
  else { // parent process
    while (1) {
      printf("%s ", promt);
      char *line = dash_read_line();
      rv = dash_eval(line, NULL, NULL);
      if (rv == 2) { // involved networking
        read(fd[0], buffer, MAX_BUFFER_SIZE);
        printf("%s\n", buffer);
      }
    }
  }

  return EXIT_SUCCESS;
} 