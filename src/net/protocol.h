enum dashp_state { DASHP_PIP, DASHP_FIO };

#define DASHP_DELIM "!@#$%"
#define DASHP_BREAK "\n\n\n\n\n"

struct dashp_msg {
  dashp_state dps;
  char *command;
  char *payload;
  char *origin;
};

typedef struct dashp_msg* dashp_msg_t;

dashp_msg_t extract_dashp(char *msg);

char *make_dashp(dashp_msg_t);