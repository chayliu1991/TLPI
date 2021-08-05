#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>


#define     POPEN_FMT "/bin/ls -d %s 2 > /dev/null"
#define     PAT_SIZE 50
#define     PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

typedef enum
{
    False = 0,
    True
}Boolean;

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


int main(int argc,char*argv[])
{
    int pat[PAT_SIZE];
    char* popenCmd[PCMD_BUF_SIZE];
    FILE* fp;
    Boolean badPattern;
    int len,status,fileCnt,j;
    char pathname[PATH_MAX];

    for(;;)
    {
        printf("pattern:    ");
        fflush(stdout);
        if(fgets(pat,PAT_SIZE,stdin) == NULL)
            break;
        
        len = strlen(pat);
        if(len <= 1)
        {
            continue;
        }

        if(pat[len -1] == '\n')
            pat[len -1] = '\0';
        
        for(j = 0,badPattern = False;j < len && !badPattern;j++)
        {
            if(!isalnum((unsigned char)pat[j]) && strchr("_*?.",pat[j]) == NULL)
                badPattern = True;
        }

        if(badPattern)
        {
            printf("Bad pattern character: %c \n",pat[j-1]);
            continue;
        }

        /*Build and execute command to glob 'pat'*/

        snprintf(popenCmd,PCMD_BUF_SIZE,POPEN_FMT,pat);
        popenCmd[PCMD_BUF_SIZE] = '\0';

        fp = popen(popenCmd,"r");
        if(fp == NULL)
        {
            printf("popen() failed\n");
            continue;
        }

        /* Read resulting list of pathnames until EOF */
        fileCnt = 0;
        while(fgets(pathname,PATH_MAX,fp) != NULL)
        {
            printf("%s",pathname);
            fileCnt++;
        }

        /* Close pipe,fetch and display termination status*/

        status = pclose(fp);
        printf("    %d matching file %s\n",fileCnt,(fileCnt != 1) ? "s" : " ");
        printf("    pclose() status == %#x\n",(unsigned int) status);
        if(status != -1)
            printWaitStatus("\t",status);
    }

    exit(EXIT_SUCCESS);
}