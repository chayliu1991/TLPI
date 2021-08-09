CC = gcc
Targets = t_mprotetc memlock


all : $(Targets)

t_mprotetc:
	$(CC) $(CFLAGS) -o t_mprotetc t_mprotetc.c

memlock:
	$(CC) $(CFLAGS) -o memlock memlock.c

clean:
	-rm -rf $(Targets)