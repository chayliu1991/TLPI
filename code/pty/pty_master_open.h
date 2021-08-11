#ifndef PTY_MASTER_OPEN_H
#define PTY_MASTER_OPEN_H

#include <sys/types.h>

int ptyMasterOpen(char *slaveName, size_t snLen);

#endif