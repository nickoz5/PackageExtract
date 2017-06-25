#include /env.mak

BDIR=bin

CXX=g++
LINK=$(CXX)

VPATH = src:unrar

SRCS_CORE=$(wildcard src/*.cpp) $(wildcard src/*.c)
_SRCS_UNRAR=strlist.cpp strfn.cpp pathfn.cpp smallfn.cpp global.cpp file.cpp filefn.cpp filcreat.cpp \
	archive.cpp arcread.cpp unicode.cpp system.cpp isnt.cpp crypt.cpp crc.cpp rawread.cpp encname.cpp \
	resource.cpp match.cpp timefn.cpp rdwrfn.cpp consio.cpp options.cpp errhnd.cpp rarvm.cpp secpassword.cpp \
	rijndael.cpp getbits.cpp sha1.cpp sha256.cpp blake2s.cpp hash.cpp extinfo.cpp extract.cpp volume.cpp \
	list.cpp find.cpp unpack.cpp headers.cpp threadpool.cpp rs16.cpp cmddata.cpp ui.cpp \
	qopen.cpp filestr.cpp scantree.cpp
SRCS_UNRAR=$(addprefix unrar/,$(_SRCS_UNRAR))

OBJS=$(SRCS_CORE:.cpp=.o) $(SRCS_UNRAR:.cpp=.o)
INCS=

INCLUDE=-Iinclude/ -Iunrar/
EXEC=$(BDIR)/pkgext

CFLAGS= -Wextra -std=c++11 -pthread -O3 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DRAR_SMP -DRARDLL $(INCLUDE)
LDFLAGS= -lstdc++ -fPIC -pthread

.PHONY: all
all: $(BDIR) $(EXEC)

$(BDIR):
	mkdir -p $@

$(EXEC): $(OBJS)
	$(LINK) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS)
	
.cpp.o: directories
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) *~ core $(IDIR)/*~ $(EXEC)

install: $(EXEC)
	mkdir -p $(DESTDIR)/usr/bin/
	install $< $(DESTDIR)/usr/bin/
	cp scripts/fix_downloadstation.sh $(DESTDIR)/usr/bin/
	cp scripts/pkgext_transmission.sh $(DESTDIR)/usr/bin/

uninstall:
	rm -f $(DESTDIR)/usr/bin/$(EXEC)
	rm -f $(DESTDIR)/usr/bin/fix_downloadstation.sh
	rm -f $(DESTDIR)/usr/bin/pkgext_transmission.sh

