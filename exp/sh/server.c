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
#include <builtin.h>

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

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
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
        numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0);
        buf[numbytes] = '\0';
        printf("%s: %s\n", name, buf);
        char **tokens = str_split(buf, ' ');
        check_for_builtins(tokens);
        if (!fork()) { // child process
          close(new_fd);
          close(STDOUT_FILENO);
          open("./output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
          int i;
          for (i = 0; *(tokens + i); i++);
            *(tokens + i) = NULL;
          execvp(tokens[0], tokens);
        } else wait(NULL);
        // printf("> ");
        // fgets (msg, MAX_MSG_SIZE, stdin);
        // if ((strlen(msg)>0) && (msg[strlen (msg) - 1] == '\n'))
        //   msg[strlen (msg) - 1] = '\0';

        FILE *f = fopen("./output", "rb");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buf2 = malloc(fsize + 1);
        fread(buf2, fsize, 1, f);
        fclose(f);

        buf2[fsize] = 0;
        send(new_fd, buf2, strlen(buf2), 0);
        free(buf2);
      }
      close(new_fd);
      exit(0);
    }
    close(new_fd);
  }

  return 0;
}