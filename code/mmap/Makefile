CC = gcc
Targets = mmcat t_mmap


all : $(Targets)

mmcat:
	$(CC) $(CFLAGS) -o mmcat mmcat.c

t_mmap:
	$(CC) $(CFLAGS) -o t_mmap t_mmap.c

clean:
	-rm -rf $(Targets)