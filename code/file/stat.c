#define _BSD_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>

typedef enum
{
    True = 0,
    False
} Boolean;

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

static void displayStatInfo(const struct stat* sb)
{
    printf("File type:              ");

    switch (sb->st_mode & S_IFMT)
    {
    case S_IFREG:
        printf("regular file\n");
        break;
    case S_IFDIR:
        printf("directory\n");
        break;
    case S_IFCHR:
        printf("character device\n");
        break;
    case S_IFBLK:
        printf("block device\n");
        break;
    case S_IFLNK:
        printf("symbolic (soft) line\n");
        break;
    case S_IFIFO:
        printf("FIFO or PIPE\n");
        break;
    case S_IFSOCK:
        printf("socket\n");
        break;
    default:
        printf("unknown file type?");
        break;
    }

    printf("Device containing i-node:major=%ld minor=%ld\n",(long)major(sb->st_dev),(long)minor(sb->st_dev));
    printf("I-node number:      %ld\n",(long)sb->st_ino);
    // printf("Mode:      %lo(%s)\n",(unsigned long)sb->st_mode,filePermStr(sb->st_mode,0));

    if(sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
    {
        printf("    special bits set:       %s%s%s\n",
            (sb->st_mode & S_ISUID) ? "set-UID" : "",
            (sb->st_mode & S_ISGID) ? "set-GID" : "",
            (sb->st_mode & S_ISVTX) ? "sticky" : "");
    }

    printf("Number of (hard) links: %ld\n",(long)sb->st_nlink);
    printf("Ownership UID=%ld  GID = %ld: %ld\n",(long)sb->st_uid,(long)sb->st_gid);
    printf("Device number (st_rdev): major=%ld;minor=%ld\n",(long)major(sb->st_rdev),(long)minor(sb->st_rdev));
    printf("File size:          %lld tytes\n",(long long)sb->st_size);
    printf("Optimal IO block size:          %lld tytes\n",(long long)sb->st_blksize);
    printf("Last file access:       %s\n",ctime(&sb->st_atime));
    printf("Last file modification:       %s\n",ctime(&sb->st_mtime));
    printf("Last states change:       %s\n",ctime(&sb->st_ctime));
}

int main(int argc,char* argv[])
{
    struct stat sb;
    Boolean statLink;
    int fname;

    statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
    fname = statLink ? 2 : 1;
    if(fname >= argc || (argc > 1 && strcmp(argv[1],"--help") == 0))
    {
        printf("%s [-l] file\n"
        "       -l = use lstat() instead of stat()\n",argv[0]);
        exit(0);
    }

    if(statLink)
    {
        if(lstat(argv[fname],&sb) < 0)
            errExit("lstat");
    }
    else
    {
        if(stat(argv[fname],&sb) < 0)
            errExit("stat");
    }

    displayStatInfo(&sb);

    exit(EXIT_SUCCESS);
}