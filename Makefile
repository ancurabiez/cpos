CC=gcc
CFLAGS=-c -Wall
LIBS=-lm
SOURCES=$(shell ls *.c)
OBJECTS=$(SOURCES:.c=.o)

all: clean $(OBJECTS) dynamic

dynamic:
	$(CC) -shared -o libcpos.so $(OBJECTS) $(LIBS)
clean:
	rm -vfr *~ *.o