CC = gcc
Targets = t_flock  i_fcntl_locking region_locking create_pid_file


all : $(Targets) 
t_flock : 
	$(CC) $(CFLAGS) -o t_flock t_flock.c

i_fcntl_locking:
	$(CC) $(CFLAGS) -o i_fcntl_locking i_fcntl_locking.c

region_locking:
	$(CC) $(CFLAGS) -fPIC -shared -o libregionlocking.so region_locking.c

create_pid_file:
	$(CC) $(CFLAGS) -fPIC -shared -o libcreatepidfile create_pid_file.c


clean:
	-rm -rf $(Targets)
