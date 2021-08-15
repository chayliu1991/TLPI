#ifndef RDWRN_H
#define RDWRN_H

#include <sys/types.h>

ssize_t readn(int fd,void* buffer,size_t count);

ssize_t writen(int fd,void* buffer,size_t count);

#endif //@ RDWRN_H