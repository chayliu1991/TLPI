CC = gcc
Targets = dump_utmpx  utmpx_login view_lastlog

all : $(Targets)

dump_utmpx : 
	$(CC) $(CFLAGS) -o dump_utmpx dump_utmpx.c

utmpx_login :
	$(CC) $(CFLAGS) -o utmpx_login utmpx_login.c

view_lastlog:
	$(CC) $(CFLAGS) -o view_lastlog view_lastlog.c

clean:
	-rm -rf $(Targets)