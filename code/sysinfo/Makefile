CC = gcc
Targets = procfs_pidmax uname

all : $(Targets)

procfs_pidmax : 
	$(CC) $(CFLAGS) -o procfs_pidmax procfs_pidmax.c

uname : 
	$(CC) $(CFLAGS) -o uname uname.c

clean:
	-rm -rf $(Targets)