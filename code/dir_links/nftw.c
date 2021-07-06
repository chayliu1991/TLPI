#define _XOPEN_SOURCE 600

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <errno.h>

void errExit(const char* call)
{
    perror(call);
    exit(1);
}

static void usageError(const char* progname,const char* msg)
{
    if(msg != NULL)
        fprintf(stderr,"%s\n",msg);
    
    fprintf(stderr,"Usage:  %s [-d] [-m] [-p] [directory-path]\n",progname);
    fprintf(stderr,"\t-d Use FTW_DEPTH flag \n");
    fprintf(stderr,"\t-d Use FTW_MOUNT flag \n");
    fprintf(stderr,"\t-d Use FTW_PHYS flag \n");
    exit(EXIT_FAILURE);
}


static int dirTree(const char* pathname,const struct stat* sbuf,int type,struct FTW* ftwb)
{
    switch(sbuf->st_mode & S_IFMT)
    {
        case S_IFREG: printf("-");break;
        case S_IFDIR: printf("d");break;
        case S_IFCHR: printf("c");break;
        case S_IFBLK: printf("b");break;
        case S_IFLNK: printf("l");break;
        case S_IFIFO: printf("p");break;
        case S_IFSOCK: printf("s");break;
        default: printf("?");break;
    }

    printf("%s  ",
        (type == FTW_D) ? "D    " : (type == FTW_DNR) ? "DNR    " :  (type == FTW_DP) ? "DP " :
        (type == FTW_F) ? "F    " :  (type == FTW_SL) ? "SL    " :  (type == FTW_SLN) ? "SLN    " :
         (type == FTW_NS) ? "NS    " : "    ");
    
    if(type != FTW_NS)
        printf("%7ld ",(long)(sbuf->st_ino));
    else
        printf("            ");
    
    printf(" %*s",4 * ftwb->level,"");
    printf(" %s",&pathname[ftwb->base]);

    return 0;
}

int main(int argc,char* argv[])
{
    int flags,opt;
    flags = 0;

    while((opt = getopt(argc,argv,"dmp")) != -1)
    {
        switch(opt){
            case 'd' :  flags |= FTW_DEPTH; break;
            case 'm' :  flags |= FTW_MOUNT; break;
            case 'p' :  flags |= FTW_PHYS; break;
            default:  usageError(argv[0],NULL);  
        }
    }
    
    if(argc > optind + 1)
        usageError(argv[0],NULL);  
    
    if(nftw((argc < optind) ? argv[optind] : "." ,dirTree,10,flags) == -1)
    {
        errExit("nftw");
    }


    exit(EXIT_SUCCESS);
}