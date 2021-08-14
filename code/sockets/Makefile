CC = gcc
Targets = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl read_line i6d_ucase_sv i6d_ucase_cl is_seqnum_sv is_seqnum_cl

all : $(Targets)

us_xfr_sv : 
	$(CC) $(CFLAGS) -o us_xfr_sv us_xfr_sv.c

us_xfr_cl:
	$(CC) $(CFLAGS) -o us_xfr_cl us_xfr_cl.c

ud_ucase_sv:
	$(CC) $(CFLAGS) -o ud_ucase_sv ud_ucase_sv.c

ud_ucase_cl:
	$(CC) $(CFLAGS) -o ud_ucase_cl ud_ucase_cl.c

read_line:
	$(CC) $(CFLAGS) -shared -fPIC -o libreadline.so read_line.c
	mv libreadline.so ../libs

i6d_ucase_sv:
	$(CC) $(CFLAGS) -o i6d_ucase_sv i6d_ucase_sv.c

i6d_ucase_cl:
	$(CC) $(CFLAGS) -o i6d_ucase_cl i6d_ucase_cl.c

is_seqnum_cl:
	$(CC) $(CFLAGS) -o is_seqnum_cl is_seqnum_cl.c -L../libs -lreadline -g

is_seqnum_sv:
	$(CC) $(CFLAGS) -o is_seqnum_sv is_seqnum_sv.c -L../libs -lreadline

clean:
	-rm -rf $(Targets)