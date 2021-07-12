CC = gcc
Targets = real_timer timed_read nanosleep ptmr_sigev_signal ptmr_sigev_thread

all : $(Targets)

real_timer : 
	$(CC) $(CFLAGS) -o real_timer real_timer.c

timed_read : 
	$(CC) $(CFLAGS) -o timed_read timed_read.c

nanosleep : 
	$(CC) $(CFLAGS) -o nanosleep nanosleep.c

ptmr_sigev_signal :
	$(CC) $(CFLAGS) -o ptmr_sigev_signal ptmr_sigev_signal.c -lrt

ptmr_sigev_thread :
	$(CC) $(CFLAGS) -o ptmr_sigev_thread ptmr_sigev_thread.c -lrt -lpthread


clean:
	-rm -rf $(Targets)