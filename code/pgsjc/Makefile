CC = gcc
Targets = setsid catch_SIGHUP disc_SIGHUP

all : $(Targets)

setsid : 
	$(CC) $(CFLAGS) -o setsid setsid.c

catch_SIGHUP:
	$(CC) $(CFLAGS) -o catch_SIGHUP catch_SIGHUP.c

disc_SIGHUP:
	$(CC) $(CFLAGS) -o disc_SIGHUP disc_SIGHUP.c

clean:
	-rm -rf $(Targets)