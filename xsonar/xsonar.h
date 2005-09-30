/* AUTHOR: Kurt Schwehr
   Prototypes for all c functions in the xsonar directory
*/

#ifndef _XSOANR_H_
#define _XSOANR_H_

#include "sonar_struct.h"
#include "alden.h"

#ifdef __GNUC__
#define UNUSED __attribute((__unused__))
#else
#define UNUSED
#endif

void show_nav(Widget w, XtPointer client_data, XtPointer call_data);

/***************/
/* utilities.c */
/***************/
void setHardReturn(Widget w, XtPointer client_data, XtPointer call_data);

int in_box (double x, double y, ImageBounds *box);
void updateSwath(Widget w, XtPointer client_data,
		 XtPointer call_data);
void writeAldenHeader(int outfd, 
		      AldenHeader *header,
		      int image_w, int image_h, char *fileName);
void checkForNans(MainSonar *sonar);


void cancelCallback(Widget w, XtPointer client_data, XtPointer call_data);
void asciiNadirText(Widget w, XtPointer client_data, XtPointer call_data);
float getcentlon (float lon);
int getFileSize(char *fileName);

void allocateMatrix(Matrix *matrix);
void deAllocateMatrix(Matrix *matrix);
unsigned char c_median (unsigned char *v, int n);
float f_median  (float *v, int n);

double get_double(unsigned char *inbuf, int location, int swap, MainSonar *sonar);
float get_float(unsigned char *inbuf, int location, int swap, MainSonar *sonar);
int get_int(unsigned char *buf, int location, int swap, MainSonar *sonar);
short get_short(unsigned char *inbuf, int location, int swap, MainSonar *sonar);

int numcmp(char *s1, char *s2);
int xy_to_i(int x, int y, ImageHeader *image_params);
void xy_raster (MainSonar *sonar, double east, double north, int *scr_x, int *scr_y, int flag);
void raster_xy (MainSonar *sonar, int scr_x, int scr_y, double *easting, double *northing);


void setRasterFlag(Widget w,
		   XtPointer client_data, XtPointer call_data);
void setAldenFlag(Widget w, XtPointer client_data, 
		  XtPointer call_data);
void setTduFlag(Widget w, XtPointer client_data, 
		XtPointer call_data);


/* setToggle.c */
void setToggle(String toggleName, Widget shell, int status);

/* messages.c */
void messages(MainSonar *sonar, char *text);

/* setup.c */
void setup(Widget w, XtPointer client_data, XtPointer call_data);
void Get_toggle_values(Widget w, XtPointer client_data, XtPointer call_data);

/* sonarMap.c */
void orient_sonar(Widget w, XtPointer client_data, XtPointer call_data);

/* demux.c */
void demux(Widget w, XtPointer client_data, XtPointer call_data);

/* histogram.c */
void histogram(Widget w, XtPointer client_data, XtPointer call_data);

/* combineImages.c */
void combineImages(Widget w, XtPointer client_data, XtPointer call_data);

/* navmerge.c */
void mergenav(Widget w,
	      XtPointer client_data,
	      XtPointer call_data);

void slant(Widget w,
	   XtPointer client_data,
	   XtPointer call_data);

void destripe(Widget w,
	      XtPointer client_data,
	       XtPointer call_data);

void beam(Widget w, XtPointer client_data,
	  XtPointer call_data);

/* linear.c */
void stretch(Widget w,
	     XtPointer client_data,
	     XtPointer call_data);

void equalize( Widget w,
	      XtPointer client_data,
	       XtPointer call_data);

void fileConvert(Widget w, XtPointer client_data,  XtPointer call_data);
void filter(Widget w, XtPointer client_data,   XtPointer call_data);
void mapCoast(Widget w, XtPointer client_data,   XtPointer call_data);

void closeDialog(MainSonar *sonar, Widget w, int error);

void rasterBounds(Widget w, XtPointer client_data,   XtPointer call_data);
void setupFishNav(Widget w, XtPointer client_data,  XtPointer call_data);
void tduSetup(Widget w, XtPointer client_data,  XtPointer call_data);

void ellipsoidSetup(Widget w, XtPointer client_data,  XtPointer call_data);
int createNavText(Widget callingWidget, char *navString, MainSonar *sonar,
		   int numNodes);

void demuxSetup(Widget w, XtPointer client_data, XtPointer call_data);
void BeamPatternSetup(Widget w, XtPointer client_data, 
		       XtPointer call_data);


void FindAltSetup(Widget w, XtPointer client_data,  XtPointer call_data);

void File_select(Widget w, XtPointer client_data,
		  XtPointer callback_data);


Widget setUpProgressWindow(MainSonar *sonar, char *dialog, char *messageBuffer, Dimension *width, Dimension *height, GC *gc);

void tvgRampFileSelect(Widget w, XtPointer client_data,  XtPointer callback_data);
void initializeFilter(  MainSonar *sonar,
		       Matrix *filter,
		         char *type,
		       int kernelNumber);
void runFilter(Widget w, XtPointer client_data,   XtPointer call_data);
int outputFile(MainSonar *sonar, char *filterType);

void filter2d(Matrix *inMatrix, Matrix *outMatrix, Matrix *kernel, char *type, MainSonar *sonar, int addBack, GC gc);
void runFunction(Widget w, MainSonar *sonar);

void flipImage(MainSonar *sonar, int infd);

void whipsWrite(FileFormat *fileToConvert, MainSonar *sonar,
		int inputNavFP, FILE *whipsNavFP);
void convertXsonar(FileFormat *fileToConvert, MainSonar *sonar);
void convertMips(FileFormat *fileToConvert, MainSonar *sonar, char *fileBuffer);
int fishNav(Widget callingWidget, MainSonar *sonar, double *depth, double *sr, int readHeader);

#endif /* _XSONAR_H_ */
