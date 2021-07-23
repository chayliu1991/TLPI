CC = gcc
Targets = fork fork_file_sharing vfork fork_whos_on_first fork_signal_sync exit_handlers fork_stdio_buf multi_wait print_wait_status make_zombie multi_SIGCHILD execve envargs execlp \
	execle execl closeonexec system acct_on clone

all : $(Targets)

fork : 
	$(CC) $(CFLAGS) -o fork fork.c

fork_file_sharing:
	$(CC) $(CFLAGS) -o fork_file_sharing fork_file_sharing.c

vfork :
	$(CC) $(CFLAGS) -o vfork vfork.c

fork_whos_on_first :
	$(CC) $(CFLAGS) -o fork_whos_on_first fork_whos_on_first.c

fork_signal_sync:
	$(CC) $(CFLAGS) -o fork_signal_sync fork_signal_sync.c

exit_handlers :
	$(CC) $(CFLAGS) -o exit_handlers exit_handlers.c

fork_stdio_buf :
	$(CC) $(CFLAGS) -o fork_stdio_buf fork_stdio_buf.c

multi_wait :
	$(CC) $(CFLAGS) -o multi_wait multi_wait.c

print_wait_status :
	$(CC) $(CFLAGS) -o print_wait_status print_wait_status.c

make_zombie :
	$(CC) $(CFLAGS) -o make_zombie make_zombie.c

multi_SIGCHILD :
	$(CC) $(CFLAGS) -o multi_SIGCHILD multi_SIGCHILD.c

execve :
	$(CC) $(CFLAGS) -o execve execve.c

envargs :
	$(CC) $(CFLAGS) -o envargs envargs.c

execlp :
	$(CC) $(CFLAGS) -o execlp execlp.c

execle:
	$(CC) $(CFLAGS) -o execle execle.c

execl:
	$(CC) $(CFLAGS) -o execl execl.c

system :
	$(CC) $(CFLAGS) -o system system.c

closeonexec :
	$(CC) $(CFLAGS) -o closeonexec closeonexec.c


acct_on:
	$(CC) $(CFLAGS) -o acct_on acct_on.c

clone:
	$(CC) $(CFLAGS) -o clone clone.c

clean:
	-rm -rf $(Targets)