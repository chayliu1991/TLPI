CC = gcc
Targets = sigqueue catch_signals

all : $(Targets)

sigqueue : 
	$(CC) $(CFLAGS) -o sigqueue sigqueue.c

catch_signals : 
	$(CC) $(CFLAGS) -o catch_signals catch_signals.c

clean:
	-rm -rf $(Targets)