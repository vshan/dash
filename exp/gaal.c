#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int
main()
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