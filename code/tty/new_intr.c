#include <termio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
    struct termios tp;
    int intrChar;

    if (argc > 1 && strcmp(argv[1], "--help") == 0){
        printf("%s [intr-char]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Determine new INTR setting from command line */

    if (argc == 1) {                                    /* Disable */
        intrChar = fpathconf(STDIN_FILENO, _PC_VDISABLE);
        if (intrChar == -1){
            printf("Couldn't determine VDISABLE");
            exit(EXIT_FAILURE);
        }
    } else if (isdigit((unsigned char) argv[1][0])) {
        intrChar = strtoul(argv[1], NULL, 0);           /* Allows hex, octal */
    } else {                                            /* Literal character */
        intrChar = argv[1][0];
    }

    /*获取当前终端设置，修改 INTR 字符，并将更改推送回终端驱动程序  */
    if (tcgetattr(STDIN_FILENO, &tp) == -1){
        printf("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tp.c_cc[VINTR] = intrChar;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1){
        printf("tcsetattr");
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}