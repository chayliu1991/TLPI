CC = gcc
Targets = unlink list_files nftw

all : $(Targets)

unlink : 
	$(CC) $(CFLAGS) -o unlink unlink.c

list_files : 
	$(CC) $(CFLAGS) -o list_files list_files.c

nftw : 
	$(CC) $(CFLAGS) -o nftw nftw.c



clean:
	-rm -rf $(Targets)