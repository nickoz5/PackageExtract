#include /env.mak

SDIR=src
ODIR=obj
BDIR=bin

OBJS=main.o jobinfo.o tr-getopt.o rar.o strlist.o strfn.o pathfn.o smallfn.o global.o file.o filefn.o filcreat.o \
	archive.o arcread.o unicode.o system.o isnt.o crypt.o crc.o rawread.o encname.o \
	resource.o match.o timefn.o rdwrfn.o consio.o options.o errhnd.o rarvm.o secpassword.o \
	rijndael.o getbits.o sha1.o sha256.o blake2s.o hash.o extinfo.o extract.o volume.o \
  list.o find.o unpack.o headers.o threadpool.o rs16.o cmddata.o ui.o
  
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

