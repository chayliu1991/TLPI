CC = gcc
Targets = pty_master_open pty_fork script pty_master_open_bsd


all : $(Targets) 


new_intr: 
	$(CC) $(CFLAGS) -o new_intr new_intr.c

pty_master_open:
	$(CC) $(CFLAGS) -fPIC -shared -o libptymasteropen.so pty_master_open.c
	mv libptymasteropen.so ../libs

pty_fork:
	$(CC) $(CFLAGS) -fPIC -shared -o libptyfork.so pty_fork.c -L. -lptymasteropen
	mv libptyfork.so ../libs

script:
	$(CC) $(CFLAGS) -o script script.c -L. -lptymasteropen -lptyfork -lttyfunctions

pty_master_open_bsd:
	$(CC) $(CFLAGS) -fPIC -shared -o libptymasteropenbsd.so pty_master_open_bsd.c
	mv libptymasteropenbsd.so ../libs


clean:
	-rm -rf $(Targets)
