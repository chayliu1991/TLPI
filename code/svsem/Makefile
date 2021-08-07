CC = gcc
Targets = svmsg_demo svsem_mon  svsem_setall svsem_op


all : $(Targets)

svmsg_demo : 
	$(CC) $(CFLAGS) -o svmsg_demo svmsg_demo.c

svsem_mon:
	$(CC) $(CFLAGS) -o svsem_mon svsem_mon.c

svsem_setall:
	$(CC) $(CFLAGS) -o svsem_setall svsem_setall.c

svsem_good_init:
	$(CC) $(CFLAGS) -o svsem_good_init svsem_good_init.c

svsem_op:
	$(CC) $(CFLAGS) -o svsem_op svsem_op.c

clean:
	-rm -rf $(Targets)