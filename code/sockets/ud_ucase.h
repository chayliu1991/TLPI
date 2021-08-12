#ifndef UD_CASE_H
#define UD_CASE_H

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 10

#define SV_SOCKET_PATH "/tmp/ud_case"

#endif // UD_CASE_H