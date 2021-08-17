CC = gcc
Targets = t_select poll_pipe demo_sigio


all : $(Targets) 


t_select: 
	$(CC) $(CFLAGS) -o t_select t_select.c

poll_pipe:
	$(CC) $(CFLAGS) -o poll_pipe poll_pipe.c

demo_sigio:
	$(CC) $(CFLAGS) -o demo_sigio demo_sigio.c -L../libs -lttyfunctions

clean:
	-rm -rf $(Targets)
