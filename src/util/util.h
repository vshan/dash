char** str_split(char* a_str, const char a_delim);
int no_of_sub_strings(char **);
char *join_strings(char **, char, int, int);
char *extract_host(char *);

#define REMOTE_PIPE "~|"
#define REMOTE_OUT  "~>"
#define REMOTE_IN   "<~"

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