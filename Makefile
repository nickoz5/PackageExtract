#include /env.mak

SDIR=src
ODIR=obj
BDIR=bin

CXX=g++
LINK=$(CXX)

VPATH = src:unrar

SRCS_CORE=main.cpp artefact.cpp environment.cpp filefinder.cpp fileio.cpp package.cpp tr-getopt.cpp
SRCS_UNRAR=strlist.cpp strfn.cpp pathfn.cpp smallfn.cpp global.cpp file.cpp filefn.cpp filcreat.cpp \
	archive.cpp arcread.cpp unicode.cpp system.cpp isnt.cpp crypt.cpp crc.cpp rawread.cpp encname.cpp \
	resource.cpp match.cpp timefn.cpp rdwrfn.cpp consio.cpp options.cpp errhnd.cpp rarvm.cpp secpassword.cpp \
	rijndael.cpp getbits.cpp sha1.cpp sha256.cpp blake2s.cpp hash.cpp extinfo.cpp extract.cpp volume.cpp \
	list.cpp find.cpp unpack.cpp headers.cpp threadpool.cpp rs16.cpp cmddata.cpp ui.cpp \
	qopen.cpp filestr.cpp scantree.cpp

OBJS=$(SRCS_CORE:.cpp=.o) $(SRCS_UNRAR:.cpp=.o)
INCS=
SCRIPTS=fix_downloadstations.sh pkgext_transmission.sh

INCLUDE=-Iinclude/ -Iunrar/
EXEC=$(BDIR)/pkgext

CFLAGS= -Wextra -std=c++11 -pthread -O3 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DRAR_SMP -DRARDLL $(INCLUDE)
LDFLAGS= -lstdc++ -fPIC -pthread

.PHONY: all
all: $(ODIR) $(BDIR) $(EXEC)

$(BDIR) $(ODIR):
	mkdir -p $@

$(EXEC): $(OBJS)
	$(LINK) -o $@ $(CFLAGS) $(LDFLAGS) $(addprefix $(ODIR)/,$(OBJS))
	
.cpp.o: directories
	$(CXX) $(CFLAGS) -c $< -o $(ODIR)/$@

#$(ODIR)/main.o: $(SDIR)/main.cpp
#	$(CC) $(CFLAGS) -o $@ -c $(SDIR)/main.cpp
#

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ $(EXEC)

install: $(EXEC) $(SCRIPTS)
	mkdir -p $(DESTDIR)/usr/bin/
	install $< $(DESTDIR)/usr/bin/

