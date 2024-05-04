#include <stdio.h>
#include <stdlib.h>
#include <malloc/malloc.h>

#define DSH_RL_BUFSIZE 1024

void dsh_loop(void);
char *dsh_read_line();

int main(int argc, char **argv)
{

    dsh_loop();

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