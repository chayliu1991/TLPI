CC = gcc
Targets = jump set_jump_vars set_jump_vars_O

all : $(Targets)

jump : 
	$(CC) $(CFLAGS) -o jump jump.c

set_jump_vars :
	$(CC) $(CFLAGS) -o set_jump_vars set_jump_vars.c

set_jump_vars_O :
	$(CC) $(CFLAGS) -O -o set_jump_vars_O set_jump_vars.c

clean:
	-rm -rf $(Targets)