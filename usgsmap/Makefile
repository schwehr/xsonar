# GNU makefile

OS := $(shell uname -s)

ifeq ($(OS),IRIX64)
  RANLIB:=/bin/true
else
  RANLIB:=ranlib
endif

CC := gcc

targets: libmap.a

SRCS := ${wildcard *.c}
OBJS := ${SRCS:.c=.o}

ifdef OPTIMIZE
  CFLAGS := -Wall -O3 -funroll-loops -fexpensive-optimizations
else
  CFLAGS := -g -Wall -Wimplicit -pedantic -Wimplicit-int -Wimplicit-function-declaration -W -Wstrict-prototypes -Wredundant-decls -Wnested-externs
endif
LIB := map
lib$(LIB).a: $(OBJS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@
targets: lib${LIB}.a

clean:
	rm -f *.a *.o

real-clean: clean
	rm -f *~
