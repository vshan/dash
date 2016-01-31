#include <stdlib.h>
#include <stdio.h>

#include "util/util.h"

char *join_strings(char **tokens, char *delim, int start, int fin)
{
  int i, len = 0;
  
  for (i = start; i < fin; i++)
  {
    len += strlen(tokens[i]);
    len += strlen(delim);
  }

  char *dest = (char *) malloc (sizeof char * len);

  for (i = start; i < fin; i++)
  {
    strcat(dest, tokens[i]);
    strcat(dest, delim);
  }

  return dest;
}

char *extract_host(char *string)
{
  int x;
  char **strings = str_split(string, ":", &x);
  return strings[0];
}

char **str_split(char *string, const char *delim, int *num_split)
{
  int str_len = strlen(string);
  int flag = 1;
  int i;
  int num_sub_strings = 0;
  char **substrings = (char **) malloc (sizeof(char *) * str_len);
  char *start = string;
  char *end = strstr(string, delim);

  if (start == end) {
    start += strlen(delim);
    end += strlen(delim);
    end = strstr(end, delim);
  }

  while (end != NULL)
  {
    if (flag) {
      substrings[num_sub_strings] = (char *) malloc (sizeof(char) * str_len);
      i = 0;
      flag = 0;
    }

    substrings[num_sub_strings][i++] = *start;
    start++;

    if (start == end) {
      start += strlen(delim);
      end += strlen(delim);
      end = strstr(end, delim);
      flag = 1;
      substrings[num_sub_strings][i] = '\0';
      num_sub_strings++;
      if (end == NULL) {
        substrings[num_sub_strings++] = strdup(start);
        break;
      }
    }

  }
  *num_split = num_sub_strings;
  return substrings;
}