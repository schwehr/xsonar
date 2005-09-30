/*
 *    Utilities.c
 *
 *    Various collection of useful tools.
 */

#include <unistd.h> /* read() */
#include <Xm/ToggleB.h> /* XmToggleButtonGetState */

#include <sys/stat.h>          /*   For file attributes  */
#include <sys/types.h>

#include <map.h>
#include "xsonar.h"
  
#include "sonar_struct.h"
#include "alden.h"

static void aldenError(int outbytes);
static void floatshuf(float *number);
static float floatFlip(float *value);


/*
 *    Generic callback for cancelling a subroutine.  Need to check
 *    status of cancelCheck in subroutine and take appropriate action.
 */

void cancelCallback(Widget w,
		    UNUSED XtPointer client_data,
		    UNUSED XtPointer call_data)
{
    int cancelCheck;

    cancelCheck = True;
    XtVaSetValues(w, XmNuserData, cancelCheck, NULL);

    return;
}

void asciiNadirText(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    if(w == XtNameToWidget(sonar->toplevel, "*NadirYesButton"))
        {
        sonar->nadirTextOut = True;
        }

    if(w == XtNameToWidget(sonar->toplevel, "*NadirNoButton"))
        {
        sonar->nadirTextOut = False;
        }

    return;

}


void setHardReturn(UNUSED Widget w, 
		   XtPointer client_data, 
		   UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;
    Boolean whiteButtonStatus;


    whiteButtonStatus = XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                                             "*WhiteToggle"));

    if(whiteButtonStatus)
        {
        sonar->setReturnWhite = True;
        sonar->setReturnBlack = False;
        }
    else
        {
        sonar->setReturnWhite = False;
        sonar->setReturnBlack = True;
        }

    return;

}


/*
 *    Subroutine to convert an integer n to a string s.
 */
/* FIX: conflicts with maps.h */
#if 0
void itoa (int n, char *s)   
{
	void reverse();
	int i, sign;
	if ((sign = n) < 0)   
		n = n * -1;      
	i = 0;
	do {                   /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   
	} while ((n /= 10) > 0);        
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);

	return;
}
#endif

/*
 *   Reverses string s in place
 */
/* conflicts with maps.h */
#if 0
void reverse(char *s)
{
	char c; 
        int i, j;
	for (i =  0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}

	return;
}
#endif

/*	getcentlon()
 *
 *	Returns the central longitude for the array lon in the UTM standard 
 */

float getcentlon (float lon)
     /*float lon;*/
{
	int fuse;
	float c_lon;


	if (lon > 0.0) 
		lon += 6.0;
	else
		lon -= 6.0;
	fuse = lon / 6;

	if (lon > 0.0) 
		fuse -= 1;
	c_lon = 3.0 + fuse * 6.0;

	return(c_lon);
}

/*
 *    Return the size of the file indicated by fileName.
 */
/* FIX: Make const */
int getFileSize(char *fileName)
     /*char *fileName;*/
{

    struct stat fileParams;

    if(stat(fileName, &fileParams) == -1)
        return(0);

    return(fileParams.st_size);

}

void allocateMatrix(Matrix *matrix)
     /*Matrix *matrix;*/
{

  unsigned int i; /* FIX: make sure unsigned is ok */
    short **members;

    members = (short **) XtCalloc(matrix->rows, sizeof(short *));
    if(!members)
        {
        printf("\nError making pointers in %dx%d file matrix\n",
                                           matrix->rows, matrix->columns);
        exit(1);
        }
    for(i = 0 ; i < matrix->rows ; i++)
        {
        members[i] = (short *) XtCalloc(matrix->columns,sizeof(short));
        if(!members[i])
            {
            printf("\nError making row %d in %dx%d file matrix\n",
                                       i, matrix->rows, matrix->columns);
            exit(1);
            }

        }

    matrix->elements = (char **) members;

    return;

}

void deAllocateMatrix(Matrix *matrix)
     /*Matrix *matrix; */
{

  unsigned int i; /* FIX: make sure unsigned is ok */

    for(i = 0 ; i < matrix->rows ; i++)
        XtFree(matrix->elements[i]);

    XtFree((char *)matrix->elements);

    return;

}

/*    c_median()
 *
 *    sorts v[0]...v[n-1] in increasing order and returns the median.
 *    Modified from Kernighan & Ritchie, p. 58
 */
unsigned char c_median (unsigned char *v, int n)
{
    int gap, i, j;
    unsigned char temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j=i-gap; j>=0 && v[j]>v[j+gap]; j-=gap)
                {
                temp = v[j];
                v[j] = v[j+gap];
                v[j+gap] = temp;
                }

    return (v[n/2]);
}

/*    f_median()
 *
 *    sorts v[0]...v[n-1] in increasing order and returns the median.
 *    Modified from Kernighan & Ritchie, p. 58
 */
float f_median  (float *v, int n)
{
    int gap, i, j;
    float temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j=i-gap; j>=0 && v[j]>v[j+gap]; j-=gap) {
                temp = v[j];
                v[j] = v[j+gap];
                v[j+gap] = temp;
            }
    return (v[n/2]);
}

/*    get_double()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE double precision
 *    floating point numbers (8 bytes) in files
 *    transferred from a mc680xx machine.
 *
 */

double get_double(unsigned char *inbuf, int location, int swap, MainSonar *sonar)
{
    int i;
    unsigned char *ptr1, *ptr3;
    double *ptr2;
    double value;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (double));

    if(sonar->fileType == EGGMIDAS && (MOTOROLA))
        {
        ptr1 += location;
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1++;
        }
    else if(MOTOROLA || swap)
        {
        ptr1 += (location + 7);
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1--;
        }
    else if(sonar->fileType == EGGMIDAS && (INTEL))
        {
        ptr1 += (location + 7);
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 8;

    ptr2 = (double *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_float()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE single precision
 *    floating point numbers (4 bytes) in files
 *    transferred from a mc680xx machine.
 */

float get_float(unsigned char *inbuf, int location, int swap, MainSonar *sonar)
{
    int i;
    unsigned char *ptr1, *ptr3;
    /*unsigned char temp[5];*/
    float *ptr2;
    float value;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (float));

    if((MOTOROLA || swap) && sonar->fileType != EGGMIDAS)
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
	else if(sonar->fileType == EGGMIDAS && (INTEL))
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 4;

    ptr2 = (float *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_int()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE integer
 *    numbers (4 bytes) in files
 *    transferred from a mc680xx machine and vice versa.
 */

int get_int(unsigned char *buf, int location, int swap, MainSonar *sonar)
{
    int i;
    unsigned char *ptr1, *ptr3;
    int *ptr2;
    int value;

    ptr1 = buf;
    ptr3 = (unsigned char *) calloc (1, sizeof (int));

    if(MOTOROLA || (swap && sonar->fileType != EGGMIDAS) )
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
	else if(sonar->fileType == EGGMIDAS && (INTEL))
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 4;

    ptr2 = (int *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_short()
 *
 *    Subroutine to enable Intel based processors to
 *    read short integer numbers (2 bytes)
 *    in files transferred from a mc680xx machine.
 *
 */

short get_short(unsigned char *inbuf, int location, int swap, MainSonar *sonar)
{
    int i;
    unsigned char *ptr1, *ptr3;
    short value;
    short *ptr2;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (short));

    if((MOTOROLA || swap) && sonar->fileType != EGGMIDAS)
        {
        ptr1 += (location + 1);
        for(i = 0; i < 2; i++)
              *ptr3++ = *ptr1--;
        }
	else if(sonar->fileType == EGGMIDAS && (INTEL))
        {
        ptr1 += (location + 1);
        for(i = 0; i < 2; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 2; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 2;

    ptr2 = (short *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*
 *  numcmp:  compare to elements numerically.  Taken from K & R, 2nd edition,
 *           page 121.
 */

int numcmp(char *s1, char *s2)
{

    double v1, v2;

    v1 = atof(s1);
    v2 = atof(s2);

    if(v1 < v2)
        return -1;
    else if(v1 > v2)
        return 1;
    else
        return 0;

}

/*****************************************************************************
 *
 *    xy_to_i()
 *
 *    Return the index i that corresponds to the coordinates x and y
 *    for the array image[].
 */
int xy_to_i(int x, int y, ImageHeader *image_params)
{
    return ((int)(image_params->width * y + x));
}

/*****************************************************************************
 *
 *    xy_raster()
 *
 *    Return the integer screen coordinates of the point with coordinates
 *    east and north
 *    NOTE: these coordinates are according to a raster world,
 *    where the LOWER left corner has coordinates (0,0).

THIS IS THE RIGHT CONVERSION TO COMPENSATE FOR THE TDU ASPECT RATIO:
(use in xy_image and xy_raster)

    sonar->pix_ew = ((double)sonar->scale / 203.0) * 0.0254;
    sonar->pix_ns = ((double)sonar->scale / 201.5) * 0.0254;

     utm_grid_int_north = (sonar->pix_s * 200.5);
     utm_grid_int_west = (sonar->pix_s * 203.0);

 */

void xy_raster (MainSonar *sonar, double east, double north, int *scr_x, int *scr_y, int flag)
{
    if(flag == TDU)
        {
        double tdu850_conv_ew = (sonar->scale / 203.0) * 0.0254;
        double tdu850_conv_ns = (sonar->scale / sonar->tduPixelsNS) * 0.0254;

        *scr_x = (int)(((east - sonar->west) / tdu850_conv_ew) + 0.5);
        *scr_y = (int)(((sonar->north - north) / tdu850_conv_ns) + 0.5);
        }
   
    if(flag == SCREEN)
        {
        *scr_x = (int)((east - sonar->west) / sonar->pix_s + 0.5);
        *scr_y = (int)((sonar->north - north) / sonar->pix_s + 0.5);
        }

}

void test_xy_raster (MainSonar *sonar, double east, double north, int *scr_x, int *scr_y)
{
    double pix_conv = (sonar->scale / 200.0) * 0.0254;
    double pix_conv_ns = (sonar->scale / 197.0) * 0.0254;

    *scr_x = (int)(((east - sonar->west) / pix_conv) + 0.5);
    *scr_y = (int)(((sonar->north - north) / pix_conv_ns) + 0.5);
}

/*
 *
 *    raster_xy()
 *
 *    Return the utm east and west coordinates of the point with screen
 *    coordinates
 *    NOTE: these coordinates are according to a raster world,
 *    where the LOWER left corner has coordinates (0,0).
 *    NOTE TOO: xy_raster() uses a rounding algorithm which is not
 *    inverse reporduced here; therefore, if you run
 *    xy_raster() then raster_xy(), you may have slightly
 *    different results (+/- pix_s/2)...dcw
 */

void raster_xy (MainSonar *sonar, int scr_x, int scr_y, double *easting, double *northing)
{
    *easting  = (double)(scr_x * sonar->pix_s) + sonar->west;
    *northing = sonar->north - (double)(scr_y * sonar->pix_s);

    return;
}



/*****************************************************************************
 *
 *    in_box()
 *
 *    returns 1 if the point (x,y) is inside a box, 0 otherwise
 */
int in_box (double x, double y, ImageBounds *box)
{
    if ((x > box->west) && (x < box->east) &&
    (y > box->south) && (y < box->north))
        return (1);
    else
        return (0);
}


void setRasterFlag(UNUSED Widget w,
		   XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*PlaceBox"), False);

    sonar->rasterFlag = TRUE;
    sonar->tduFlag = FALSE;
    sonar->aldenFlag = FALSE;

    return;

}

void setTduFlag(UNUSED Widget w, XtPointer client_data, 
		UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*PlaceBox"), True);

    sonar->rasterFlag = FALSE;
    sonar->tduFlag = TRUE;
    sonar->aldenFlag = FALSE;

    return;

}

void setAldenFlag(UNUSED Widget w, XtPointer client_data, 
		  UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*PlaceBox"), True);

    sonar->rasterFlag = FALSE;
    sonar->tduFlag = FALSE;
    sonar->aldenFlag = TRUE;

    return;

}

void updateSwath(Widget w, XtPointer client_data,
		 UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget progressWindow;
    /*Widget setUpProgressWindow();*/

    GC progressGC;

    Dimension progressWidth;
    Dimension progressHeight;
    Dimension drawAmount;

    char warningmessage[255];
    char messageBuffer[255];
    char callingDialog[100];

    struct stat file_params;

    int i;
    int infd;
    int scansize, imagesize;
    int headsize;
    int checkFileStatus;

    long fileSize, position;

    float percentDone;

    /*void messages();*/

    /*
     *   Open input and output files.
     */

    if((infd = open(sonar->infile, O_RDWR)) == -1)
        {
        sprintf(warningmessage,
              "Error opening input file\nPlease select a data file in Setup");
        messages(sonar, warningmessage);
        return;
        }

    /*
     *   Make sure the data file can be read.
     */

    if (read (infd, &sonar->ping_header, 256) != 256)
         {
         sprintf(warningmessage, "Cannot read first header");
         messages(sonar, warningmessage);
         close(infd);
         return;
         }

    position = lseek(infd, 0L, SEEK_SET);

    if(w == XtNameToWidget(sonar->toplevel, "*SwathButton"))
        strcpy(messageBuffer, "Updating swath width ...");
    else if(w == XtNameToWidget(sonar->toplevel, "*UpdateHeaderButton"))
        strcpy(messageBuffer, "Updating header ...");
#if 0
    else
        ;  /* this won't happen, reserved for future options */
#endif

    strcpy(callingDialog, "*SetupDialog");

    progressWindow = setUpProgressWindow(sonar, callingDialog, 
                   messageBuffer, &progressWidth, &progressHeight, &progressGC);

     /*
      *     Find file size, and compute number of pings.
      *     Then compute total number of minutes (time) for file.
      */

    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;
    headsize = HEADSIZE;

    if(imagesize % 512)
        getScanSize(sonar, &scansize, &headsize);

    if((checkFileStatus = stat(sonar->infile, &file_params)) == -1)
        {
        sprintf(warningmessage, "Cannot read data file size");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }

    fileSize = file_params.st_size;

/*
fprintf(stderr, "filesize = %d\n", fileSize);
fprintf(stderr, "scansize = %d\n", scansize);
fprintf(stderr, "imagesize = %d\n", imagesize);
fprintf(stderr, "modulo returns %d\n", imagesize % 512);
fprintf(stderr, "height = %d\n", progressHeight);
fprintf(stderr, "width = %d\n", progressWidth);
*/

    i = 0;

    while(read (infd, &sonar->ping_header, 256) == 256)
        {
        i++;
        sonar->ping_header.swath_width = sonar->swath;
        sonar->ping_header.sdatasize = scansize;
        sonar->ping_header.fileType = XSONAR;
        if(INTEL)
            sonar->ping_header.byteOrder = INTEL;
        if(MOTOROLA)
            sonar->ping_header.byteOrder = MOTOROLA;

        sonar->ping_header.portBeamLimit = sonar->swath / 2;
        sonar->ping_header.stbdBeamLimit = sonar->swath / 2;


        position = lseek(infd, -256L, SEEK_CUR);

        write(infd, &sonar->ping_header, 256);
        position = lseek(infd, (long) imagesize, SEEK_CUR);

        percentDone = ((float) i * scansize / (float) fileSize);

        drawAmount = (Dimension) (percentDone * (float)progressWidth);

        XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

        XmUpdateDisplay(sonar->toplevel);

        }

    /*
     *  Free up the gc created, close down the input file,
     *  and popdown the progress monitor.
     */

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));

    close(infd);

    return;
}


/* FIX: should header really not be used!!??! */
void writeAldenHeader(int outfd, 
		      UNUSED AldenHeader *header,
		      int image_w, int image_h, char *fileName)
{

    int outbytes;
    int i;

    /* 
     *    Alden header.  Each field must be written in the order listed
     *    below.  This header CANNOT be written as a structure on anything
     *    other than a 16 bit machine.
     */

    char    Start_Of_Header;        /* First byte = ASCII SOH */
    short   Left_Margin;            /* Start position 0 -2047 */
    short   Line_Length;            /* Bytes/line 1 - 2048 */
    short   Image_Length;           /* scans/file 1 - 65535 */
    char    Margin_Color;           /* color of both margins 0-255 */
    char    Line_Repeat;            /* # of times to dup. 0 - 7 */
    char    Vertical_Compression;   /* see below */
    char    Pixel_Repeat;           /* # times pixel duped 0 - 7 */
    char    Translation_Select;     /* see below */
    char    Image_Type;             /* see below */
    char    Image_Number[4];        /* see below */
    char    Line_Spacing;           /* distance between scans */
    char    Line_Rate;              /* see below */
    char    Paper_Type;             /* see below */
    short   Feed_Length;            /* see below */
    char    Reserved[10];           /* not used */
    char    Printed_Header[96];     /* see below */
    char    Translation_Table[256]; /* see below */



    /* setup the Alden header record */

    Start_Of_Header = 0x01;   /* ASCII SOH */
    Left_Margin = htons((unsigned short) 0);
    Line_Length = htons((unsigned short)image_w); /* full image */
    Image_Length = htons((unsigned short)image_h);
    Margin_Color = 0;
    Line_Repeat = 0;
    Vertical_Compression = 0;
    Pixel_Repeat = 0;
    Translation_Select = 0;
    Image_Type = 0;
    Line_Spacing = 8;
    Line_Rate = 18;
    Paper_Type = 0;
    Feed_Length = htons((unsigned short) 80);
    for(i = 0; i < 10; i++)
        Reserved[i] = 0x00;
    sprintf(Printed_Header, "%s", fileName);
/*
fprintf(stdout, "header is: %s\n", Printed_Header);
    for(i = strlen(Printed_Header); i < 96; i++)
        Printed_Header[i] = 0x00;
*/
    for(i = 0; i < 256; i++)
        Translation_Table[i] = 0x00;

    if((outbytes = write(outfd, &Start_Of_Header,sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Left_Margin, sizeof(short))) != sizeof(short))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Line_Length, sizeof(short))) != sizeof(short))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Image_Length, sizeof(short))) != sizeof(short))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Margin_Color, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Line_Repeat, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Vertical_Compression, 
                                              sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Pixel_Repeat, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Translation_Select, 
                                              sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Image_Type, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, Image_Number, sizeof(int))) != sizeof(int))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Line_Spacing, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Line_Rate, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Paper_Type, sizeof(char))) != sizeof(char))
        aldenError(outbytes);
    if((outbytes = write(outfd, &Feed_Length, sizeof(short))) != sizeof(short))
        aldenError(outbytes);
    if((outbytes = write(outfd, Reserved, 10)) != 10)
        aldenError(outbytes);
    if((outbytes = write(outfd, Printed_Header, 96)) != 96)
        aldenError(outbytes);
    if((outbytes = write(outfd, Translation_Table, 256)) != 256)
        aldenError(outbytes);

    return;

}


static void aldenError(int outbytes)
{
    fprintf(stderr,"Error writing alden header\n");
    fprintf(stderr,"bytes written: %d\n",outbytes);
    fprintf(stderr, "Exiting ...\n");
    exit(-1);
}

/* floatshuf changes VAX 32 bit floats to Sun floats */

/* FIX: THIS IS BROKEN ?!? */
static void floatshuf(float *number)
{
  printf ("FIX: floatshuf in: %f  ", *number);
  union {
    float *number;
    unsigned int ui4;
  } num;

  unsigned b1,b2,b3,b4;

  b1 = b2 = b3 = b4 = num.ui4;
  b1 >>= 8;
  b2 <<= 8;
  b3 >>= 8;
  b4 <<= 8;

  b1 &= 077600000;
  b2 &= 037700000000;
  if (b2 != 0) {
    if (b2 >= 2) {
      b2 >>= 23;
      b2 -= 2;
      b2 <<= 23;
    } else {
      fprintf(stderr,"\nWarning: real*4 number too small");
    }
  }
  b3 &= 0377;
  b4 &= 0177400;

  num.ui4 = (b1|b2|b3|b4);
  printf ("out: %f  - this should be byte order shifted", *number);  // FIX
  return;
}


float getVaxFloat(unsigned char *inbuf, int location)
{
    int i;
    unsigned char *ptr1, *ptr3;
    float *ptr2;
    float value;


    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (float));

    ptr1 += location;
    for(i = 0; i < 4; i++)
          *ptr3++ = *ptr1++;

    ptr3 -= 4;

    ptr2 = (float *) ptr3;

    floatshuf(ptr2);

    if(INTEL)
        value = floatFlip(ptr2);
    if(MOTOROLA)
        value = *ptr2;

    free(ptr3);

    return(value);
}

static float floatFlip(float *value)
{
    int i;
    unsigned char *ptr1, *ptr3;
    float *ptr2;
    float returnValue;

    ptr1 = (unsigned char *) value;
    ptr3 = (unsigned char *) calloc (1, sizeof (float));

    ptr1 += 3;
    for(i = 0; i < 4; i++)
          *ptr3++ = *ptr1--;

    ptr3 -= 4;

    ptr2 = (float *) ptr3;

    returnValue = *ptr2;

    free(ptr3);

    return(returnValue);
}

void checkForNans(MainSonar *sonar)
{

  //int isnan(double);
    double testValue;

    testValue = (double) sonar->ping_header.alt;

    if(isnan(testValue))
         sonar->ping_header.alt= .2;

    return;

}
