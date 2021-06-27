CC = gcc
Targets = current strtime show_time process_time

all : $(Targets)

current : 
	$(CC) $(CFLAGS) -o current current.c

strtime : 
	$(CC) $(CFLAGS) -o strtime strtime.c

show_time : 
	$(CC) $(CFLAGS) -o show_time show_time.c

process_time : 
	$(CC) $(CFLAGS) -o process_time process_time.c

clean:
	-rm -rf $(Targets)