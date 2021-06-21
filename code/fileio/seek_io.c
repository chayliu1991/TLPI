#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s file {r<length>|R<length>|w<length>|s<offset>}...\n", argv[0]);
        exit(0);
    }

    int fd, ap;
    ssize_t numread, numwrite;

    fd = open(argv[1],O_RDWR |O_CREAT,0644);
    if(fd < 0)
    {
        perror("open() file");
        exit(-1);
    }

    for (ap = 2; ap < argc;ap++)
    {
        switch (argv[ap][0])
        {
        case 'r':
        case 'R':
        {

        }
        break;    
        default:
            break;
        }
    }

        output = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(output < 0)
    {
        perror("open() output file");
        exit(-1);
    }


    exit(0);
}