#include <shell.h>
#include "lib/util.h"

int dash_eval(char *line)
{
  if (line == NULL)
  	return 1;

  char **tokens = tokenize(line);
}