TLPI_DIR = ..
TLPI_LIB = ${TLPI_DIR}/libtlpi.a
TLPI_INC_DIR = ${TLPI_DIR}/lib


LINUX_LIBRT = -lrt
LINUX_LIBDL = -dl
LINUX_LIACL = -lacl
LINUX_LIBCRYPT = -lcrypt
LINUX_LIBCAP = -lcap


IMPL_CFLAGS = -std=c99 \
            -g \
            -I$(TLPI_INC_DIR) \
            -Wall \
            -W

            
IMPL_THREAD_FLAGS = -pthread

IMPL_LDLIBS = ${TLPI_LIB} -lm

LDLIBS = ${IMPL_LDLIBS}

RM = rm -f


CFLAGS = ${IMPL_CFLAGS}