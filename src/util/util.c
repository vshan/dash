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

int no_of_sub_strings(char **strings)
{
  int i;
  for (i = 0; strings[i] != NULL, i++);
  return i;
}

char *join_strings(char **tokens, char delim, int start, int fin)
{
  int i, len = 0;
  
  for (i = start; i < fin; i++)
  {
    len += strlen(tokens[i]);
    len += 1;
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
  char **strings = str_split(string, ':');
  return strings[0];
}

char **
strsplit(char *string, const char *delim, int *num_split)
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