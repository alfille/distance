CC=gcc
CFLAGS=-I. -lm
DEPS =
OBJ = distance.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

distance: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
