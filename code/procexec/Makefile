CC = gcc
Targets = fork fork_file_sharing vfork fork_whos_on_first fork_signal_sync exit_handlers fork_stdio_buf

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

exit_handlers:
	$(CC) $(CFLAGS) -o exit_handlers exit_handlers.c

fork_stdio_buf:
	$(CC) $(CFLAGS) -o fork_stdio_buf fork_stdio_buf.c

clean:
	-rm -rf $(Targets)