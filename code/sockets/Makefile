CC = gcc
Targets = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl read_line i6d_ucase_sv i6d_ucase_cl is_seqnum_sv is_seqnum_cl inet_sockets t_gethostbyname id_echo_sv id_echo_cl \
	is_echo_sv rdwrn is_echo_cl socknames


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

inet_sockets:
	$(CC) $(CFLAGS) -shared -fPIC -o libinetsockets.so inet_sockets.c
	mv libinetsockets.so ../libs

t_gethostbyname:
	$(CC) $(CFLAGS) -o t_gethostbyname t_gethostbyname.c 

id_echo_sv:
	$(CC) $(CFLAGS) -o id_echo_sv id_echo_sv.c  -L../libs -lbecomedaemon -linetsockets

id_echo_cl:
	$(CC) $(CFLAGS) -o id_echo_cl id_echo_cl.c  -L../libs -lbecomedaemon -linetsockets	

is_echo_sv:
	$(CC) $(CFLAGS) -o is_echo_sv is_echo_sv.c  -L../libs -lbecomedaemon -linetsockets	

rdwrn:
	$(CC) $(CFLAGS) -shared -fPIC -o librdwrn.so rdwrn.c
	mv librdwrn.so ../libs

is_echo_cl:
	$(CC) $(CFLAGS) -o is_echo_cl is_echo_cl.c -L../libs -linetsockets	

socknames:
	$(CC) $(CFLAGS) -o socknames socknames.c -L../libs -linetsockets	


clean:
	-rm -rf $(Targets)