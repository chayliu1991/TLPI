CC = gcc
Targets = psem_create psem_unlink psem_post psem_getvalue  thread_incr_psem pshm_create pshm_write pshm_read pshm_unlink


all : $(Targets)

psem_create : 
	$(CC) $(CFLAGS) -o psem_create psem_create.c -lpthread

psem_unlink:
	$(CC) $(CFLAGS) -o psem_unlink psem_unlink.c -lpthread

psem_post:
	$(CC) $(CFLAGS) -o psem_post psem_post.c -lpthread

psem_getvalue:
	$(CC) $(CFLAGS) -o psem_getvalue psem_getvalue.c -lpthread

thread_incr_psem:
	$(CC) $(CFLAGS) -o thread_incr_psem thread_incr_psem.c -lpthread

pshm_create:
	$(CC) $(CFLAGS) -o pshm_create pshm_create.c -lpthread -lrt

pshm_write:
	$(CC) $(CFLAGS) -o pshm_write pshm_write.c -lpthread -lrt

pshm_read:
	$(CC) $(CFLAGS) -o pshm_read pshm_read.c -lpthread -lrt

pshm_unlink:
	$(CC) $(CFLAGS) -o pshm_unlink pshm_unlink.c -lpthread -lrt

clean:
	-rm -rf $(Targets)