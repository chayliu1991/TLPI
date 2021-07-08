CC = gcc
Targets = sigqueue catch_signals sigsuspend

all : $(Targets)

sigqueue : 
	$(CC) $(CFLAGS) -o sigqueue sigqueue.c

catch_signals : 
	$(CC) $(CFLAGS) -o catch_signals catch_signals.c

sigsuspend:
	$(CC) $(CFLAGS) -o sigsuspend sigsuspend.c

clean:
	-rm -rf $(Targets)