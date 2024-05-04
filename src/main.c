#include <stdio.h>
#include <stdlib.h>
#include <malloc/malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define DSH_RL_BUFSIZE 1024
#define DSH_TOK_BUFSIZE 64
#define DSH_TOK_DELIM " \t\r\n\a"

void dsh_loop(void);
char *dsh_read_line();
char **dsh_split_line(char *line);
int dsh_launch(char **args);

/*
  Function Declarations for builtin shell commands:
 */
int dsh_cd(char **args);
int dsh_help(char **args);
int dsh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[])(char **) = {
    &dsh_cd,
    &dsh_help,
    &dsh_exit,
};

int dsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int main(int argc, char **argv)
{
    dsh_loop();

    return 0;
}

int dsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "dsh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("dsh");
        }
    }
    return 1;
}

int dsh_help(char **args)
{
    int i;

    printf("Daksh's DingDong Shell\n");
    printf("Enter program name and arguments, and press enter\n");
    printf("The following are built in:\n");

    for (i = 0; i < dsh_num_builtins(); i++)
    {
        printf("    %s\n", builtin_str[i]);
    }
    printf("Enter man command for information on other programs\n");
    return 1;
}

int dsh_exit(char **args)
{
    return 0;
}

void dsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = dsh_read_line();
        args = dsh_split_line(line);
        status = dsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *dsh_read_line(void)
{
    int bufSize = DSH_RL_BUFSIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufSize);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "dsh: allocation error\n");
        exit(1);
    }

    while (1)
    {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[pos] = '\0';
            return buffer;
        }
        else
        {
            buffer[pos] = c;
        }
        pos++;

        if (pos >= bufSize)
        {
            bufSize += DSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufSize);
            if (!buffer)
            {
                fprintf(stderr, "dsh: allocation error\n");
                exit(1);
            }
        }
    }
}

char **dsh_split_line(char *line)
{
    int bufSize = DSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(sizeof(char *) * bufSize);
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "dsh: allocation error\n");
        exit(1);
    }

    token = strtok(line, DSH_TOK_DELIM);

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufSize)
        {
            bufSize += DSH_RL_BUFSIZE;
            tokens = realloc(tokens, bufSize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "dsh: allocation error\n");
                exit(1);
            }
        }
        token = strtok(NULL, DSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int dsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("dsh");
        }
        else
        {
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 1;
}