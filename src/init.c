#include <net/server.h>
#include <sh/shell.h>

int main(int argc, char *argv[]) {
   
  char *prompt = "~$";
  char *reg_msg;
  pid_t pid;
  
  // Ensure no. of arguments are correct
  if (argc != 3) {
    fprintf(stderr, "usage: dash <IP address> <hostname>\n");
    return 0;
  }

  reg_msg = dashp_register(argv[2]);

  dashp_send(reg_msg, argv[1]);

  pid = fork();

  if (pid == 0) { // child process
    // listening process
  }

  while (1) {
    printf("%s ", promt);
    char *line = read_line();
    dash_eval(line);
  }

  return 0;
} 