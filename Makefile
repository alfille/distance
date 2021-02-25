CC=gcc
CFLAGS=-I.
DEPS =

distance: distance.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_x: distance_x.c
	$(CC) -o $@ $^ $(CFLAGS) -lm

distance_hr: distance_hr.c
	$(CC) -o $@ $^ $(CFLAGS) -lgmp -lmpfr

all: distance distance_x distance_hr
