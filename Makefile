# Makefile: helps in effeciently compiling the project
#
# Run `make` from the root directory to compile the project
# this only compiles modified files when rerun
#

CC=gcc
CFLAGS=-c -Wall -I.
LDFLAGS=-lGL -lglut -lGLU -lpthread -lm -I.
SOURCES=cube.c opengl.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(EXECUTABLE)

clean:
	rm *.o
	rm main
	
$(EXECUTABLE): $(OBJECTS) main.c
	$(CC) $(LDFLAGS) $(OBJECTS) main.c -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

