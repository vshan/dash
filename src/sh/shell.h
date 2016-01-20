int dash_eval(char *);
char *extract_host(char *);
int dash_eval(char *, char *, char *);
char* dash_exec_scmd(char **, int, int, char *);
dash_exec_t create_exec_t(char **, int, int, char *);
int create_process(int, int, dash_scmd_t);
int fork_pipe_exec(dash_exec_t);
