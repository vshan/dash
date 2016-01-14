char** str_split(char* a_str, const char a_delim);

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
};

typedef struct dash_exec* dash_exec_t;