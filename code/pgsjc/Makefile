CC = gcc
Targets = setsid catch_SIGHUP disc_SIGHUP job_mon handling_SIGTSTP orphaned_pgrp_SIGHUP

all : $(Targets)

setsid : 
	$(CC) $(CFLAGS) -o setsid setsid.c

catch_SIGHUP:
	$(CC) $(CFLAGS) -o catch_SIGHUP catch_SIGHUP.c

disc_SIGHUP:
	$(CC) $(CFLAGS) -o disc_SIGHUP disc_SIGHUP.c

job_mon:
	$(CC) $(CFLAGS) -o job_mon job_mon.c

handling_SIGTSTP:
	$(CC) $(CFLAGS) -o handling_SIGTSTP handling_SIGTSTP.c

orphaned_pgrp_SIGHUP:
	$(CC) $(CFLAGS) -o orphaned_pgrp_SIGHUP orphaned_pgrp_SIGHUP.c


clean:
	-rm -rf $(Targets)