#------------------------------------------------------------------------
# Makefile for showimage
#------------------------------------------------------------------------

#
#  For newer versions of RedHat Linux Motif (2.1.10),
#  set the -lXpm library to -lXp.
#

#LIBFLAGS= -L/usr/X11R6/lib -lXpm -lXext -lXmu -lXm -lXt -lX11

LIBFLAGS= -L/usr/X11R6/lib -lXp -lXext -lXmu -lXm -lXt -lX11
MAPLIB= -lmap
MATHLIB= -lm
OPTFLAGS= -I/usr/X11R6/include
CFLAGS= -c -g $(OPTFLAGS)
HEADERS=        showimage2.h sonar.h


OBJS= 	main.o display.o telemetry.o messages.o featureRemove.o \
		repaint.o getSwath.o getFile.o equalize.o events.o \
		fileSelect.o flipColor.o getScanSize.o zoom.o \
		correctAlt.o writeAlt.o showAlt.o stretch.o optionsFile.o \
		getHeaderInfo.o setFileType.o getClon.o utilities.o progress.o \
                newClon.o bitShift.o showTrace.o traceBeamLimits.o writeBeamLimits.o \
                showBeamLimits.o

SRCS= 	main.c display.c telemetry.c messages.c featureRemove.c \
		repaint.c getSwath.c getFile.c equalize.c events.c \
		fileSelect.c flipColor.c getScanSize.c zoom.c \
		correctAlt.c writeAlt.c showAlt.c stretch.c optionsFile.c \
		getHeaderInfo.c setFileType.c getClon.c utilities.c progress.c \
                newClon.c bitShift.c showTrace.c traceBeamLimits.c writeBeamLimits.c \
                showBeamLimits.c


IFLAGS= -I/usr/include/mit/X11
CC= gcc
RM= rm -f

#------------------------------------------------------------------------
# programs
#------------------------------------------------------------------------


.c.o:
	$(RM) $@
	$(CC) $(CFLAGS)  $*.c

#$(SRCS):	$(HEADERS)

all:: showimage

bitShift.o:		 	bitShift.c showimage2.h externals.h

correctAlt.o:		 	correctAlt.c showimage2.h externals.h

display.o: 			display.c showimage2.h externals.h

equalize.o: 			equalize.c showimage2.h externals.h

events.o: 			events.c showimage2.h externals.h

fileSelect.o:		 	fileSelect.c showimage2.h externals.h

flipColor.o: 			flipColor.c showimage2.h externals.h

getClon.o: 			getClon.c showimage2.h externals.h

getFile.o: 			getFile.c showimage2.h externals.h

getHeaderInfo.o: 		getHeaderInfo.c showimage2.h externals.h qmips.h

getScanSize.o:	 		getScanSize.c showimage2.h externals.h

getSwath.o: 			getSwath.c showimage2.h externals.h

main.o:				main.c showimage2.h externals.h sonar.h iconBitmap.h

messages.o: 			messages.c showimage2.h externals.h

newClon.o: 			newClon.c showimage2.h externals.h

optionsFile.o: 			optionsFile.c showimage2.h externals.h

progress.o: 			progress.c showimage2.h externals.h

repaint.o: 			repaint.c showimage2.h externals.h

featureRemove.o:		featureRemove.c showimage2.h externals.h

setFileType.o: 			setFileType.c showimage2.h externals.h

showAlt.o:	 		showAlt.c showimage2.h externals.h

showBeamLimits.o:	 	showBeamLimits.c showimage2.h externals.h

showTrace.o:	 		showTrace.c showimage2.h externals.h

stretch.o: 			stretch.c showimage2.h externals.h

telemetry.o:			telemetry.c showimage2.h externals.h sonar.h

traceBeamLimits.o:		traceBeamLimits.c showimage2.h externals.h sonar.h

utilities.o:			utilities.c sonar.h externals.h

writeAlt.o: 			writeAlt.c showimage2.h externals.h

writeBeamLimits.o:		writeBeamLimits.c showimage2.h externals.h sonar.h

zoom.o: 			zoom.c showimage2.h externals.h

showimage:	$(OBJS)
	$(RM) $@
	$(CC) $(OPTFLAGS) -o $@ $(OBJS) $(LIBFLAGS) $(MAPLIB) $(MATHLIB)

clean:
	rm -f *.o showimage
