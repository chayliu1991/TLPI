CC = gcc
Targets = print_rlimit become_daemon

all : $(Targets)

print_rlimit : 
	$(CC) $(CFLAGS) -o print_rlimit print_rlimit.c


become_daemon:
	$(CC) $(CFLAGS) -o become_daemon become_daemon.c

clean:
	-rm -rf $(Targets)