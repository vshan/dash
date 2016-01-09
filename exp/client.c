#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3491" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define MAX_MSG_SIZE 1001

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(argv[1], PORT, &hints, &servinfo);

    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                servinfo->ai_protocol);

    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    //close(sockfd);
    
    inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)servinfo->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    send(sockfd, argv[2], strlen(argv[2]), 0);

    char *msg = (char *) malloc (MAX_MSG_SIZE);
    for (;;) {
        get_in:
        printf("\n> ");
        fgets (msg, MAX_MSG_SIZE, stdin);
        if ((strlen(msg)>0) && (msg[strlen (msg) - 1] == '\n'))
          msg[strlen (msg) - 1] = '\0';
        if (send(sockfd, msg, strlen(msg), 0) == -1) {
            printf("error in send");
            goto get_in;
        }
        printf("waiting for reply...\n");
        numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
        buf[numbytes] = '\0';
        printf("chetan: %s\n", buf);
    }

    close(sockfd);

    return 0;
}