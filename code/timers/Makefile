CC = gcc
Targets = real_timer timed_read nanosleep

all : $(Targets)

real_timer : 
	$(CC) $(CFLAGS) -o real_timer real_timer.c

timed_read : 
	$(CC) $(CFLAGS) -o timed_read timed_read.c

nanosleep : 
	$(CC) $(CFLAGS) -o nanosleep nanosleep.c

clean:
	-rm -rf $(Targets)