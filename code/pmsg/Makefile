CC = gcc
Targets = pmsg_unlink pmsg_create pmsg_getattr pmsg_send pmsg_receive mq_notify_sig mq_notify_thread


all : $(Targets)

pmsg_unlink:
	$(CC) $(CFLAGS) -o pmsg_unlink pmsg_unlink.c -lrt

pmsg_create:
	$(CC) $(CFLAGS) -o pmsg_create pmsg_create.c -lrt

pmsg_getattr:
	$(CC) $(CFLAGS) -o pmsg_getattr pmsg_getattr.c -lrt

pmsg_send:
	$(CC) $(CFLAGS) -o pmsg_send pmsg_send.c -lrt

pmsg_receive:
	$(CC) $(CFLAGS) -o pmsg_receive pmsg_receive.c -lrt

mq_notify_sig:
	$(CC) $(CFLAGS) -o mq_notify_sig mq_notify_sig.c -lrt

mq_notify_thread:
	$(CC) $(CFLAGS) -o mq_notify_thread mq_notify_thread.c -lrt


clean:
	-rm -rf $(Targets)