CC = gcc
Targets = setpriority sched_set  sched_view

all : $(Targets)

setpriority : 
	$(CC) $(CFLAGS) -o setpriority setpriority.c

sched_set:
	$(CC) $(CFLAGS) -o sched_set sched_set.c

sched_view:
	$(CC) $(CFLAGS) -o  sched_view  sched_view.c

clean:
	-rm -rf $(Targets)