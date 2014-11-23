CROSS = arm-linux-
CFLAGS=-g -Wall --static
CC=$(CROSS)gcc
AR=$(CROSS)ar
RANLIB=ranlib
LIBS=-L./ -lcgic

all: libcgic.a cgictest.cgi capture

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

cgictest.cgi: cgictest.o libcgic.a
	$(CC) cgictest.o -o cgictest.cgi ${LIBS} --static

capture: capture.o libcgic.a
	$(CC) capture.o -o capture ${LIBS} --static

clean:
	rm -f *.o *.a cgictest.cgi capture

