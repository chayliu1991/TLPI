CC = gcc
Targets = mmcat t_mmap  anon_mmap


all : $(Targets)

mmcat:
	$(CC) $(CFLAGS) -o mmcat mmcat.c

t_mmap:
	$(CC) $(CFLAGS) -o t_mmap t_mmap.c

anon_mmap:
	$(CC) $(CFLAGS) -o anon_mmap anon_mmap.c

clean:
	-rm -rf $(Targets)