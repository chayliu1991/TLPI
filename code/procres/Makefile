CC = gcc
Targets = print_rlimit become_daemon daemon_SIGHUP

all : $(Targets)

print_rlimit : 
	$(CC) $(CFLAGS) -o print_rlimit print_rlimit.c


become_daemon:
	$(CC) $(CFLAGS) -o become_daemon become_daemon.c

daemon_SIGHUP:
	$(CC) $(CFLAGS) -o daemon_SIGHUP daemon_SIGHUP.c

clean:
	-rm -rf $(Targets)