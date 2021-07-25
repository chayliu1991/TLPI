CC = gcc
Targets = simple_thread thread_incr thread_incr_mutex thread_multi_join strerror strerror_tsd strerror_tls
FLAGS = -lpthread

all : $(Targets)

simple_thread : 
	$(CC) $(CFLAGS) -o simple_thread simple_thread.c $(FLAGS)

thread_incr :
	$(CC) $(CFLAGS) -o thread_incr thread_incr.c $(FLAGS)

thread_incr_mutex:
	$(CC) $(CFLAGS) -o thread_incr_mutex thread_incr_mutex.c $(FLAGS)

thread_multi_join:
	$(CC) $(CFLAGS) -o thread_multi_join thread_multi_join.c $(FLAGS)

strerror:
	$(CC) $(CFLAGS) -o strerror strerror.c $(FLAGS)

strerror_tsd:
	$(CC) $(CFLAGS) -o strerror_tsd strerror_tsd.c $(FLAGS)

strerror_tls:
	$(CC) $(CFLAGS) -o strerror_tls strerror_tls.c $(FLAGS)

	
clean:
	-rm -rf $(Targets)