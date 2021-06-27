CC = gcc
Targets = current strtime

all : $(Targets)

current : 
	$(CC) $(CFLAGS) -o current current.c

strtime : 
	$(CC) $(CFLAGS) -o strtime strtime.c

clean:
	-rm -rf $(Targets)