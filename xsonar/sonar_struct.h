#ifndef _SONAR_STRUCT_H_
#define _SONAR_STRUCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#ifndef __APPLE__
#include <values.h>
#endif
#include <Xm/Xm.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#if 0
/* Now detect for any host using byteOrder program */
#if defined(__alpha) || defined(ultrix) || defined(__ultrix) || defined(i386)
#define INTEL 1
#else
#define INTEL 0
#endif

#if defined(mc68000) || defined(sony) || defined(sgi) || defined(sun) || defined(__sparc)
#define MOTOROLA 1
#else
#define MOTOROLA 0
#endif

#endif

#if defined(sony) || defined(__sparc) || defined(sun)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <libgen.h>
#endif

#define FILEHEADERSIZE    1024               /* for QMIPS and ISIS files */
#define HEADSIZE          256                /* for USGS type files */

#define TDU     10
#define SCREEN  20

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_MEMORY  100000000 /* 100 megabytes of mem. */
                             /* Adjust for system as neccessary. */


/*
 *    Ellipsoid definitions
 */

#define WGS84        1
#define CLARKE1866   2

/*    file header structure.  Added to system on 4/5/91 to accomodate
 *    new information for each file. --bwd
 */

struct file_header {
    unsigned char spare[2048];
};


/*      radian to degree conversion -- to get from degrees to radians,
 *      divide the number of degrees by this number.  To get from radians
 *      to degrees, multiply the number of radians by this number.
 */

#define     RAD_TO_DEG     57.295780

#define     PMODE        0666         /* file permissions mask */
#define     SWATH         200         /* default swath width   */
#define    EPSILON         0.01
#define    MAX(A, B)    ((A) > (B) ? (A) : (B))
#define    MIN(A, B)    ((A) < (B) ? (A) : (B))

typedef struct {           /* typedef for file conversion functions */
       int inputFileFP;
       int outputFileFP;
       int answer;
       int inputFormat;
       int outputFormat;
} FileFormat;

#define XSONAR   100
#define WHIPS    101
#define MIPS     102
#define RASTER   103
#define ISIS     104
#define EGG      105
#define QMIPS    106
#define SEGY     107
#define EGGMIDAS 108

typedef struct {           /* typedef for filter functions */
    unsigned int rows;
    unsigned int columns;
    int elementSize;
    char **elements;
    } Matrix;

typedef struct {            /* structure for image boundaries */
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    double east;
    double west;
    double north;
    double south;
} ImageBounds;

/*
 *    New structure for compacted data which has been expanded
 *    to hold more variables -- updated 6/11/90 -- bwd
 *    expanded this out to 256k -- 11/13/90 -- bwd
 *    updated 4/18/94 -> added clon variable (central longitude) -- bwd
 */

typedef struct {         /* total header size = 256 bytes                    */
    double djday;        /* 0 decimal julian day for time                    */
    double utm_n;        /* 8 UTM northing for ping position                 */
    double utm_e;        /* 16 UTM easting for ping position                 */
    double utm_azi;      /* 24 towfish azimuth determined from spline        */
    float fish_azi;      /* 32 original azimuth readings from towfish        */
    float latitude;      /* 36 latitude from QMIPS data file                 */
    float longitude;     /* 40 longitude from QMIPS data file                */
    float course;        /* 44 course from nav -- Ship Gyro                  */
    float pitch;         /* 48 fish pitch from telemetry                     */
    float roll;          /* 52 fish roll from telemetry                      */
    float depth;         /* 56 pressure depth in meters                      */
    float alt;           /* 60 altitude in meters of sonar vehicle           */
    float total_depth;   /* 64 depth of seafloor at vehicle position         */
    float range;         /* 68 slant range to fish from Sonotech system      */
    float sec;           /* 72 total seconds from 00:00 GMT                  */
    int year;            /* 76 from nav system, saved in data file           */
    int julian_day;      /* 80 from nav system, saved in data file           */
    int sdatasize;       /* 84 # of bytes in ping: header + data             */
    int swath_width;     /* 88 swath width in meters                         */
    float pulse_width;   /* 92 pulse width of outgoing in milliseconds       */
    int power;           /* 96 transmit power                                */
    float clon;          /* 100 central longitude for UTM zone calculations  */
    int merged;          /* 104 flag to indicate whether nav is merged       */
    int fileType;        /* 108 flag to indicate file type (XSONAR)          */
    int byteOrder;       /* 112 flag for processor type used to create file  */
    float temperature;   /* 116 water temp in degrees C                      */
    float conductivity;  /* 120 in siemens / meter                           */
    float waterPressure; /* 124 in psia                                      */
    float shipsSpeed;    /* 128 in knots                                     */
    float portBeamLimit; /* 132 max range to use port in beam patt correction*/
    float stbdBeamLimit; /* 136 max range to use stbd in beam patt correction*/
    int spare[29];       /* 116 spare bytes for future use                   */
} PingHeader;


/*
 *    structure for the header of screen image files put directly on disc
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

/*    
 *    Main structure passed to each Motif callback as a pointer
 *    (client_data).
 */

typedef struct {                  /* structure for map */
    XtAppContext toplevel_app;    /* application context */
    Widget toplevel;              /* top level shell */
    Widget window;                /* spare draw window */
    Widget nav_window;            /* nav window */
    Widget popup_widget;          /* reference to pop up widgets */
    Widget spare1;                /* some spare widgets to pass */
    Widget spare2;
    Widget spare3;
    Widget spare4;
    GC main_gc;                   /* gc for app */
    GC nav_gc;                    /* gc for navigation */
    Colormap colormap;            /* colormap for app */
    Pixmap orient_pixmap;         /* pixmap for orient window */
    Pixmap nav_pixmap;            /* pixmap for nav window */
    int swath;                    /* swath width of sonar */
    double scale;                 /* scale for output */
    int hour_int;                 /* display data every hour_int hours */
    int infd;                     /* file handle for input */
    int outfd;                    /* file handle for output */
    FILE *navfp;                  /* file handle for nav output */
    FILE *magfp;                  /* file handle for maggy output */
    int mer_grid_int;             /* interval for mercator grid */
    double c_lon;                 /* central longitude of data file */
    double pix_ns;                /* takes care of aspect problems for TDU */
    double pix_ew;                /* 850: prints 203x200 pixels / sq. in. */
    double pix_s;                 /* on screen pixel size */
    double rasterPixSize;         /* raster image pixel size */
    double mapPixSize;            /* survey map image pixel size */
    double west;                  /* bounds of data screen for TDU display */
    double east;
    double south;
    double north;
    double mapWest;               /* bounds of data screen for map */
    double mapEast;
    double mapSouth;
    double mapNorth;
    double rasterWest;            /* bounds of data screen for raster image */
    double rasterEast;
    double rasterSouth;
    double rasterNorth;
    double utm_west[20];          /* bounds of oriented screen data for TDU */
    double utm_south[20];
    int number_of_images;
    char *outfile;                /* output file names */
    char infile[255];             /* input file names */
    char *navfile;                /* input navigation file */
    unsigned char *image;         /* pointer to image data */
    PingHeader ping_header;       /* sonar header */
    PingHeader next_ping_header;  /* sonar header */
    int beamNumLines;             /* Number of pings to use in beam correct */
    int beamAverageLines;         /* Use this to smooth transition */
    int beamResponseAngle;        /* Angle to use for angular response stats */
    int maxBeamAngle;             /* max grazing angle for beam correction */
    int normalize;                /* Toggle for switching on normalization */
    int portNormalizeValue;       /* Value to normalize data set to for port*/
    int stbdNormalizeValue;       /* Value to normalize data set to for stbd*/
    int portMaxNormalizeValue;    /* Leave 16 bit values unchanged above this*/
    int stbdMaxNormalizeValue;    /* Leave 16 bit values unchanged above this*/
    int bitShift;                 /* Value to normalize data set to */
    int adjustBeam;               /* Adjust port/stbd brightness if true */
    int findAltFlag;              /* Flag for auto tracing first return */
    int findAltThreshold;         /* Search strength for alt auto tracing */
    int altManuallyCorrected;     /* Flag to indicate alt traced with mouse */
    int meterRange;               /* useable meters of data from nadir */
    int scrheight;                /* screen height in pixels */
    int scrwidth;                 /* screen width in pixels */
    int along_track;              /* number of pixels for median filter */
    int across_track;             /* number of pixels for median filter */
    int fish_azi_flag;            /* get fish azimuth values if set */
    int utm_flag;                 /* display utm grid if set */
    int mercator_flag;            /* display mercator grid if set */
    int latlon_flag;              /* retrieve lat-lon from data if set */
    int magnetometer_flag;        /* retrieve maggy settings from data */
    int eastnorth_flag;           /* retrieve eastings-northings if set */
    int median_filter_size;       /* boxcar size for desampling QMIPS file */
    float navInterval;            /* output ascii nav every nav_interval */
    int magnetic_declination;     /* deviation from map north */
    int useBeamLimits;            /* Toggle for max range for beam correction */
    int demuxFlag;                /* Series of flags for demux/processing */
    int eightBit;                 /* parameters */
    int sixteenBit;
    int navmerge_flag;
    int slant_flag;
    int destripe_flag;
    int beam_flag;
    int linear_flag;
    int equalize_flag;
    int removeRampPortFlag;
    int removeRampStbdFlag;
    int startScanForRemoveRampPort;
    int endScanForRemoveRampPort;
    int startScanForRemoveRampStbd;
    int endScanForRemoveRampStbd;
    int removeRampPortPivot;
    int removeRampStbdPivot;
    int removeRampRangeStartPort;
    int removeRampRangeStartStbd;
    int getCurveModelText;
    FILE *tvgRampTextFile;        /* file handle for user supplied TVG model */
    char tvgRampTextFileName[255];
    int skipScans;
    int portOffset;
    int stbdOffset;
    int portBitShift;
    int stbdBitShift;
    int stretch_params[3];        /* parameters for linear stretch */
    float scaleInt;               /* scan line interval in meters */
    char *file_format;            /* input file format */
    int setReturnWhite;           /* sets high backscatter to white */
    int setReturnBlack;           /* sets high backscatter to black */
    int nadirTextOut;             /* flag to send nadir position to text file */
    float tduPixelsNS;            /* Number of pixels used to map NS on TDU */
    int fileType;                 /* file type input to the program */
    int ellipsoid;                /* ellipoid to use in the mapping routines */
    int tduFlag;
    int aldenFlag;
    int rasterFlag;
    int expose_counter;
    char currentDirectory[255];   /* keep track of current directory      */
    XColor redPixel;       /* pixel values if default visual is    */
    XColor greenPixel;     /* TrueColor.  Otherwise these are hard */
    XColor yellowPixel;    /* coded for PseudoColor                */
    XColor whitePixel;     /*                                      */
    XColor colors[256];    /* This is for the PseudoColor visual   */
    double mosaicRange;    /* range to use for mosaicing           */

} MainSonar; 

/* Need a better header file for these prototypes. */
int getScanSize(MainSonar *sonar, int *scansize, int *headsize);

#endif /* _SONAR_STRUCT_H_ */
