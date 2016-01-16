#include "protocol.h"
#include <string.h>

dashp_msg_t extract_dashp(char* msg)
{
  dashp_msg_t proto_msg = (dashp_msg_t) malloc (sizeof struct dashp_msg);
  char **msg_split = str_split(msg, DASHP_DELIM);
  int i;
  if (strcmp(msg_split[0], 'PIP') == 0) 
  {
    proto_msg->dps = DASHP_PIP;
    proto_msg->command = strdup(msg_split[1]);
    proto_msg->origin = strdup(msg_split[2]);
    proto_msg->payload = strdup(msg_split[3]);
  }
  else if (strcmp(msg_split[0], 'FIO') == 0)
  {
    proto_msg->dps = DASHP_FIO;
    proto_msg->command = NULL;
    proto_msg->origin = NULL;
    proto_msg->payload = msg_split[1];
  }
  else {
    // error handle
  }
  return proto_msg;
}

char* make_dashp(dashp_msg_t proto_msg)
{
  char *msg;
  if (proto_msg->dps == DASHP_PIP)
  {
    strcat(msg, 'PIP');
    strcat(msg, DASHP_DELIM);
    strcat(msg, proto_msg->command);
    strcat(msg, DASHP_DELIM);
    strcat(msg, proto_msg->origin);
    strcat(msg, DASHP_DELIM);
    strcat(msg, proto_msg->payload);
    //strcat(msg, DASHP_BREAK); 
  }
  else if (proto_msg->dps == DASHP_FIO)
  {
    strcat(msg, 'FIO');
    strcat(msg, DASHP_DELIM);
    strcat(msg, proto_msg->payload);
    //strcat(msg, DASHP_BREAK);
  }
  else {
    //error handle
  }
  return msg;
}