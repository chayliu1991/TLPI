CC = gcc
Targets = t_select poll_pipe


all : $(Targets) 


t_select: 
	$(CC) $(CFLAGS) -o t_select t_select.c

poll_pipe:
	$(CC) $(CFLAGS) -o poll_pipe poll_pipe.c

clean:
	-rm -rf $(Targets)
