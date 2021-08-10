CC = gcc
Targets = new_intr no_echo


all : $(Targets) 


new_intr: 
	$(CC) $(CFLAGS) -o new_intr new_intr.c

no_echo:
	$(CC) $(CFLAGS) -o no_echo no_echo.c

clean:
	-rm -rf $(Targets)
