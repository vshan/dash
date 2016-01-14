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

char** tokenize(char *line)
{
    char **tokens = str_split(line, ' ');
    char *token;
    int i;
    int num_tokens = no_of_sub_strings(tokens);

    for (i = 0; i < num_tokens; i++) {
        if (strcmp(tokens[i], REMOTE_PIPE) == 0) {
            if (i == 0) {
                fprintf(stderr, "Please ensure there is expression behind remote pipe.\n", );
                return 0;
            }
            else if (i == num_tokens) {
                fprintf(stderr, "Please ensure there is expression after remote pipe.\n", );
                return 0;
            }
            char *subcommand = join_strings(tokens, ' ', i+1, num_tokens);
            char *remotehost = extract_host(subcommand);
            char *maincommand = join_strings(tokens, ' ', 0, i);
            char *msg_data = dash_exec_scmd(maincommand_tokens, num_mc_tokens);
            char *pip_msg = dashp_pip(msg_data, subcommand, remotehost);
            send_to_host(pip_msg, remotehost);
            
        }
    }
}

char* dash_exec_scmd(char **tokens, int num)
{   
    char *std_output;
    validate_command(tokens, num);
    dash_exec_t dash_cmd = create_exec_t(tokens, num);

    std_output = fork_pipe_exec(dash_cmd);

    return std_output;
}

int create_process(int in, int out, dash_scmd_t scmd)
{
    pid_t pid;

    if ((pid = fork()) == 0) {
        if (in != 0) {
          dup2 (in, 0);
          close (in);
        }

        if (out != 1) {
          dup2 (out, 1);
          close (out);
        }

        return execvp(scmd->name, scmd->args);
    }

    return pid;
}

int fork_pipe_exec(dash_exec_t cmds)
{
    int i;
    pid_t pid;
    int in, fd[2];
    char *std_output;

    in = 0;

    for (i = 0; i < cmds->pipes; ++i) {
        pipe(fd);
        create_process(in, fd[1], cmds->commands + i);
        close(fd[1]);
        in = fd[0];
    }
    
    if (in != 0)
        dup2(in, 0);

    if (!fork()) { //child process
        close(STDOUT_FILENO);
        open("/var/tmp/output", "rb");
        if (is_builtin(cmds)) {
            run_builtins(cmds);
            exit(0);
        } else {
            execvp(cmds->commands[i]->name, 
                   cmds->commands[i]->args);
        }
    } else wait(NULL);

    FILE *f = fopen("/var/tmp/output", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    std_output = malloc(fsize + 1);
    fread(std_output, fsize, 1, f);
    fclose(f);

    std_output[fsize] = 0;

    return std_output;
}