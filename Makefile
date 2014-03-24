CC=gcc
CFLAGS=-c -Wall
SOURCES=$(shell ls *.c)
OBJECTS=$(SOURCES:.c=.o)

all: clean $(OBJECTS) dynamic

dynamic:
	$(CC) -shared -o libcpos.so $(OBJECTS)
clean:
	rm -vfr *~ *.o