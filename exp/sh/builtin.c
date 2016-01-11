#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <builtin.h>

char *builtin_names[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_functions[]) (char **) = {
  &dash_cd,
  &dash_help,
  &dash_exit
};

int dash_builtin_num() {
  return sizeof(builtin_names) / sizeof(char *);
}

int dash_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "dash: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("dash");
    }
  }
  return 1;
}

int dash_help(char **args)
{
  int i;
  printf("Vinay Bhat's dash\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < dash_builtin_num(); i++) {
    printf("  %s\n", builtin_names[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int dash_exit(char **args) {
  return 0;
}

int check_for_builtins(char **args)
{
  int i;
  for (i = 0; i < dash_builtin_num(); i++)
  {
    if (strcmp(args[0], builtin_names[i]) == 0) {
      return (*builtin_functions[i])(args);
    }
  }
}