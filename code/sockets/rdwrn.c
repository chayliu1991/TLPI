#include "rdwrn.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t readn(int fd,void* buffer,size_t count)
{
    ssize_t numRead;
    size_t totRead;
    char* buf;

    buf = buffer;
    for(totRead = 0;totRead < count;)
    {
        numRead = read(fd,buf,count- totRead);

        if(numRead == 0)
            return totRead;
        if(numRead == -1)
        {
            if(errno == EINTR)
                continue;
            else
                return -1;
        }

        totRead += numRead;
        buf += numRead;
    }

    return totRead;
}

ssize_t writen(int fd,void* buffer,size_t count)
{
    ssize_t numWritten;
    size_t totWritten;
    const char* buf;

    buf = buffer;
    for(totWritten = 0;totWritten < count;)
    {
        numWritten = write(fd,buf,count - totWritten);

        if(numWritten <= 0)
        {
            if(numWritten == -1 && errno == EINTR)
                continue;
            else
                return -1;
        }

        totWritten += numWritten;
        buf += numWritten;
    }

    return totWritten;
}