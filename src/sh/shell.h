#include <stdio.h>
#include <stdlib.h>

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

char *dash_read_line(void);
char *extract_host(char *);
int dash_eval(char *, char *, char *);
char *dash_exec_scmd(char **, int, int, char *);
dash_exec_t create_exec_t(char **, int, int, char *);
int create_process(int, int, dash_scmd_t);
char *fork_pipe_exec(dash_exec_t);

