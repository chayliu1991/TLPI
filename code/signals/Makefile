CC = gcc
Targets = sigqueue catch_signals sigsuspend sigwaitinfo signalfd

all : $(Targets)

sigqueue : 
	$(CC) $(CFLAGS) -o sigqueue sigqueue.c

catch_signals : 
	$(CC) $(CFLAGS) -o catch_signals catch_signals.c

sigsuspend:
	$(CC) $(CFLAGS) -o sigsuspend sigsuspend.c

sigwaitinfo:
	$(CC) $(CFLAGS) -o sigwaitinfo sigwaitinfo.c

signalfd:
	$(CC) $(CFLAGS) -o signalfd signalfd.c

clean:
	-rm -rf $(Targets)