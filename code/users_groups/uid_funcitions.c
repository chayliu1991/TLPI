#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>

char* userNameFromId(uid_t uid)
{
    struct passwd *pwd;
    pwd = getpwuid(uid);
    return pwd == NULL ? NULL : pwd->pw_name;
}

uid_t userIdFromname(const char* name)
{
    if(name == NULL || *name == '\0')
        return -1;

    struct passwd *pwd;
    pwd = getpwnam(name);   
    return pwd == NULL ? -1 : pwd->pw_uid;
}

char* groupNameFromId(gid_t gid)
{
    struct group *grp;
    grp = getgrgid(gid);
    return grp == NULL ? NULL : grp->gr_name;
}

gid_t groupIdFromName(const char* name)
{
    if(name == NULL || *name == '\0')
        return -1;

    struct group *grp;
    grp = getgrnam(name);
    return grp == NULL ? -1 : grp->gr_gid;
}


int main(int argc,char* argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
         printf("%s user_name group_name\n", argv[0]);
         exit(0);
    }

    uid_t uid = userIdFromname(argv[1]);
    gid_t gid = groupIdFromName(argv[2]);
    printf("uid:%lld\n",(long long)uid);
    printf("gid:%lld\n",(long long)gid);

    printf("user:%s\n",userNameFromId(uid));
    printf("group:%s\n",groupNameFromId(gid));

    exit(EXIT_SUCCESS);
}