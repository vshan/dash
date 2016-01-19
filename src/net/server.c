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
#include <assert.h>
#include <fcntl.h>
#include "server.h"

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int start_listening_on(char *port)
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
  getaddrinfo(NULL, port, &hints, &servinfo);
  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  freeaddrinfo(servinfo);
  listen(sockfd, BACKLOG);
  return sockfd;
}

char *receive_msg(int sockfd)
{
  char buf[100];
  int numbytes, new_fd;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
  sin_size = sizeof their_addr;
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

  inet_ntop(their_addr.ss_family, 
    get_in_addr((struct sockaddr *)&their_addr),
    s, sizeof s);

  char name[100];
  close(sockfd);
  numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0);
  buf[numbytes] = '\0';
  close(new_fd);
  return buf;
}

int send_to_host(char *msg, char *remotehost)
{
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(remotehost, PORT, &hints, &servinfo);
  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
              servinfo->ai_protocol);
  connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)servinfo->ai_addr),
          s, sizeof s);
  freeaddrinfo(servinfo);
  send(sockfd, msg, strlen(msg), 0);
  close(sockfd);
  return 0;
}

char *get_my_ip_addr()
{
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  ifr.ifr_addr.sa_family = AF_INET;

  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
  goto eth0;
 
  wlan0:
  strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
 
  eth0:
  ioctl(fd, SIOCGIFADDR, &ifr);

  char *result = (char *) malloc (sizeof(char) * 100);

  result = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

  if (strcmp(result, "0.0.0.0") == 0)
    goto wlan0;
  
  close(fd);
  return result;
}