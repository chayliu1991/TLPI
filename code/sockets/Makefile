CC = gcc
Targets = us_xfr_sv us_xfr_cl

all : $(Targets)

us_xfr_sv : 
	$(CC) $(CFLAGS) -o us_xfr_sv us_xfr_sv.c

us_xfr_cl:
	$(CC) $(CFLAGS) -o us_xfr_cl us_xfr_cl.c

clean:
	-rm -rf $(Targets)