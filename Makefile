#include /env.mak

SDIR=src
ODIR=obj
BDIR=bin

srcdir = src

SRCS=main.cpp jobinfo.cpp
SRCS_C=tr-getopt.c
UNRAR_SRCS=rar.cpp strlist.cpp strfn.cpp pathfn.cpp smallfn.cpp global.cpp file.cpp filefn.cpp filcreat.cpp \
	archive.cpp arcread.cpp unicode.cpp system.cpp isnt.cpp crypt.cpp crc.cpp rawread.cpp encname.cpp \
	resource.cpp match.cpp timefn.cpp rdwrfn.cpp consio.cpp options.cpp errhnd.cpp rarvm.cpp secpassword.cpp \
	rijndael.cpp getbits.cpp sha1.cpp sha256.cpp blake2s.cpp hash.cpp extinfo.cpp extract.cpp volume.cpp \
  list.cpp find.cpp unpack.cpp headers.cpp threadpool.cpp rs16.cpp cmddata.cpp ui.cpp
 
OBJS = $(SRCS:.cpp=.o) $(SRCS_C:.c=.o) $(UNRAR_SRCS:.cpp=.o)
INCS = 

EXEC=$(BDIR)/pkgext

CFLAGS= -g -Wall -D_FILE_OFFSET_BITS=64 -Iinclude
LDFLAGS= -lstdc++

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	
$(OBJS): $(SRCS) $(UNRAR_SRCS)
#$(ODIR)/main.o: $(SDIR)/main.cpp
#	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/main.cpp
#
#$(ODIR)/jobinfo.o: $(SDIR)/jobinfo.cpp
#	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/jobinfo.cpp
#
#$(ODIR)/tr-getopt.o: $(SDIR)/tr-getopt.c
#	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/tr-getopt.c

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ $(EXEC)

install: $(EXEC)
	mkdir -p $(DESTDIR)/usr/bin/
	install $< $(DESTDIR)/usr/bin/

