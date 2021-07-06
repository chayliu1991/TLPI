CC = gcc
Targets = unlink list_files nftw view_symlink

all : $(Targets)

unlink : 
	$(CC) $(CFLAGS) -o unlink unlink.c

list_files : 
	$(CC) $(CFLAGS) -o list_files list_files.c

nftw : 
	$(CC) $(CFLAGS) -o nftw nftw.c

view_symlink : 
	$(CC) $(CFLAGS) -o view_symlink view_symlink.c

clean:
	-rm -rf $(Targets)