CC = gcc
Targets = copy readv largefile directread

all : $(Targets)

copy : 
	$(CC) $(CFLAGS) -o copy copy.c

readv : 
	$(CC) $(CFLAGS) -o readv readv.c

largefile : 
	$(CC) $(CFLAGS) -o largefile largefile.c

directread:
	$(CC) $(CFLAGS) -o directread directread.c

clean:
	-rm -rf $(Targets)