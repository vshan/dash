#include <stdio.h>
#include <stdlib.h>

char **str_split(char *, const char *, int *);
char *join_strings(char **, char *, int, int);
char *extract_host(char *);
void remove_substring(char *, const char *);