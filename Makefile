CC=gcc
CFLAGS=-I.
DEPS =
OBJ = distance.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

distance: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
