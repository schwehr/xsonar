/*
 * showimage.h 
 *
 * Include file for showimage
 * 
 * William W. Danforth
 * U.S. Geological Survey
 * 7/30/91
 *
 * I'm currently working on reducing these globals, hopefully to none ...
 *
 */

#define TOP_DIR     "*"

#define USGS          2
#define UNDEFINED     3
#define NOSONAR       4
#define NOTELEM       5
#define NOZOOMWIN     6
#define NOZOOM        7
#define NODATA        8
#define NOTYPE        10
#define READING       11
#define BADFILE       12
#define NOSCAN        13
#define BADWRITE      14
#define BADFONT       15
#define BEGINFILE     16
#define QMIPS_DSP     17
#define NOSTBDIMAGE   18
#define NOPORTIMAGE   19
#define NOMEMORY      20
#define NOTXSONAR     21
#define NOTQMIPS      22
#define NOTRASTER     23

#define PORTALT       600
#define STBDALT       601
#define ZOOMPORTALT   602
#define ZOOMSTBDALT   603

#define PORTBEAMLIMIT       604
#define STBDBEAMLIMIT       605
#define ZOOMPORTBEAMLIMIT   606
#define ZOOMSTBDBEAMLIMIT   607

#define XSONAR   100
#define WHIPS    101
#define MIPS     102
#define RASTER   103
#define ISIS     104
#define EGG      105
#define QMIPS    106

#define MAX_MEMORY  48000000 /* 24 megabytes of mem. */
                             /* Adjust for system as neccessary. */

#ifdef vax
#define VAX 1
#else
#define VAX 0
#endif

#if defined(mc68000) || defined (sony) || defined(sgi) || defined(sun)
#define MOTOROLA 1
#else
#define MOTOROLA 0
#endif

#if defined(ultrix) || defined(__alpha) || defined(__ultrix)  || defined(i386)
#define INTEL 1
#else
#define INTEL 0
#endif


#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/DrawingA.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/PanedWP.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Scale.h>

#include "qmips.h"

struct draw {
     XtAppContext topLevelApp;     /* application context                     */
     Widget shell;                 /* main shell for program                  */
     GC main_gc;                   /* GC for the draw area                    */
     GC zoom_gc;                   /* GC for the zoom area                    */
     XGCValues zoom_GC_values;     /* for handling the GC                     */
     Widget graphics;              /* area for graphics                       */
     Widget spare;                 /* spare widget to pass in callbacks       */
     Colormap graphics_colormap;   /* graphics gray map                       */
     Pixmap imagePixmap;           /* Image pixmap used for expose redraws    */
     Pixmap zoomPixmap;            /* Pixmap used for zoom area redraws       */
     XColor gray[256];             /* Xcolor struct for rgb values            */
     float clon;                   /* Central longitude for map area          */
     Visual *visual;
     long grayPixels[55]; /*  This will be an index starting from zero for */
                          /*  the gray scale pixels allocated in grayMap   */
     XColor grayMap[55];  /*  Used for TrueColor grayscale allocation      */

     int bitsPerPixel;
     int base_x;
     int base_y;
     int zoom_x;
     int zoom_y;
     int zoomWidth;
     int zoomHeight;
     int magnifiedZoomWidth;
     int top_ping;
     int magnify;
     int fileType;
     int portBitShift;
     int stbdBitShift;
     int numImageryChannels;
     int pixelsPerChannel;
     }; 

/*
 *    structure for the header of screen image files put directly on disk
 */

#define LSB 0       /*  definitions for the byteOrder member of ImageHeader */
#define MSB 1

typedef struct {
    int type;
    int byteOrder;
    int width;
    int height;
    float pix_size;
    int back_color;
    int spare[4];
    double east;
    double west;
    double north;
    double south;
    double centralLon;
} ImageHeader;

