#ifndef CREATE_PID_FILE_H /* Prevent accidental double inclusion */
#define CREATE_PID_FILE_H

#define CPF_CLOEXEC 1

int createPidFile(const char *progName, const char *pidFile, int flags);

#endif