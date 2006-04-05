/*
 *    sonar.h
 *
 *    Hi resolution side scan sonar include file for 
 *    QMIPS data.
 */

#include <math.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>

#define TRECSIZE     2304    /* size of a QMIPS record on disk (bytes) */
#define RAWDATASIZE    2048    /* size of sonar +/- subbottom image data  */
#define SDATASIZE     2048    /* size of one record of port and starboard (bytes) */
#define HEADSIZE    1024    /* size of header data in QMIPS file */
#define ALONG_DEC     3    /* along-track decimation */
#define ACROSS_DEC     4    /* across-track decimation */
#define PMODE         0666    /* pmode of output files */

#define FILE_HEADER_SIZE    1024    /* demuxed file header size */ 

/*
 *       window size of decimation kernel 
 */

#define C_WINSIZE ((ALONG_DEC) * (ACROSS_DEC))

/* 
 *       sonar data size after decimation 
 */

#define C_SDATASIZE ((SDATASIZE) / (ACROSS_DEC))

#define C_PDATASIZE     24        /* position data size */
#define LONG_C_PDATASIZE  256     /* position data size for revision 1 */
#define LONG_POS_SIZE  256        /* size of position data */
#define C_TOTSIZE ((C_SDATASIZE) + (C_PDATASIZE)) /* size of short buffer */
#define LONG_C_TOTSIZE ((C_SDATASIZE) + (LONG_C_PDATASIZE))  /* long buffer */
#define IMAGE_HEADER_SIZE 50 

/*    
 *        sizes (elements) of spline arrays
 */

#define MAX_NNODES     480    /* max. number of navigation nodes (120 hrs) */
#define MAX_NSPL     6000     /* max. number of spline points */

/*    
 *    geometery of sonar swath
 */
#define NEAR_DIST 0.02    /*  distance from nadir to first data as ratio of swath width */
#define SWATH 200.0       /*  Klein swath width @ 7.5 ping/second pulse rate */
#define PING_INT 0.1333333   /*  Klein ping interval @ 200 meter swath width */

/*
 *    ranges of dn values being used
 */

#define MIN_DN 1 
#define MAX_DN 254

/*    
 *    dn values for screen background and grid line and shadows
 */
#define SCREENBACKGROUND 255
#define SCREENGRID 0
#define SHADOW 1

/*   
 *    radian to degree conversion -- to get from degrees to radians,
 *    divide the number of degrees by this number.  To get from radians
 *    to degrees, multiply the number of radians by this number.
 */

#define RAD_TO_DEG  57.295780

/*    
 *    used to increment the fake UTM coordinates if navigation
 *    is to be faked in the demux process.
 */

#define INCREMENT_UTM  5.0
#define SEC_NET 600    

/*    
 *    structure c_buf, in which the compacted data are stored (old buffer)
 */
struct c_buf {
    float djday;
    float utm_n;
    float utm_e;
    float utm_azi;
    float depth;
    float alt;
    unsigned char s_data[C_SDATASIZE];
};

/*    
 *    file header structure.  Added to system on 4/5/91 to accomodate
 *    new information for each file. --bwd
 */

struct file_header {
    unsigned char spare[1024];
};


/*    
 *    New structure for compacted data which has been expanded
 *    to hold more variables -- updated 6/11/90 -- bwd
 *    expanded this out to 256k -- 11/13/90 -- bwd
 *    updated 4/18/94 -> added clon variable (central longitude) -- bwd
 */

struct long_c_buf {         /* total header size = 256 bytes                  */
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
    unsigned char s_data[C_SDATASIZE];
};

struct long_pos_buf {
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
};

/*    structure for the header of screen image files put directly on disc
*/
struct image_header {
    int image_w;
    int image_h;
    int pix_size;
    int back_color;
    float west;
    float south;
    float c_long;
    char filename[30];
};

