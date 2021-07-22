#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>


#define MAX_CMD_LEN (200)

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


void printWaitStatus(const char *msg, int status)
{
    if (msg != NULL)
        printf("%s", msg);

    if (WIFEXITED(status)) {
        printf("child exited, status=%d\n", WEXITSTATUS(status));

    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d (%s)",
                WTERMSIG(status), strsignal(WTERMSIG(status)));
#ifdef WCOREDUMP        /* Not in SUSv3, may be absent on some systems */
        if (WCOREDUMP(status))
            printf(" (core dumped)");
#endif
        printf("\n");

    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n",
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED     /* SUSv3 has this, but older Linux versions and
                           some other UNIX implementations don't */
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
#endif

    } else {            /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
                (unsigned int) status);
    }

    printf("\n");
}

int main(int argc,char* argv[])
{

    char str[MAX_CMD_LEN];
    int status;

    for(;;)
    {
        printf("Command: ");
        fflush(stdout);

        if(fgets(str,MAX_CMD_LEN,stdin) == NULL)
            break;

        status = system(str);
        printf("system() returned : status = 0x%04x (%d%d)\n",(unsigned int) status,status >> 8,status & 0xFF);

        if(status == -1)
            errExit("system()");
        else
        {
            if(WIFEXITED(status) && WEXITSTATUS(status) == 127)
            {
                printf("(Probably) could not invoke shell\n");
            }
            else
            {
                printWaitStatus(NULL,status);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
