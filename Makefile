# To use this Makefile, you must type:
#
#        make PROG=xxxx
# where
#       xxxx.cpp is the name of the file you wish to compile.
#
#
# Use: 'make PROG=Project1' to compile our project source file into executable.
#
#

CC = g++

GCCFLAGS = -fpcc-struct-return
CFLAGS = -g
INCLUDE = -I/usr/include
LDFLAGS = -L/usr/lib/
LDLIBS =  -lGLU -lGL -lglut -lm


$(PROG):
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) $(PROG).cpp $(LDFLAGS) $(LDLIBS) -o $(PROG)