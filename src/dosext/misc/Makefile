
top_builddir=../../..
include $(top_builddir)/Makefile.conf


CFILES=xms.c emm.c

SFILES=
ALL=$(CFILES) $(SFILES)

OBJS=$(CFILES:.c=.o)
OOBJS=bios.o
DEPENDS=$(CFILES:.c=.d)

include $(REALTOPDIR)/src/Makefile.common

all: lib

install: all
