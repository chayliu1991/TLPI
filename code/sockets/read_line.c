#include "read_line.h"

#include <errno.h>
#include <unistd.h>


ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;
    size_t toRead;
    char *buf;
    char ch;


    if(n <= 0 || buffer == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    toRead = 0;
    for (;;)
    {
        numRead = read(fd,&ch,1);
        if(numRead == -1)
        {
            if(errno == EINTR)
                continue;
            else
                return -1;
        }
        else if(numRead == 0)
        {
            if(toRead == 0)
                return 0;
            else
                break;
        }  
        else
        {
            if(toRead < n-1)
            {
                toRead++;
                *buf++ = ch;
            }
            if(ch == '\n')
                break;
        }       
    }

    *buf = '\0';
    return toRead;
}