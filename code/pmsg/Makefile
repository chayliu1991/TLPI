CC = gcc
Targets = pmsg_unlink pmsg_create pmsg_getattr


all : $(Targets)

pmsg_unlink:
	$(CC) $(CFLAGS) -o pmsg_unlink pmsg_unlink.c -lrt

pmsg_create:
	$(CC) $(CFLAGS) -o pmsg_create pmsg_create.c -lrt

pmsg_getattr:
	$(CC) $(CFLAGS) -o pmsg_getattr pmsg_getattr.c -lrt

clean:
	-rm -rf $(Targets)