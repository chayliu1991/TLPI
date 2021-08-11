CC = gcc
Targets = pty_master_open pty_fork


all : $(Targets) 


new_intr: 
	$(CC) $(CFLAGS) -o new_intr new_intr.c

pty_master_open:
	$(CC) $(CFLAGS) -fPIC -shared -o libptymasteropen.so pty_master_open.c

pty_fork:
	$(CC) $(CFLAGS) -fPIC -shared -o libptyfork.so pty_fork.c -L. -lptymasteropen

clean:
	-rm -rf $(Targets)
