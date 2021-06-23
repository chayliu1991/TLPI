CC = gcc
Targets = copy readv largefile

all : $(Targets)

copy : 
	$(CC) $(CFLAGS) -o copy copy.c

readv : 
	$(CC) $(CFLAGS) -o readv readv.c

largefile : 
	$(CC) $(CFLAGS) -o largefile largefile.c

clean:
	-rm -rf $(Targets)