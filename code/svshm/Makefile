CC = gcc
Targets = binarysems svshm_xfr_writer svshm_xfr_reader


all : $(Targets)

binarysems:
	$(CC) $(CFLAGS) -fPIC -shared -o libbinarysems.so binary_sems.c

svshm_xfr_writer:
	$(CC) $(CFLAGS) -o svshm_xfr_writer svshm_xfr_writer.c -L. -lbinarysems

svshm_xfr_reader:
	$(CC) $(CFLAGS) -o svshm_xfr_reader svshm_xfr_reader.c -L. -lbinarysems

clean:
	-rm -rf $(Targets)