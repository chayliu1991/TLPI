#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static jmp_buf env;

static void dojump(int nvar,int rvar,int vvar)
{
    printf("Inside dojump():nvar=%d,rvar=%d,vvar=%d\n",nvar,rvar,vvar);
    longjmp(env,1);
}

int main(int argc,char* argv[])
{
    int nvar;
    register int rvar;
    volatile int vvar;
    if(setjmp(env) == 0)
    {
        nvar = 777;
        rvar = 888;
        vvar = 999;
        dojump(nvar,rvar,vvar);
    }
    else{
        printf("After longjmp():nvar=%d,rvar=%d,vvar=%d\n",nvar,rvar,vvar);
    }

    exit(EXIT_SUCCESS);
}