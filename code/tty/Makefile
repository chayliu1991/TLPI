CC = gcc
Targets = new_intr no_echo tty_functions test_tty_functions


all : $(Targets) 


new_intr: 
	$(CC) $(CFLAGS) -o new_intr new_intr.c

no_echo:
	$(CC) $(CFLAGS) -o no_echo no_echo.c

tty_functions:
	$(CC) $(CFLAGS) -fPIC -shared -o libttyfunctions.so tty_functions.c

test_tty_functions:
	$(CC) $(CFLAGS) -o test_tty_functions test_tty_functions.c -L. -lttyfunctions



clean:
	-rm -rf $(Targets)
