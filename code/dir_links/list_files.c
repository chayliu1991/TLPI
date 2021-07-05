#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>


#define CMD_SIZE 200
#define BUF_SIZE 1024


typedef enum 
{
    False = 0,
    True    
}Boolean;

void errExit(const char* call)
{
    perror(call);
    exit(1);
}

static void listFiles(const char* dirpath)
{
    DIR* dirp;
    struct dirent* dp;
    Boolean isCurrent;

    isCurrent = strcmp(dirpath,".") == 0;
    dirp = opendir(dirpath);
    if(dirp == NULL)
    {
        errExit("opendir");
    }

    for(;;)
    {
        errno = 0;
        dp = readdir(dirp);
        if(dp == NULL)
            break;
        
        if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
            continue;
        
        if(!isCurrent)
            printf("%s",dirpath);
        printf("%s\n",dp->d_name);
    }

    
    if(errno != 0)
            errExit("readdir");
    
    if(closedir(dirp) < 0)
            errExit("closedir");
}

int main(int argc,char* argv[])
{
    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [dir...] \n",argv[0]);
        exit(1);
    }

    if(argc == 1)
        listFiles(".");
    else
    {
        for(argv++;*argv;argv++)
        {
             listFiles(*argv);
        }
    }
    
    exit(EXIT_SUCCESS);
}