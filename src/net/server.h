//#define PORT "3491"
#define BACKLOG 10
#define MAXDATASIZE 100
#define MAX_MSG_SIZE 1001

int send_msg_to_host(char *, char *);
int receive_msg_from_host(char *, char *);
int start_listening_on(char *);
char* receive_msg(int);

