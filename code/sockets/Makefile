CC = gcc
Targets = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl read_line

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

clean:
	-rm -rf $(Targets)