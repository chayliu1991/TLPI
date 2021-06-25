#define _BSD_SOURCE  //@ getpass
#define _XOPEN_SOURCE //@ crypt

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <errno.h>



typedef enum Boolean_E
{
    FALSE,
    TRUE
} Boolean;

void errExit(const char *caller)
{
    perror(caller);
    exit(1);
}

void fatal(const char *msg)
{
    fprintf(stderr,msg);
    fflush(stderr);
    exit(1);
}

int main(int argc,char* argv[])
{
    char *username, *password, *encrypted, *p;
    struct passwd *pwd;
    struct spwd *spwd;
    Boolean authOk;
    size_t len;
    long lnmax;
    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if(lnmax == -1)
        lnmax = 256;

    username = malloc(lnmax);
    if(username == NULL)
        errExit("malloc");

    printf("Username: ");
    fflush(stdout);
    if(fgets(username,lnmax,stdin) == NULL)
        exit(EXIT_FAILURE);

    len = strlen(username);
    if(username[len-1] == '\n')
        username[len - 1] = '\0';

    pwd = getpwnam(username);
    if(pwd == NULL)
        fatal("colud not get password record\n");
    spwd = getspnam(username);
    if(spwd == NULL && errno == EACCES)
        fatal("no permission to read shadow password file\n");
    
    if(spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;

    password = getpass("PassWord: ");
    encrypted = crypt(password,pwd->pw_passwd);
    for (p = password; *p != '\0';)
        *p++ = '\0';

    if(encrypted == NULL)
        errExit("crypt");

    authOk = strcmp(encrypted,pwd->pw_passwd) == 0;
    if(!authOk)
        fatal("Incorrect password\n");

    printf("Successfully authenticated, UID : %ld \n",(long)pwd->pw_uid);

    exit(EXIT_SUCCESS);
}



