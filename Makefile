#include /env.mak

SDIR=src
ODIR=obj
BDIR=bin

OBJS=$(ODIR)/main.o $(ODIR)/jobinfo.o $(ODIR)/tr-getopt.o
EXEC=$(BDIR)/pkgext

CFLAGS= -g -Wall -D_FILE_OFFSET_BITS=64 -Iinclude
LDFLAGS= -lstdc++

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(ODIR)/main.o: $(SDIR)/main.cpp
	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/main.cpp

$(ODIR)/jobinfo.o: $(SDIR)/jobinfo.cpp
	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/jobinfo.cpp

$(ODIR)/tr-getopt.o: $(SDIR)/tr-getopt.c
	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/tr-getopt.c

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ $(EXEC)

install: $(EXEC)
	mkdir -p $(DESTDIR)/usr/bin/
	install $< $(DESTDIR)/usr/bin/

