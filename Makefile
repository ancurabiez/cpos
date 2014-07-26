CC=gcc
CFLAGS=-Wall -fPIC
LIBS=-lm
SOURCES=build.c \
        parser.c \
        socket.c \
        utils.c
OBJECTS=$(SOURCES:.c=.o)

all: clean $(OBJECTS) dynamic unpack_test build_test

dynamic:
	$(CC) -shared -o libcpos.so $(OBJECTS) $(LIBS)
clean:
	rm -vfr *~ *.o *.so
install:
	cp *.so /usr/lib
	cp *.h /usr/include
unpack_test:
	$(CC) unpack_benchmark.c -o unpack_benchmark ./libcpos.so
build_test:
	$(CC) build_benchmark.c -o build_benchmark ./libcpos.so
	