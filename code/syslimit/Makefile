CC = gcc
Targets = sysconf pathconf

all : $(Targets)

sysconf : 
	$(CC) $(CFLAGS) -o sysconf sysconf.c

pathconf : 
	$(CC) $(CFLAGS) -o pathconf pathconf.c

clean:
	-rm -rf $(Targets)