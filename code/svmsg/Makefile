CC = gcc
Targets = svmsg_create svmsg_send svmsg_receive svmsg_rm svmsg_chqbytes svmsg_ls 

all : $(Targets)

svmsg_create : 
	$(CC) $(CFLAGS) -o svmsg_create svmsg_create.c

svmsg_send:
	$(CC) $(CFLAGS) -o svmsg_send svmsg_send.c

svmsg_receive:
	$(CC) $(CFLAGS) -o svmsg_receive svmsg_receive.c

svmsg_rm:
	$(CC) $(CFLAGS) -o svmsg_rm svmsg_rm.c

svmsg_chqbytes:
	$(CC) $(CFLAGS) -o svmsg_chqbytes svmsg_chqbytes.c

svmsg_ls:
	$(CC) $(CFLAGS) -o svmsg_ls svmsg_ls.c

clean:
	-rm -rf $(Targets)