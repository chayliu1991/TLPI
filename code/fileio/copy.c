#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc,char* argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
         printf("%s old-file new-file\n", argv[0]);
         exit(0);
    }

    int input, output,numread,numwrite;

    input = open(argv[1],O_RDONLY);
    if(input < 0)
    {
        perror("open() input file");
        exit(-1);
    }

    output = open(argv[2],O_WRONLY | O_TRUNC | O_CREAT,0644);
    if(output < 0)
    {
        perror("open() output file");
        exit(-1);
    }

    char buf[BUFSIZE];
    while((numread = read(input,buf,BUFSIZE)) != 0)
    {
        if(numread < 0)
        {
            perror("read()");
            exit(-1);
        }

        numwrite = write(output,buf,numread);
        if(numread != numwrite)
        {
            perror("write()");
            exit(-1);
        }
    }

    close(input);
    close(output);

    return 0;
}
