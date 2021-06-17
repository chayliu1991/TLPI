include ../Makefile.inc

allgen : ${TLPI_LIB}

${TLPI_LIB} : *.c ename.c.inc
	${CC} -c -g ${CFLAGS} *.c
	${RM} ${TLPI_LIB}
	${AR} rs ${TLPI_LIB} *.o

ename.c.inc :
	sh Build_ename.sh > ename.c.inc
	echo 1>&2 "ename.c.inc built"

clean :
	${RM} *.o ename.c.inc ${TLPI_LIB}

