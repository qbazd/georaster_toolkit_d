#
# sbgdal Makefile
# makefile based on Makefile from iniparser
# 

CC      = gcc
CFLAGS  = -fPIC  `gdal-config --cflags`

# Ar settings to build the library
AR	    = ar
ARFLAGS = rcv

SHLD = ${CC} ${CFLAGS}
LDSHFLAGS = -shared -Wl,-Bsymbolic  -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib 
LDFLAGS = -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib `gdal-config --libs`
 
# Set RANLIB to ranlib on systems that require it (Sun OS < 4, Mac OSX)
# RANLIB  = ranlib
RANLIB = true

RM      = rm -f


# Implicit rules

SUFFIXES = .o .c .h .a .so .sl

COMPILE.c=$(CC) $(CFLAGS) -c 
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<) 


SRCS = georaster_toolkit.c

OBJS = $(SRCS:.c=.o)

default: libgeoraster_toolkit.a libgeoraster_toolkit.so georaster_toolkit_api.d

libgeoraster_toolkit.a:	$(OBJS)
	@($(AR) $(ARFLAGS) libgeoraster_toolkit.a $(OBJS))
	@($(RANLIB) libgeoraster_toolkit.a)

libgeoraster_toolkit.so:	$(OBJS)
	@$(SHLD) $(LDSHFLAGS) -o $@.0 $(OBJS) $(LDFLAGS) \
		-Wl,-soname=`basename $@`.0 -L/usr/lib 

georaster_toolkit_api.d: georaster_toolkit_api.h
	dstep georaster_toolkit_api.h -o georaster_toolkit_api.d

clean:
	$(RM) $(OBJS)

veryclean:
	$(RM) $(OBJS) libgeoraster_toolkit.a libgeoraster_toolkit.so* georaster_toolkit_api.d

docs:
	@(cd doc ; $(MAKE))
	
check:
	@(cd test ; $(MAKE))
