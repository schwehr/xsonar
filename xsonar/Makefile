#------------------------------------------------------------------------
# Makefile for xsonar
#------------------------------------------------------------------------

CC := gcc

#
#  For newer versions of RedHat Linux Motif (2.1.10),
#  set the -lXpm library to -lXp.
#
#LIBFLAGS= -L/usr/X11R6/lib -lXpm -lXext -lXmu -lXm -lXt -lX11 

#LIBFLAGS= -L../usgsmap -L/sw/lib -L/usr/X11R6/lib -lXp -lXext -lXmu -lXm -lXt -lX11 
LIBFLAGS= -L../usgsmap -L/sw/lib -L/usr/X11R6/lib -lXm -lXt -lX11 
MAPLIB= -lmap
MATHLIB := -lm

# FIX: make this not complain when it is not present
BYTEORDER:=${shell ./byteOrder}

OPTFLAGS:= -I/usr/X11R6/include -I/sw/include -I../usgsmap ${BYTEORDER}

ifdef OPTIMIZE
  CFLAGS  := -c -Wall -O3 -funroll-loops -fexpensive-optimizations $(OPTFLAGS)
else
  #CFLAGS= -c -Wall $(OPTFLAGS)
  CFLAGS  := -c -g $(OPTFLAGS) -Wall -Wimplicit -Wimplicit-int -Wimplicit-function-declaration -W -Wstrict-prototypes -Wnested-externs
  #   -Wredundant-decls -pedantic
  CXXFLAGS := ${CFLAGS}
endif


# OBJS=   xsonar.o slant.o showNav.o setup.o sonarMap.o messages.o linear.o \
#      beamResponse.o destripe.o navmerge.o utilities.o demux.o \
#      demuxSetup.o histogram.o getTVGFileDialog.o\
#      flipImage.o equalize.o scanSize.o setToggle.o closeDialog.o \
#      whips.o filters.o filter2d.o fileDialog.o combineImages.o \
#      mapCoast.o rasterBounds.o beamSetup.o tduSetup.o progress.o \
#      navText.o convert.o mips.o flipXsonar.o fishnav.o fishEntry.o \
#      ellipsoidSetup.o findAltSetup.o

SRCS=   xsonar.c slant.c showNav.c setup.c sonarMap.c messages.c linear.c \
     beamResponse.c destripe.c navmerge.c utilities.c demux.c \
     demuxSetup.c histogram.c getTVGFileDialog.c\
     flipImage.c equalize.c scanSize.c setToggle.c closeDialog.c \
     whips.c filters.c filter2d.c fileDialog.c combineImages.c \
     mapCoast.c rasterBounds.c beamSetup.c tduSetup.c progress.c \
     navText.c convert.c mips.c flipXsonar.c fishnav.c fishEntry.c \
     ellipsoidSetup.c findAltSetup.c
OBJS:=${SRCS:.c=.o}

#CC= cc
CC= gcc
RM= rm -f


#------------------------------------------------------------------------
# programs
#------------------------------------------------------------------------
# had an rm:	#$(RM) $@
.c.o:
	$(CC) $(CFLAGS) $*.c

build:
	make byteOrder
	make xsonar

all: byteOrder xsonar 

#xsonar:     $(OBJS)
#	$(RM) $@
#	$(CC) $(OPTFLAGS) -o $@ $(OBJS) $(LIBFLAGS) $(MAPLIB) $(MATHLIB)
xsonar:     $(OBJS)
	$(CC) $(OPTFLAGS) -o $@ $(OBJS) $(LIBFLAGS) $(MAPLIB) $(MATHLIB)

byteOrder: byteOrder.c
	$(CC) $< -o $@ -Wall -g

clean:
	rm -f *.o xsonar byteOrder

check:
	grep -n FIX *.[ch]

etags:
	etags *.c
