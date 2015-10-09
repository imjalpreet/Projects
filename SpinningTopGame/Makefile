CC = g++
CFLAGS = -Wall -g
PROG = terrain

SRCS = main.cpp imageloader.cpp vec3f.cpp
LIBS = -lglut -lGL -lGLU -lm

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
