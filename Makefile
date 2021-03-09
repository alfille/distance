CC=gcc
CFLAGS=-I.
DEPS =

distance: distance.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_any: distance_any.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_f: distance_f.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_x: distance_x.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_hr: distance_hr.c
	$(CC) -o $@ $^ $(CFLAGS) -lgmp -lmpfr

all: distance distance_any distance_f distance_x distance_hr
