#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "sh/shell.h"
//#include "sh/builtin.h"
#include "net/server.h"
#include "net/protocol.h"
#include "util/util.h"

#define MAX_BUFFER_SIZE 1000000

#ifndef INVOLVED_NETWORKING
#define INVOLVED_NETWORKING 2

int main(int argc, char *argv[]) {
   
  char prompt[] = "~$";
  char *reg_msg;
  pid_t pid;
  char buffer[MAX_BUFFER_SIZE];
  
  // Ensure no. of arguments are correct
  // if (argc != 3 || (strcmp(argv[1], "--help") == 0)) {
  //   fprintf(stderr, "usage: dash <IP address> <hostname>\n");
  //   return 0;
  // }

  int fd[2];

  pipe(fd);

  pid = fork();

  if (pid == 0) { // child process
    // listening process
    int sockfd = start_listening_on(PORT);
    while (1) {
      char *protocol_msg = receive_msg(sockfd);
      dashp_msg_t dmsg = extract_dashp(protocol_msg);
      if (dmsg->dps == DASHP_FIO) { 
        write(fd[1], dmsg->payload, strlen(dmsg->payload)); // write dmsg->payload to fd[1]
      }
      else if (dmsg->dps == DASHP_PIP) {
        char *command = dmsg->command;
        dash_eval(command, dmsg->payload, dmsg->origin);
      }
    }

  }
  else { // parent process
    while (1) {
      printf("%s ", prompt);
      char *line = dash_read_line();
      int rv = dash_eval(line, NULL, NULL);
      if (rv == INVOLVED_NETWORKING) { // involved networking
        read(fd[0], buffer, MAX_BUFFER_SIZE);
        printf("%s\n", buffer);
      }
    }
  }

  return EXIT_SUCCESS;
} 

#endif