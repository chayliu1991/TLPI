CC = gcc
Targets = uid_funcitions check_passswd

all : $(Targets)

uid_funcitions : 
	$(CC) $(CFLAGS) -o uid_funcitions uid_funcitions.c

check_passswd :
	$(CC) $(CFLAGS) -o check_passswd check_passswd.c -lcrypt


clean:
	-rm -rf $(Targets)