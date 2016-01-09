#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3491"
#define BACKLOG 10
#define MAXDATASIZE 100
#define MAX_MSG_SIZE 1001

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  char buf[100];
  int numbytes;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
 
  getaddrinfo(NULL, PORT, &hints, &servinfo);

  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

  freeaddrinfo(servinfo);

  listen(sockfd, BACKLOG);

  printf("server: waiting for connections...\n");

  while(1) {
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

    inet_ntop(their_addr.ss_family, 
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);

    printf("server: got connection from %s\n", s);
    char name[100];
    char *msg = (char *) malloc (MAX_MSG_SIZE);
    if (!fork()) {
      close(sockfd);
      send(new_fd, "hello, world!", 13, 0);
      numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0);
      buf[numbytes] = '\0';
      printf("client %s registered\n", buf);
      strcpy(name, buf);
      for (;;) {
        printf("waiting for reply...\n");
        numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0);
        buf[numbytes] = '\0';
        printf("%s: %s\n", name, buf);
        printf("> ");
        fgets (msg, MAX_MSG_SIZE, stdin);
        if ((strlen(msg)>0) && (msg[strlen (msg) - 1] == '\n'))
          msg[strlen (msg) - 1] = '\0';
        send(new_fd, msg, strlen(msg), 0);
      }
      close(new_fd);
      exit(0);
    }
    close(new_fd);
  }

  return 0;
}