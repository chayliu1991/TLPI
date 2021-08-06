CC = gcc
Targets = simple_pipe pipe_sync pipe_ls_wc popen_glob fifo_sequem_server fifo_sequem_client

all : $(Targets)

simple_pipe : 
	$(CC) $(CFLAGS) -o simple_pipe simple_pipe.c

pipe_sync:
	$(CC) $(CFLAGS) -o pipe_sync pipe_sync.c

pipe_ls_wc:
	$(CC) $(CFLAGS) -o pipe_ls_wc pipe_ls_wc.c

popen_glob:
	$(CC) $(CFLAGS) -o popen_glob popen_glob.c

fifo_sequem_server:
	$(CC) $(CFLAGS) -o fifo_sequem_server fifo_sequem_server.c

fifo_sequem_client:
	$(CC) $(CFLAGS) -o fifo_sequem_client fifo_sequem_client.c

clean:
	-rm -rf $(Targets)