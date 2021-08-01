#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#define BD_NO_CHDIR           01
#define BD_NO_CLOSE_FILES     02
#define BD_NO_REOPEN_STD_FDS  04
#define BD_NO_UMASK0         010
#define BD_MAX_CLOSE        8192

static const char* LOG_FILE = "/tmp/ds.log";
static const char* CONFIG_FILE = "/tmp/ds.conf";

static volatile sig_atomic_t hupReceived = 0;

static FILE *logfp;

static void logMessage(const char *format, ...)
{
    va_list argList;
    const char *TIMESTAMP_FMT = "%F %X";
#define TS_BUF_SIZE sizeof("YYYY-MM-DD HH:MM:SS")
    char timestamp[TS_BUF_SIZE];
    time_t t;
    struct tm *loc;

    t = time(NULL);
    loc = localtime(&t);
    if (loc == NULL ||
           strftime(timestamp, TS_BUF_SIZE, TIMESTAMP_FMT, loc) == 0)
        fprintf(logfp, "???Unknown time????: ");
    else
        fprintf(logfp, "%s: ", timestamp);

    va_start(argList, format);
    vfprintf(logfp, format, argList);
    fprintf(logfp, "\n");
    va_end(argList);
}


static void logOpen(const char *logFilename)
{
    mode_t m;

    m = umask(077);
    logfp = fopen(logFilename, "a");
    umask(m);

    if (logfp == NULL)
        exit(EXIT_FAILURE);

    setbuf(logfp, NULL); 
    logMessage("Opened log file");
}


static void logClose(void)
{
    logMessage("Closing log file");
    fclose(logfp);
}

static void readConfigFile(const char *configFilename)
{
    FILE *configfp;
#define SBUF_SIZE 100
    char str[SBUF_SIZE];

    configfp = fopen(configFilename, "r");
    if (configfp != NULL) 
    {
        if (fgets(str, SBUF_SIZE, configfp) == NULL)
            str[0] = '\0';
        else
            str[strlen(str) - 1] = '\0'; 
        logMessage("Read config file: %s", str);
        fclose(configfp);
    }
}

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int becomeDaemon(int flags)
{
    int maxfd,fd;

    switch(flags)
    {
    case -1: 
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if(setsid() == -1)
    {
        return -1;
    }

    switch(fork())
    {
    case -1: 
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if(!(flags & BD_NO_UMASK0))
        umask(0);
    
    if(!(flags & BD_NO_CHDIR))
        chdir("/");
    
    if(!(flags & BD_NO_CLOSE_FILES))
    {
        maxfd = sysconf(_SC_OPEN_MAX);
        if(maxfd == -1)
            maxfd = BD_MAX_CLOSE;
        
        for(fd = 0;fd < maxfd;fd++)
            close(fd);
    }

    if(!(flags & BD_NO_REOPEN_STD_FDS))
    {
        close(STDIN_FILENO);

        fd = open("/dev/null",O_RDWR);

        if(fd != STDIN_FILENO)
            return -1;
        
        if(dup2(STDIN_FILENO,STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        
        if(dup2(STDIN_FILENO,STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}

static void sighupHandler(int sig)
{
    hupReceived = 1;
}


int main(int argc,char* argv[])
{
    const int SLEEP_TIME = 15;
    int count = 0;
    int unslept;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sighupHandler;

    if(sigaction(SIGHUP,&sa,NULL) == -1)
        errExit("sigaction()");

    if(becomeDaemon(0) == -1)
        errExit("becomeDaemon()");
    
    logOpen(LOG_FILE);
    readConfigFile(CONFIG_FILE);

    unslept = SLEEP_TIME;

    for(;;)
    {
        unslept = sleep(unslept);

        if(hupReceived)
        {
            logClose();
            logOpen(LOG_FILE);
            readConfigFile(CONFIG_FILE);
            hupReceived = 0;
        }

        if(unslept == 0)
        {
            count ++;
            logMessage("Main:%d",count);
            unslept = SLEEP_TIME;
        }          
    }
}