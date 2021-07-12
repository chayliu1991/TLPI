CC = gcc
Targets = real_timer timed_read

all : $(Targets)

real_timer : 
	$(CC) $(CFLAGS) -o real_timer real_timer.c

timed_read : 
	$(CC) $(CFLAGS) -o timed_read timed_read.c

clean:
	-rm -rf $(Targets)