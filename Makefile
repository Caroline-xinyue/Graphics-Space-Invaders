# basic Makefile 
# compile with "make <filename without .c>
# Note that if you installed your OpenGL libraries in a different place
# the paths that follow -L in LDFLAGS and -I in CFLAGS will have to change
# accordingly

ifeq ($(shell uname -s), Darwin)
LDFLAGS = -L/opt/ports/lib -framework OpenGL -lglfw -lGLEW -lm
else
LDFLAGS = -lX11 -lGL -lGLU -lglfw -lGLEW -lm
endif
CC = gcc
CFLAGS = -g -Wall -I/opt/ports/include
DEPS = hw2.h
OBJ = hw2.o
OBJ_DEBUG = hw2_debug.o

hw2.o: hw2.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<
hw2: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
hw2_debug.o: hw2.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -c -DDEBUG -o $@ $<
hw2_debug: $(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
clean:
	rm *.o hw2 hw2_debug
