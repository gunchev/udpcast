DESTDIR =

#CFLAGS = -O4 -Wall -DNDEBUG -DLOSSTEST
CFLAGS =-Wall -Wshadow -DBB_FEATURE_UDPCAST_FEC -D_FILE_OFFSET_BITS=64 -DUSE_SYSLOG -DUSE_ASSEMBLER -O6
LDFLAGS =-s -Wl,-warn-common
LIBS=-lpthread
BUSYBOX=../udp-busybox/busybox
BBTARGET=$(BUSYBOX)/udpcast
EXESUFFIX=

CFLAGS += -DNO_BB

all: udp-receiver$(EXESUFFIX) udp-sender$(EXESUFFIX) \
	udp-receiver.1 udp-sender.1
#all: fec-test

disk: udpcast.img.gz

#disks: sender.img.gz receiver.img.gz

clean:
	rm -f *.o udp-sender$(EXESUFFIX) udp-receiver$(EXESUFFIX) \
	udp-sender.1 udp-receiver.1 *~

install: udp-sender udp-receiver
	install -d $(DESTDIR)/usr/sbin
	install -m 755 udp-sender udp-receiver $(DESTDIR)/usr/sbin
	install -d $(DESTDIR)/usr/share/man/man1
	install -m 644 udp-sender.1 udp-receiver.1 $(DESTDIR)/usr/share/man/man1

udp-sender$(EXESUFFIX): udp-sender.o socklib.o udpcast.o rate-limit.o \
	sender-diskio.o senddata.o udps-negotiate.o \
	fifo.o produconsum.o participants.o log.o statistics.o \
	fec.o udpc_version.o console.o process.o
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@


udp-receiver$(EXESUFFIX): udp-receiver.o socklib.o udpcast.o \
	receiver-diskio.o receivedata.o udpr-negotiate.o produconsum.o \
	fifo.o log.o statistics.o fec.o udpc_version.o console.o process.o
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

udp-receiver.1 udp-sender.1: html2man.pl cmd.html 
	perl $< $^

fec-test: fec-test.o
	$(CC) $(LDFLAGS) $^ -o $@

fec.o: fec.c
	$(CC) -c $(CFLAGS) -fno-inline $<


bbcopy: $(BBTARGET) \
	$(BBTARGET)/fec.c $(BBTARGET)/participants.h $(BBTARGET)/socklib.c \
	$(BBTARGET)/udp-receiver.c $(BBTARGET)/fec.h \
	$(BBTARGET)/produconsum.c $(BBTARGET)/socklib.h \
	$(BBTARGET)/udp-receiver.h $(BBTARGET)/fec-test.c \
	$(BBTARGET)/produconsum.h $(BBTARGET)/statistics.c \
	$(BBTARGET)/udpr-negotiate.c $(BBTARGET)/fifo.c \
	$(BBTARGET)/rate-limit.c $(BBTARGET)/statistics.h \
	$(BBTARGET)/udp-sender.c $(BBTARGET)/fifo.h $(BBTARGET)/rate-limit.h \
	$(BBTARGET)/udpcast.c $(BBTARGET)/udp-sender.h \
	$(BBTARGET)/libbb_udpcast.h $(BBTARGET)/receivedata.c \
	$(BBTARGET)/udpcast.h $(BBTARGET)/udps-negotiate.c \
	$(BBTARGET)/log.c $(BBTARGET)/receiver-diskio.c \
	$(BBTARGET)/udpc-protoc.h $(BBTARGET)/util.h \
	$(BBTARGET)/log.h $(BBTARGET)/senddata.c $(BBTARGET)/udpc_version.c \
	$(BBTARGET)/participants.c $(BBTARGET)/sender-diskio.c \
	$(BBTARGET)/udpc_version.h \
	$(BBTARGET)/console.h $(BBTARGET)/console.c \
	$(BBTARGET)/process.h $(BBTARGET)/process.c \
	$(BBTARGET)/threads.h \
	$(BBTARGET)/Config.in $(BBTARGET)/Makefile.in $(BBTARGET)/Makefile

$(BBTARGET):
	mkdir $(BBTARGET)

$(BBTARGET)/Makefile: Makefile.busybox
	cp -f Makefile.busybox $(BBTARGET)/Makefile

$(BBTARGET)/Makefile.in: Makefile.in.busybox
	cp -f Makefile.in.busybox $(BBTARGET)/Makefile.in

$(BBTARGET)/%.c: %.c
	cp -f $< $(BBTARGET)

$(BBTARGET)/%.h: %.h
	cp -f $< $(BBTARGET)

$(BBTARGET)/Config.in: Config.in
	cp -f $< $(BBTARGET)

bb: bbcopy
	make -C $(BUSYBOX)
