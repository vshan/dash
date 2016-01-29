// include file for struct sockaddr

//#define PORT "3491"
#define BACKLOG 10
#define MAXDATASIZE 100
#define MAX_MSG_SIZE 1001

void *get_in_addr(struct sockaddr *);
int send_to_host(char *, char *);
char *get_my_ip_addr(void);
int start_listening_on(char *);
char* receive_msg(int);

