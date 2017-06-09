include /env.mak


SOURCES=main.cpp jobinfo.cpp tr-getopt.c
OBJS=main.o jobinfo.o tr-getopt.o
EXEC=extracter
CFLAGS= -g -Wall -D_FILE_OFFSET_BITS=64

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

jobinfo.o: jobinfo.cpp
	$(CC) $(CFLAGS) -c jobinfo.cpp

tr-getopt.o: tr-getopt.c
	$(CC) $(CFLAGS) -c tr-getopt.c



clean:
	rm -rf *.o $(PROG)

install: $(EXEC)
	mkdir -p $(DESTDIR)/usr/bin/
	install $< $(DESTDIR)/usr/bin/


