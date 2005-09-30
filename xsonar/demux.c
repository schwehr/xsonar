/*     
 *    demux.c
 *
 *   demultiplexes a "raw" file of sonar data collected with QMIPS.
 *   The output is a file that contains the data in order from the
 *   starboardmost to the portmost. By default, the data are compacted:
 *    twelve pixels (three along by four across track) become one, by 
 *    taking the median. This is to reduce spot noise (measles) and stripes.
 *
 *      A. Malinverno 
 *      September 30 1987   
 *
 *    Updated:
 *
 *    11/31/89:   Reads data files written by QMIPS.
 *                William Danforth, U.S.G.S
 *
 *    7/25/92:    Updated and revised to run under X11 Release 4 
 *                and Motif 1.1.3.
 *                William Danforth, U.S.G.S.
 *
 *    03/01/00:   Reads data files written by EDGETECH-MIDAS system.
 *                William Danforth, U.S.G.S
 *
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include <map.h>
#include "xsonar.h"

#include "sonar_struct.h"
#include "qmips.h"
#include "egg260.h"
#include "segy.h"

#define EIGHT_BITS 8
#define SIXTEEN_BITS 16

/*
 *   Telemetry data
 */

typedef struct {
        float *raw_depth;
        float *raw_pitch;
        float *raw_roll;
        float *raw_course;
        float *raw_ap_alt;
        float *raw_fish_alt;
    } Telemetry;


/*
 *    Variables used in getQmipsNav()
 */

typedef struct {
    int londeg, latdeg;    
    double lonmin, latmin, latitude, longitude;
    float sectenths;
    int year, month, jday, day, hour, min, sec;
    int previousYear, previousMonth;
    } NavParams;

/*
 *    Just a couple of globals for sizing things.
 */

void close_demux(MainSonar *sonar, Widget w, unsigned char *inbuf, unsigned char *outbuf, unsigned char *outdata);

void c_and_out(MainSonar *sonar, int *c_count, unsigned char *outdata,
	       unsigned char *outbuf, Telemetry *telemetry, int imageSize, 
	       NavParams *navigation, unsigned char *inbuf, int *lastsec, 
	       int *navFileUpdate, 
	       Widget parent,
	       unsigned char *c_temp, 
	       int dspData, int outfd, int *result, int totalDataSize);

void getEggNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf);
void getSegyNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf);
void getMidasNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf);


void getMidasSizes(int *sonarDataSize, int infd);
int readQmipsHeader(MainSonar *sonar, unsigned char *header, int infd, int *sonarDataSize, int *halfScan, int *pixelsPerChannel, int *scanSize, int *totalDataSize, int *new_flag, int *dsp_data, Widget demuxDialog, int *numChannels, int *bytesPerPixel, int *numSonarChannels);
int readSegyHeader(MainSonar *sonar, unsigned char *header, int infd, int *sonarDataSize, int *halfScan, int *pixelsPerChannel, int *scanSize, int *totalDataSize, Widget demuxDialog, int *numChannels, int *bytesPerPixel, int *numSonarChannels);

int getQmipsNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf, int totalDataSize, int *navFileUpdate);

void correct_fish_azi(Telemetry *telemetry, MainSonar *sonar);

void getQmipsTelemetry(MainSonar *sonar, Telemetry *telemetry, int totalDataSize, unsigned char *inbuf, int imageSize, NavParams *navigation, int dspData);
void getEggTelemetry(MainSonar *sonar, Telemetry *telemetry, int totalDataSize, unsigned char *inbuf, int imageSize, NavParams *navigation);





void demux(Widget w, XtPointer client_data,  UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    Widget demuxDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget separator2;
    Widget displayPings;
    Widget buttonHolder;
    Widget cancelButton;
    Widget fileLabel;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    unsigned char *c_temp;
    unsigned char *inbuf;     /* input buffer for sonar data + header */
    unsigned char *outbuf;    /* Place to store sonar data for decimation */
    unsigned char *outdata;    /* Place to store finalized sonar data */

    double tempDoubleValue;

    /* FIX: can be used uninitialized */
    unsigned int *portCurveModel;  /* background values of port selected by user */

    /* FIX: can be used uninitialized */
    unsigned int *stbdCurveModel;  /* background values of stbd selected by user */
    /*unsigned int portTemp;*/
    /*unsigned int stbdTemp;*/

    double portDiff;
    double stbdDiff;

    union {
    unsigned char * tempCharBuffer;
    unsigned short *tempBuffer;
    } curveModel;

    int tempPixel;

    char *string_ptr;

    Arg args[40];
    Cardinal n;

    char pingmessage[255];
    char statusmessage[300];
    char warningmessage[300];
    char fileLabelName[255];    /* Name of current file for file name */
                                /* label in setup.c                   */

    int firstMessage;          /*  Used to check that a pop-up happens once */
    int imageSize;
    int sonarDataSize; 
    int totalDataSize;
    int scanSize;
    int halfScan;
    int bytesPerPixel;
    /*int bitsPerPixel;*/
    int pixelsPerChannel;
    int pixelRange;
    int addPixels;
    int subtractPixels;
    int pixelsToCut;
    int numChannels;
    int numSonarChannels;
    int inbytes;            /* # of bytes read from infile */
    int tape;
    int dsp_data;

    /* FIX: can be used uninitialized */
    int eventCheck;         /* checks for events to process in the loop */
    int cancelCheck;        /* checks for cancel event */
    off_t position;           /* file pointer position from lseek() */
    off_t startPosition;      /* file pointer startPosition from lseek() */

    int infd;
    int outfd;
    int result;

    int status;

    Telemetry telemetry;
    NavParams navigation;

    unsigned short stbd16Bit100KhzValue;
    unsigned short port16Bit100KhzValue;
    /*unsigned short stbd16Bit500KhzValue;*/
    /*unsigned short port16Bit500KhzValue;*/

    unsigned short stbdMinValue, stbdMaxValue;
    int stbdCount;
    float stbdValue; 
    /*float stbdAverage;*/

    unsigned char tempValue;

    char outputfile[100]/*, *infile*/;
    char *outputfile_ptr/*, *temp_ptr*/;
    char maggyfile[100];
    char originalFile[255];       /* File name of selected file          */
    char navfile[100];
    char filename[45];
    char *file_name_ptr;

    /*short *temp;*/

    /* int c, i, j, k, l, m, y, z; */        /* Counters used in the program */
    int i, j, k, l, m;         /* Counters used in the program */
    int pingNum;           /* ping number, used to count EGGMIDAS pings */
    int portPingsRead;           /* used to count pings for ramp curve models */
    int stbdPingsRead;           /* used to count pings for ramp curve models */
    int firstTrig;           /* place holder for EGG Midas trigger count */
    int sampleCounter;       /* Used to place pixels in proper position */ 
    int trueSampleCount;     /* Used to keep track of number of pixels */ 
                             /* between triggers in MIDAS data sets */ 
    int frameCounter;        /* Counts the EGG Midas frame sets */
    int lastsec;                  /* previous second for c_and_out()  */
    int c_count;           /* counter used in c_and_out() */
    int navFileUpdate;
    int new_flag;             /* set if QMIPS is version 6.35 or greater */

    /*float delta_seconds;*/          /* Time between current and last ping */
    /*float miss_rec;*/               /* Number of missing records */
    float inputResolution;

    int startPortRamp;
    int startStbdRamp;

    int n_rec;                    /* running count of records read */

    /* FIX: can be used uninitialized */
    int decm;
    /*int return_val;*/

    /* FIX: can be used uninitialized */
    unsigned char *header;        /* Place to store QMIPS header  */

    int charsCopied;          /* number of chars copied in string functions */

    FILE *rampModel;
    char rampModelFile[255];

    /*
     *    If demux flag was not set in setup, return.
     */
    
    if(!sonar->demuxFlag)
        return;

    lastsec = 0;
    dsp_data = 0;
    c_count = navFileUpdate = new_flag = 0;
    result = 1;

    stbdMinValue = 65335;
    stbdMaxValue = 0;
    stbdCount = 0;
    stbdValue = 0;

    firstMessage = 1;
    pingNum = 0;
    firstTrig = 0;
    sampleCounter = 0;
    trueSampleCount = 0;

    /*
     *   Start creating widgets for this app.
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNx, 400); n++;
    XtSetArg(args[n], XmNy, 400); n++;
    demuxDialog = XtCreatePopupShell("DemuxDialog",
                  transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                     demuxDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Demultiplexing Data",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNwidth, 500); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    statusMessage = XtCreateWidget("StatusMessage",
                        xmLabelWidgetClass, statusWindow, args, n);
    XtManageChild(statusMessage);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, statusMessage); n++;
    separator1 = XtCreateWidget("Separator1", xmSeparatorWidgetClass,
                                  statusWindow, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Processing Ping # 0",
         XmSTRING_DEFAULT_CHARSET)); n++;
    displayPings = XtCreateWidget("DisplayPings",
                   xmLabelWidgetClass, statusWindow, args, n);
    XtManageChild(displayPings);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, displayPings); n++;
    separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                                  statusWindow, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    buttonHolder = XtCreateWidget("ButtonHolder", xmRowColumnWidgetClass,
                        statusWindow, args, n);
    XtManageChild(buttonHolder);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Cancel", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton", xmPushButtonWidgetClass,
                        buttonHolder, args, n);
    XtManageChild(cancelButton);
                        
    cancelCheck = False;

    XtAddCallback(cancelButton, XmNactivateCallback, 
                                      cancelCallback, NULL);
    XtVaSetValues(cancelButton, XmNuserData, cancelCheck, NULL);

    XtRealizeWidget(demuxDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(demuxDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(demuxDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(demuxDialog, args, n);

    /*
     *   And pop the dialog up....
     */

    XtPopup(demuxDialog, XtGrabNone);

    XtSetSensitive(w, False);

    inbuf = NULL;
    outbuf = NULL;
    outdata = NULL;


    /*
     *   Open input and output files.
     */

    if(sonar->infile == NULL)
         {
         sprintf(warningmessage, "Select QMIPS file in setup");
         messages(sonar, warningmessage);
         close_demux(sonar, demuxDialog, NULL, NULL, NULL);
         return;
         }

    if((infd = open(sonar->infile, O_RDONLY)) == -1)
         {
         sprintf(warningmessage, "Error opening input file");
         messages(sonar, warningmessage);
         close_demux(sonar, demuxDialog, NULL, NULL, NULL);
         return;
         }

   /*
    *    test for errors in the parameters, or non-selected buttons.
    */

    if(sonar->fileType != QMIPS && sonar->fileType != EGG && sonar->fileType != SEGY && sonar->fileType != EGGMIDAS)
        {
        sprintf(warningmessage, "Select a raw file type in setup");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }

    if(sonar->swath == (int) NULL || sonar->swath == 0)
        {
        sprintf(warningmessage, "Select swath width in setup");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }

    if((sonar->meterRange * 2) > sonar->swath)
        {
        sprintf(warningmessage, "Range selected is greater than\noriginal range.  Please reselect range.");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }

    if(!sonar->latlon_flag && !sonar->eastnorth_flag)
        {
        sprintf(warningmessage, "Select QMIPS nav type in setup");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }

    if(!sonar->navInterval)
        {
        sprintf(warningmessage, "Select navigation output interval in setup");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }

    if(!sonar->eightBit && !sonar->sixteenBit)
        {
        sprintf(warningmessage, "Select the input bit type in setup");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        return;
        }


    if(!sonar->along_track && !sonar->across_track)
        {
        sonar->along_track = sonar->across_track = 1;
        sprintf(warningmessage,
            "using value 1 for along and across track filtering interval");
        messages(sonar, warningmessage);
        }

    /*
     *    Determine if file is from tape or disk.
     */

    if(strncmp("/dev", sonar->infile, 4) == 0)
        tape = 1;
    else
        tape = 0;
    

    /*
     *    Read the file header (if any)
     */


    if(sonar->fileType == QMIPS)
        {

        header = (unsigned char *) XtCalloc(FILEHEADERSIZE, 
                                                 sizeof(unsigned char));
        if(header == NULL)
            exit(-1);

        status = readQmipsHeader(sonar, header, infd, &sonarDataSize, &halfScan,
          &pixelsPerChannel, &scanSize, &totalDataSize, &new_flag, &dsp_data,
          demuxDialog, &numChannels, &bytesPerPixel, &numSonarChannels);

        if(!status)
            {
            close(infd);
            XtFree((char *)header);
            close_demux(sonar, demuxDialog, NULL, NULL, NULL);
            return;
            }
        }

    if(sonar->fileType == EGG)
        {
        header = (unsigned char *) XtCalloc(32, sizeof(unsigned char));
        scanSize = 1800;
        sonarDataSize = 1768;
        halfScan = pixelsPerChannel = sonarDataSize / 2;
        bytesPerPixel = EIGHT_BITS;
        numChannels = 2;
        numSonarChannels = 2;
        totalDataSize = sonarDataSize;
        }

    if(sonar->fileType == EGGMIDAS)
        {
        header = (unsigned char *) XtCalloc(864, sizeof(unsigned char));
        scanSize = 36864;         /* in bytes */

        /* sonarDataSize is  port + stbd actual pixels per frame */

        /*
        getMidasSizes(&sonarDataSize, infd);
         */

        sonarDataSize = (int) ((float) sonar->swath * 35.82); 
                                              /* 35.82 = samps/meter */
        if(!(sonarDataSize % 2))
            sonarDataSize += 1;

        halfScan = pixelsPerChannel = sonarDataSize / 2;
        bytesPerPixel = SIXTEEN_BITS;
        numChannels = 4;
        numSonarChannels = 4;
        totalDataSize = sonarDataSize * 2;

        }

    if(sonar->fileType == SEGY)
        {
        header = (unsigned char *) XtCalloc(EBCHDLEN, sizeof(unsigned char));

        status = readSegyHeader(sonar, header, infd, &sonarDataSize, &halfScan,
          &pixelsPerChannel, &scanSize, &totalDataSize, demuxDialog, 
          &numChannels, &bytesPerPixel, &numSonarChannels);

        if(!status)
            {
            close(infd);
            XtFree((char *)header);
            close_demux(sonar, demuxDialog, NULL, NULL, NULL);
            return;
            }

        }



    /*
     *    Get the input and output resolution in meters/pixel.
     *    Check to see if a specific range was selected and set
     *    the cutoff point and imagesize appropriately.
     */

    inputResolution = (float) sonar->swath / (float) sonarDataSize;

    if(sonar->meterRange)
        {
        pixelRange = (int) (((float) sonar->meterRange 
                                          / inputResolution) + 0.5);
        addPixels = subtractPixels = pixelRange;

        while(addPixels % 4)
            addPixels++;
        while(subtractPixels % 4)
            subtractPixels--;

        if(addPixels > subtractPixels)
            pixelRange = subtractPixels;
        else
            pixelRange = addPixels;

        pixelsToCut = sonarDataSize - pixelRange * 2;
        sonar->swath = (int) ((float)pixelRange * inputResolution * 2.0 + 0.5);
        }
    else
        {
        pixelRange = pixelsPerChannel;
        pixelsToCut = sonarDataSize - pixelRange * 2;
        }

    imageSize = (sonarDataSize - pixelsToCut) / sonar->across_track;


/*
fprintf(stderr, "header[0] = %d\n", header[0]);
fprintf(stderr,"pixels  per channel = %d\n", pixelsPerChannel);
fprintf(stderr,"bytesPerPixel  = %d\n", bytesPerPixel);
fprintf(stderr,"num  channels = %d\n", numChannels);
fprintf(stderr,"number  of sonar channels = %d\n", numSonarChannels);
fprintf(stderr,"bits  per pixel = %d\n", get_short(header, 32, False, sonar));
fprintf(stderr,"async  chan # = %d\n", get_short(header, 44, False, sonar));
fprintf(stdout,"range to demux = %d\n", sonar->meterRange);
fprintf(stdout,"pixelRange  = %d\n", pixelRange);
fprintf(stdout,"pixels  to cut = %d\n", pixelsToCut);
fprintf(stdout,"inputResolution  = %f\n", inputResolution);
fprintf(stdout,"swath  = %d\n", sonar->swath);
*/

    /*
     *    If the input file type is QMIPS, get the filename from the
     *    header.  If the type is EGG, just use the input file name
     *    from the file selection dialog.
     */


    if(sonar->fileType == QMIPS)
        {
        /*
         *   Get filename from infile or tape
         */

        header += 484;
        charsCopied = sprintf(filename, "%.45s", header);
        header -= 484;

        outputfile_ptr = outputfile;

        if(strlen(filename) == 0)
            {
            file_name_ptr = strrchr(sonar->infile, '/');
            file_name_ptr++;
            }
        else if((file_name_ptr = strrchr(filename, '\\')) == NULL &&
            (file_name_ptr = strrchr(filename, ':')) == NULL)
            {
            file_name_ptr = filename;
            }
        else
            file_name_ptr++;

        i = 0;
        while(decm != 46)
            {
            decm = *file_name_ptr;
            if(decm > 64)
                decm = *outputfile_ptr++ = tolower(*file_name_ptr++);
            else
                decm = *outputfile_ptr++ = *file_name_ptr++;
            }

        *outputfile_ptr = '\0';
        }

    /*
     *  Now reset the the infile member of the sonar 
     *  structure to represent the current directory
     *  where the QMIPS file, and reset the name to
     *  that of the filename from tape.  Save the original filename
     *  in case the demux operation is cancelled.
     */

    sprintf(originalFile, "%s", sonar->infile);
    sprintf(sonar->currentDirectory, "%s", originalFile);
    string_ptr = strrchr(sonar->currentDirectory, '/');
    string_ptr++;
    *string_ptr = '\0';

    if(sonar->fileType == QMIPS)
        {
        outputfile_ptr = outputfile;
        string_ptr = strrchr(sonar->infile, '/');
        string_ptr++;
        while(*outputfile_ptr != '\0')
            *string_ptr++ = *outputfile_ptr++;
        *string_ptr = '\0';
        }
    else         /*   Is an EGG file or SEGY file, use input file name  */
        {
        string_ptr = strrchr(sonar->infile, '.');
        string_ptr++;
        *string_ptr = '\0';
        sprintf(outputfile, "%s", sonar->infile);
        }


    /*
     *  Open the output file.
     */


    if((outfd = open(outputfile, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningmessage, "Error opening output file");
        messages(sonar, warningmessage);
        close(infd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        XtFree((char *)header);
        return;
        }
    else
        {
        sprintf(statusmessage, "Demultiplexing file: %s.dat", sonar->infile);
        n = 0;
        XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                             XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(statusMessage, args, n);
        XmUpdateDisplay(demuxDialog);
        }

    /*
     *     Open the ASCII navigation file.
     */

    sprintf(navfile, "%snav", outputfile);
    sonar->navfile = navfile;
    if((sonar->navfp = fopen(navfile, "w")) == NULL)
        {
        sprintf(warningmessage, "Error opening nav file");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        close_demux(sonar, demuxDialog, NULL, NULL, NULL);
        XtFree((char *)header);
        return;
        }

    /*    Save magnetometer readings to an ASCII file
     *    containing decimal julian day, lat, lon, mag x, mag y, and mag z
     *    if magnetometer_flag is set.
     */

    if(sonar->magnetometer_flag)
        {
        sprintf(maggyfile, "%smag", sonar->infile);
        if((sonar->magfp = fopen(maggyfile, "w")) == NULL)
            {
            sprintf(warningmessage, "Error opening magnetometer file");
            messages(sonar, warningmessage);
            close(infd);
            close(outfd);
            close_demux(sonar, demuxDialog, NULL, NULL, NULL);
            XtFree((char *)header);
            return;
            }
        }

    /*
     *    skip first_rec records, read n_rec records and output compacted data
     */

    position = lseek(infd, (off_t) (sonar->skipScans * scanSize), SEEK_CUR);
    startPosition = position;  /* In case we do the ramp removal */

    sprintf(statusmessage, "Starting demux at scan: %d\n", (int)(position / scanSize)); 
    strcat(statusmessage, "Output file name is: ");
    strcat(statusmessage, outputfile);
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetValues(statusMessage, args, n);


/*
    Write file header


    if ((write (outfd, &demuxed_file_header, HEADSIZE)) != HEADSIZE)
        {
        fprintf (stdout, "error writing file header.\n");
        err_exit();
        }
*/

    /*
     *    Allocate memory for the median buffer.
     */


    c_temp = (unsigned char *) XtCalloc((sonarDataSize - pixelsToCut) *
                    sonar->along_track, sizeof(unsigned char));

    if(c_temp == NULL)
         {
         sprintf(warningmessage, "Not enough memory for sonar median buffer");
         messages(sonar, warningmessage);
         close(infd);
         close(outfd);
         close_demux(sonar, demuxDialog, NULL, NULL, NULL);
         XtFree((char *)header);
         return;
         }


    /*
     *   Allocate memory for input and output ping.
     */


    outbuf = (unsigned char *) XtCalloc(sonarDataSize - pixelsToCut,
                                    sizeof(unsigned char));

    if(outbuf == NULL)
         {
         sprintf(warningmessage, "Not enough memory for sonar filter buffer");
         messages(sonar, warningmessage);
         close(infd);
         close(outfd);
         close_demux(sonar, demuxDialog, NULL, NULL, NULL);
         XtFree((char *)header);
         return;
         }

    outdata = (unsigned char *) XtCalloc(imageSize, sizeof(unsigned char));
    if(outdata == NULL)
         {
         sprintf(warningmessage, "Not enough memory for output sonar array");
         messages(sonar, warningmessage);
         close(infd);
         close(outfd);
         close_demux(sonar, demuxDialog, NULL, outbuf, NULL);
         XtFree((char *)header);
         XtFree((char *)outbuf);
         return;
         }

    inbuf = (unsigned char *) XtCalloc(scanSize, sizeof(unsigned char));
    if(inbuf == NULL)
         {
         sprintf(warningmessage, "Not enough memory for input sonar array");
         messages(sonar, warningmessage);
         close(infd);
         close(outfd);
         close_demux(sonar, demuxDialog, NULL, outbuf, outdata);
         XtFree((char *)header);
         XtFree((char *)outdata);
         XtFree((char *)outbuf);
         return;
         }

    telemetry.raw_depth = (float *)XtCalloc(sonar->along_track, sizeof(float));
    telemetry.raw_pitch = (float *)XtCalloc(sonar->along_track, sizeof(float));
    telemetry.raw_roll = (float *)XtCalloc(sonar->along_track, sizeof(float));
    telemetry.raw_course = (float *)XtCalloc(sonar->along_track,sizeof(float));
    telemetry.raw_ap_alt = (float *)XtCalloc(sonar->along_track, sizeof(float));
    telemetry.raw_fish_alt = 
                         (float *)XtCalloc(sonar->along_track, sizeof(float));

    /*
     *  If the user wants to remove a background model based on a range of
     *  pings that were selected by the user in showimage, calculate the 
     *  average here before the main loop.
     */

    if((sonar->removeRampPortFlag || sonar->removeRampStbdFlag) 
        && sonar->sixteenBit)
        {

        /*
	 *  Initialize the port & stbd model pointers
	 */

        portCurveModel = (unsigned int *) 
                          XtCalloc(pixelsPerChannel, sizeof(unsigned int));

        stbdCurveModel = (unsigned int *) 
                          XtCalloc(pixelsPerChannel, sizeof(unsigned int));

        /*
         * Open the ASCII ramp model file and truncate if it exists.
         * First skip to first port scan for curve model and acquire buffer.
	 * unless the user wants to input a custom ramp file.
         */

        if(!sonar->getCurveModelText)
            {
	      /*sprintf(rampModelFile, "%sRampModel.txt\0", outputfile); FIX: \0??? */
            sprintf(rampModelFile, "%sRampModel.txt", outputfile);
            if((rampModel = fopen(rampModelFile, "w+")) == NULL)
                {
                sprintf(warningmessage, "Error opening port ramp model text file");
                messages(sonar, warningmessage);
                close(infd);
                close(outfd);
                close_demux(sonar, demuxDialog, NULL, outbuf, outdata);
                XtFree((char *)header);
                XtFree((char *)outdata);
                XtFree((char *)outbuf);
                return;
                }

            curveModel.tempBuffer = (unsigned short *) 
                          XtCalloc(pixelsPerChannel * numChannels, 
                          sizeof(unsigned short));

            curveModel.tempCharBuffer = (unsigned char *) 
                          XtCalloc(pixelsPerChannel * bytesPerPixel * numChannels, 
                          sizeof(unsigned char));

            if(inbuf == NULL)
                 {
                 sprintf(warningmessage, 
                     "Not enough memory for input sonar array");
                 messages(sonar, warningmessage);
                 close(infd);
                 close(outfd);
                 close_demux(sonar, demuxDialog, NULL, outbuf, outdata);
                 XtFree((char *)header);
                 XtFree((char *)outdata);
                 XtFree((char *)outbuf);
                 return;
                 }


            portPingsRead = stbdPingsRead = 0;

            position = lseek(infd, (off_t) startPosition, SEEK_SET);

            if(sonar->removeRampPortFlag) 
                {
                position = lseek(infd, (off_t) (sonar->startScanForRemoveRampPort
                                                           * scanSize), SEEK_CUR);
                sprintf(statusmessage, 
                    "Getting data for port model starting at scan: %d\n", 
                    (int)(startPosition / scanSize)); 
                n = 0;
                XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                                 XmSTRING_DEFAULT_CHARSET)); n++;
                XtSetValues(statusMessage, args, n);

                for(i = sonar->startScanForRemoveRampPort;
                    i < sonar->endScanForRemoveRampPort; i++)
                    {
                    portPingsRead++;
                    if((inbytes = read(infd, curveModel.tempCharBuffer, 
                        totalDataSize)) == totalDataSize)
                        {
                        for(j = 0, k = pixelsPerChannel - 1; 
                            j < pixelsPerChannel; j++, k--)
                            {
                            portCurveModel[j] += curveModel.tempBuffer[k];
                            }
                        }
                    else
                        fprintf(stderr, "error reading port values for ramp\n");
                    position = lseek(infd, 
                            (off_t) scanSize - totalDataSize, SEEK_CUR);
                    }
                }

            position = lseek(infd, (off_t) startPosition, SEEK_SET);

            if(sonar->removeRampStbdFlag) 
                {
                position = lseek(infd, (off_t) (sonar->startScanForRemoveRampStbd
                                                           * scanSize), SEEK_CUR);
                sprintf(statusmessage, 
                    "Getting data for stbd model starting at scan: %d\n", 
                    (int)(startPosition / scanSize)); 
                n = 0;
                XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                                 XmSTRING_DEFAULT_CHARSET)); n++;
                XtSetValues(statusMessage, args, n);

                for(i = sonar->startScanForRemoveRampStbd;
                    i < sonar->endScanForRemoveRampStbd; i++)
                    {
                    stbdPingsRead++;
                    if((inbytes = read(infd, curveModel.tempCharBuffer, 
                        totalDataSize)) == totalDataSize)
                        {
                        for(j = 0; j < pixelsPerChannel; j++)
                            {
                            stbdCurveModel[j] += 
                                    curveModel.tempBuffer[j + pixelsPerChannel];
                            }
                        }
                    else
                        fprintf(stderr, "error reading stbd values for ramp\n");
                    position = lseek(infd, 
                            (off_t) scanSize - totalDataSize, SEEK_CUR);
                    }
                }

            for(j = 0; j < pixelsPerChannel; j++)
                {
                if(sonar->removeRampPortFlag) 
                    portCurveModel[j] /= portPingsRead;
                if(sonar->removeRampStbdFlag) 
                    stbdCurveModel[j] /= stbdPingsRead;
                fprintf(rampModel, "%d\t%d\t%d\n", j, 
                          portCurveModel[j], stbdCurveModel[j]);
                }
            }
        else  /* read in a custom ramp model  */
            {
            if((rampModel = fopen(sonar->tvgRampTextFileName, "r+")) == NULL)
                {
                sprintf(warningmessage, "Error opening port ramp model text file");
                messages(sonar, warningmessage);
                close(infd);
                close(outfd);
                close_demux(sonar, demuxDialog, NULL, outbuf, outdata);
                XtFree((char *)header);
                XtFree((char *)outdata);
                XtFree((char *)outbuf);
                return;
                }

            for(j = 0; j < pixelsPerChannel; j++)
                {
                fscanf(rampModel, "%d %u %u", &i, 
                                 &portCurveModel[j], &stbdCurveModel[j]);

                }
            }


        fclose(rampModel);
        position = lseek(infd, (off_t) startPosition, SEEK_SET);

        }


     fprintf(stderr, "start position is %d, cur pos = %d\n", (int)startPosition, (int)position);

    /*
     *    Set inbytes to the proper amount before entering main loop.
     */

    inbytes = scanSize;

    /*
     *    Main loop
     */

    for (i = 0; inbytes == scanSize; i++)
        {

        while(eventCheck == 1)
            {
            if(XtAppPending(sonar->toplevel_app))
                {
                XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
                XmUpdateDisplay(sonar->toplevel);
                }
            else
                eventCheck = 0;
            }

        eventCheck = 1;

        /*
         *    test if everything is fine with the input buffer
         *    and input the sonar data.
         */

         if(!(i % 100))
             { 
             sprintf(pingmessage, "Processing Ping # %d", i + sonar->skipScans);

             n = 0;
             XtSetArg(args[n], XmNlabelString,
                           XmStringCreateLtoR(pingmessage,
                           XmSTRING_DEFAULT_CHARSET)); n++;
             XtSetValues(displayPings, args, n);
             XmUpdateDisplay(demuxDialog);
             }


         if((inbytes = read(infd, inbuf, scanSize)) == scanSize)
            {

            if(sonar->fileType == EGGMIDAS)
                {
                for(frameCounter = 0; frameCounter < 1000; frameCounter++)
                    {
                    trueSampleCount += 3; /*  Keep track of actual number of
                                              pixels before next trigger */

                    /*
                     *   Check to make sure that sample count does not
                     *   exceed outbuf size.  If MIDAS operator increased
                     *   swath width in a file, the outer part of the swath
                     *   will get chopped off based on input swath width
                     *   in setup.
                     */

                    if(firstTrig && sampleCounter * 2 
                        < sonarDataSize - pixelsToCut)
                        {
                        port16Bit100KhzValue = 
                             get_short(inbuf, 0+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((port16Bit100KhzValue >> sonar->portBitShift) > 255)
                                outbuf[sampleCounter++ + halfScan] = 255;
                            else
                                outbuf[sampleCounter++ + halfScan] = (unsigned char)
                                (port16Bit100KhzValue >> sonar->portBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(port16Bit100KhzValue > 
                               sonar->portMaxNormalizeValue)
                               { 
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
                               } 
			    else
                               { 
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[sampleCounter++ + halfScan] = 
                                    (unsigned char) tempPixel;
                            }
                                                        

                        stbd16Bit100KhzValue = 
                             get_short(inbuf, 2+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((stbd16Bit100KhzValue >> sonar->stbdBitShift) > 255)
                                outbuf[halfScan - sampleCounter] = 255;
                            else
                                outbuf[halfScan - sampleCounter] = (unsigned char)
                                (stbd16Bit100KhzValue >> sonar->stbdBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(stbd16Bit100KhzValue > 
                               sonar->stbdMaxNormalizeValue)
                               tempPixel = (int)((float)stbd16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
			    else
                               { 
                               tempPixel = (int) ((float)stbd16Bit100KhzValue 
                                      / (float) sonar->stbdNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[halfScan - sampleCounter] 
                                        = (unsigned char) tempPixel;
                            }

                        port16Bit100KhzValue = 
                             get_short(inbuf, 12+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((port16Bit100KhzValue >> sonar->portBitShift) > 255)
                                outbuf[sampleCounter++ + halfScan] = 255;
                            else
                                outbuf[sampleCounter++ + halfScan] = (unsigned char)
                                (port16Bit100KhzValue >> sonar->portBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(port16Bit100KhzValue > 
                               sonar->portMaxNormalizeValue)
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
			    else
                               { 
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[sampleCounter++ + halfScan] = 
                                    (unsigned char) tempPixel;
                            }

                        stbd16Bit100KhzValue = 
                             get_short(inbuf, 14+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((stbd16Bit100KhzValue >> sonar->stbdBitShift) > 255)
                                outbuf[halfScan - sampleCounter] = 255;
                            else
                                outbuf[halfScan - sampleCounter] = (unsigned char)
                                (stbd16Bit100KhzValue >> sonar->stbdBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(stbd16Bit100KhzValue > 
                               sonar->stbdMaxNormalizeValue)
                               tempPixel = (int)((float)stbd16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
			    else
                               { 
                               tempPixel = (int) ((float)stbd16Bit100KhzValue 
                                      / (float) sonar->stbdNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[halfScan - sampleCounter] 
                                        = (unsigned char) tempPixel;
                            }

                        port16Bit100KhzValue = 
                             get_short(inbuf, 24+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((port16Bit100KhzValue >> sonar->portBitShift) > 255)
                                outbuf[sampleCounter++ + halfScan] = 255;
                            else
                                outbuf[sampleCounter++ + halfScan] = (unsigned char)
                                (port16Bit100KhzValue >> sonar->portBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(port16Bit100KhzValue > 
                               sonar->portMaxNormalizeValue)
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
			    else
                               { 
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[sampleCounter++ + halfScan] = 
                                    (unsigned char) tempPixel;
                            }

                        stbd16Bit100KhzValue = 
                             get_short(inbuf, 26+frameCounter*36, False, sonar);
                        if(sonar->bitShift)
                            {
                            if((stbd16Bit100KhzValue >> sonar->stbdBitShift) > 255)
                                outbuf[halfScan - sampleCounter] = 255;
                            else
                                outbuf[halfScan - sampleCounter] = (unsigned char)
                                (stbd16Bit100KhzValue >> sonar->stbdBitShift);
                            }
                        else  /* normalize the 16 bit value to 8bit */
                            {
                            if(stbd16Bit100KhzValue > 
                               sonar->stbdMaxNormalizeValue)
                               tempPixel = (int)((float)stbd16Bit100KhzValue 
                                      / (float) 65535 * 255.0); 
			    else
                               { 
                               tempPixel = (int) ((float)stbd16Bit100KhzValue 
                                      / (float) sonar->stbdNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

                            outbuf[halfScan - sampleCounter] 
                                        = (unsigned char) tempPixel;
                            }

                        if(stbd16Bit100KhzValue < stbdMinValue && 
                                                          sampleCounter > 200)
                            stbdMinValue = stbd16Bit100KhzValue;
                        if(stbd16Bit100KhzValue > stbdMaxValue && 
                                                          sampleCounter > 200)
                            stbdMaxValue = stbd16Bit100KhzValue;

                        stbdValue += (float) stbd16Bit100KhzValue;
                        stbdCount++;

                        }


                    if(get_short(inbuf, 8+frameCounter*36, False, sonar) >= 247)
                        {

/*
fprintf(stdout, "true samps = %d, sonar size = %d, ping num = %d\n", trueSampleCount, sonarDataSize, pingNum);
*/

                        if(trueSampleCount * 2 - (sonarDataSize - pixelsToCut)
                            > 50 && firstMessage && pingNum > 10)
                            {
                            sprintf(warningmessage, 
                               "Number of samples read at ping %d indicate\n", 
                                   pingNum);
                            strcat(warningmessage,
                               "an increase in swath width. Will continue\n");
                            strcat(warningmessage,
                               "to demux and ignore outer part of larger\n");
                            strcat(warningmessage,
                               "swath.  Hit cancel and re-enter swath width\n");
                            strcat(warningmessage,
                               "to preserve this data.");
                            messages(sonar, warningmessage);
                            firstMessage = 0;
                            }

                        if(trueSampleCount * 2 - (sonarDataSize - pixelsToCut)
                            < -50 && firstMessage && pingNum > 10)
                            {
                            sprintf(warningmessage, 
                               "Number of samples read at ping %d indicate\n",
                                   pingNum);
                            strcat(warningmessage,
                               "a smaller swath width for this file.\n");
                            strcat(warningmessage,
                               "Will continue to demux, the outer part of\n");
                            strcat(warningmessage,
                               "the swath will be blank from here on in.\n");
                            strcat(warningmessage,
                               "Hit cancel and re-enter swath width\n");
                            strcat(warningmessage,
                               "if most of the file is at a shorter swath.");
                            messages(sonar, warningmessage);
                            firstMessage = 0;
                            }

                        if(firstTrig)
                            {
                            /*    send output buffer to c_and_out(), where data
                             *    will be compacted and output
                             */

                            c_and_out(sonar, &c_count, outdata, outbuf, 
                                &telemetry, imageSize, &navigation, inbuf,
                                &lastsec, &navFileUpdate, demuxDialog, c_temp, 
                                dsp_data, outfd, &result, totalDataSize);


                            }


                        for(j = 0; j < sonarDataSize - pixelsToCut; j++)
                            outbuf[j] = 0;

                        sampleCounter = 0;
                        trueSampleCount = 0;

                        if(pingNum == 0)
                           firstTrig++;

                        pingNum++;
                        }

                    }
                }
            else
                {
                for(j = 0, k = (pixelRange - 1), l = halfScan - 1, m = 32; 
                    j < pixelRange; j++, k--, l--, m += 2)
                    {    
                    if(sonar->eightBit)
                        {
                        if(sonar->fileType == QMIPS)
                            {
                            /* stbd */
                            tempValue = inbuf[halfScan + j + sonar->stbdOffset];
                            /* if(tempValue > 255) tempValue = 255; FIX: was always false */
                            outbuf[k] = (unsigned char) tempValue;

                            /* port */
                            tempValue = inbuf[l + sonar->portOffset];
                            /* if(tempValue > 255) tempValue = 255; FIX: WAS ALWAYS FALSE */
                            outbuf[j+pixelRange] = (unsigned char) tempValue;
                            }
                        else     /*  EGG data   */
                            {
                            /* stbd */
                            tempValue = inbuf[m + 1] * 255 / 63;
                            /* if(tempValue > 255) tempValue = 255; FIX: WAS ALWAYS FALSE */
                            outbuf[k] = tempValue;

                            /* port */
                            tempValue = inbuf[m] * 255 / 63;
                            /* if(tempValue > 255) tempValue = 255; FIX: WAS ALWAYS FALSE */
                            outbuf[j+pixelRange] = tempValue; 
                            }
                        

                        if(outbuf[k] < stbdMinValue && k > 200)
                            stbdMinValue = outbuf[k];
                        /*if(outbuf[k] < 32000 && k > 200)*/  /* FIX: Always true?!?! */
                            stbdMaxValue = outbuf[k];

                        stbdValue += (float) outbuf[k];
                        stbdCount++;
                        }

                    if(sonar->sixteenBit)
                        {

                        if(sonar->fileType == QMIPS)
                            {
                            stbd16Bit100KhzValue = (unsigned short) get_short(inbuf, 
                                (halfScan + j + sonar->stbdOffset) * 2, 
                                False, sonar);
                            port16Bit100KhzValue = (unsigned short) get_short(inbuf, 
                                       (l + sonar->portOffset) * 2,
                                       False, sonar);
                            }


                        if(stbd16Bit100KhzValue < stbdMinValue && j > 200)
                            stbdMinValue = stbd16Bit100KhzValue;
                        if(j > 200 && stbd16Bit100KhzValue > stbdMaxValue && stbd16Bit100KhzValue < 50000)
                            stbdMaxValue = stbd16Bit100KhzValue;

                        stbdValue += (float) stbd16Bit100KhzValue;
                        stbdCount++;

                        /*
                         *  Adjust for ramping affects towards the
                         *  far range if signaled by user. 
                         *
                         */
                        startPortRamp = (int) ((float) 
                               sonar->removeRampRangeStartPort / inputResolution);
                        if(sonar->removeRampPortFlag && j > startPortRamp)
                            {
                            portDiff = (double) portCurveModel[j] - 
                                          (double) sonar->removeRampPortPivot;
                            tempDoubleValue =  (double) port16Bit100KhzValue 
                                                                - portDiff;
                            if(tempDoubleValue < 0)
                                tempDoubleValue = 0;
                            if(tempDoubleValue > 65335)
                                tempDoubleValue = 65335;

                            port16Bit100KhzValue = (unsigned short) tempDoubleValue;
                            }
/*
printf("BEFORE: pixel = %d, portDiff = %.0f, curve model = %d, port val = %d\n", j, portDiff, portCurveModel[j], port16Bit100KhzValue);
                            port16Bit100KhzValue = (unsigned short) tempDoubleValue;
printf("AFTER : pixel = %d, portDiff = %.0f, curve model = %d, port val = %d\n", j, portDiff, portCurveModel[j], port16Bit100KhzValue);
*/                           

                        startStbdRamp = (int) ((float) 
                               sonar->removeRampRangeStartStbd / inputResolution);
                        if(sonar->removeRampStbdFlag && j > startStbdRamp)
                            {
                            stbdDiff = (double) stbdCurveModel[j] - 
                                          (double) sonar->removeRampStbdPivot;
                            tempDoubleValue =  (double) stbd16Bit100KhzValue 
                                                                - stbdDiff;
                            if(tempDoubleValue < 0)
                                tempDoubleValue = 0;
                            if(tempDoubleValue > 65335)
                                tempDoubleValue = 65335;

                            stbd16Bit100KhzValue = (unsigned short) tempDoubleValue;
                            }

/*
printf("BEFORE: portDiff = %.0f, curve model = %d, port val = %d\n", portDiff, portCurveModel[k], port16Bit100KhzValue);
printf("BEFORE: portDiff = %.0f, curve model = %d, port val = %d\n", portDiff, portCurveModel[k], port16Bit100KhzValue);
*/


                        /*
                         *  Half-wave rectify.  Shouldn't be necessary....
                         *  Added this to accomodate Klein 595 aquisition
                         *  with ISIS.  Seems to record the full wave ... ?
                         *


                        if(stbd16Bit100KhzValue < 0)
                            stbd16Bit100KhzValue = 0;

                        if(port16Bit100KhzValue < 0)
                            port16Bit100KhzValue = 0;

                         */

                        /*
                         *   Use a mask here to turn off the upper bit
                         *   when using Edgetech data.  The upper bit is
                         *   on by default.

                        port16Bit100KhzValue = port16Bit100KhzValue & 32767;
                        stbd16Bit100KhzValue = stbd16Bit100KhzValue & 32767;
                         */


                        if(sonar->fileType == SEGY)
                            {
                            stbd16Bit100KhzValue = get_short(inbuf, 
                                 (j + halfScan + TRHDLEN) * 2 + 
                                 sonar->stbdOffset, False, sonar);
                            port16Bit100KhzValue = get_short(inbuf, 
                                  j * 2 + TRHDLEN + sonar->portOffset, 
                                  False, sonar);
                            }

                        if(sonar->normalize)
                            {
                            if(stbd16Bit100KhzValue > 
                               sonar->stbdMaxNormalizeValue)
                               tempPixel = 255;
			    else if(stbd16Bit100KhzValue > 
                                    sonar->stbdNormalizeValue)
                                { 
                                tempPixel = (int) 
                                  ((log10((double)stbd16Bit100KhzValue)  - 
                                    log10((double) sonar->stbdNormalizeValue))
                                 / (log10((double) sonar->stbdMaxNormalizeValue)
                                  - log10((double)sonar->stbdNormalizeValue)) 
                                    * 128.0) + 127; 
			        if(tempPixel > 255)
                                    tempPixel = 255;
                                } 
			    else if(sonar->stbdNormalizeValue == 
                                    sonar->stbdMaxNormalizeValue)
                                { 
                                tempPixel = (int) ((float)stbd16Bit100KhzValue 
                                      / (float) sonar->stbdNormalizeValue 
                                      * 255.0); 
			        if(tempPixel > 255)
                                    tempPixel = 255;
                                } 
                            else
                                tempPixel = (int) ((float)stbd16Bit100KhzValue 
                                      / (float) sonar->stbdNormalizeValue 
                                      * 128.0); 

                            outbuf[k] = (unsigned char) tempPixel;

                            if(port16Bit100KhzValue > 
                               sonar->portMaxNormalizeValue)
                               tempPixel = 255;
			    else if(port16Bit100KhzValue > 
                                    sonar->portNormalizeValue)
                                { 
                                tempPixel = (int) 
                                  ((log10((double)port16Bit100KhzValue)  - 
                                    log10((double) sonar->portNormalizeValue))
                                 / (log10((double) sonar->portMaxNormalizeValue)
                                  - log10((double)sonar->portNormalizeValue)) 
                                    * 128.0) + 127; 
			        if(tempPixel > 255)
                                    tempPixel = 255;
                                } 
			    else if(sonar->portNormalizeValue == 
                                    sonar->portMaxNormalizeValue)
                                { 
                                tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 255.0); 
			        if(tempPixel > 255)
                                    tempPixel = 255;
                                } 
                            else
                                tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 128.0); 

                            outbuf[j + pixelRange] = 
                                                  (unsigned char) tempPixel;
/*
                            if(port16Bit100KhzValue > 
                               sonar->portMaxNormalizeValue)
                               tempPixel = 255;
			    else
                               { 
                               tempPixel = (int) ((float)port16Bit100KhzValue 
                                      / (float) sonar->portNormalizeValue 
                                      * 255.0); 
			       if(tempPixel > 255)
                                   tempPixel = 255;
                               } 

*/

                            }
                        else
                            {
                            if((stbd16Bit100KhzValue >> 
                                             sonar->stbdBitShift) > 255)
                                outbuf[k] = 255;
                            else
                                outbuf[k] = (unsigned char)
                                  (stbd16Bit100KhzValue >> sonar->stbdBitShift);

                            if((port16Bit100KhzValue >> 
                                             sonar->portBitShift) > 255)
                                outbuf[j + pixelRange] = 255;
                            else
                                outbuf[j + pixelRange] = (unsigned char)
                                  (port16Bit100KhzValue >> sonar->portBitShift);
                            }



                        }

                    
                    }  /*  End of for statement unpacking sonar */

                /*    send output buffer to c_and_out(), where data
                 *    will be compacted and output
                 */

                c_and_out(sonar, &c_count, outdata, outbuf, &telemetry, 
                    imageSize, &navigation, inbuf, &lastsec, &navFileUpdate, 
                    demuxDialog, c_temp, dsp_data, outfd, 
                    &result, totalDataSize);


                }  /*  End of else statement checking EGGMIDAS  */


            }

        XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);

        if(cancelCheck == True || !result)
            inbytes = 0;

        }  /* end of for loop  */


        /*      Reached EOF or the pre-selected number of records
         *      were processed.  Write out the positional data
         *      for the very last scan to navfp.  Very important
         *      for high ping rates.
         */

        if(inbytes != scanSize || i == n_rec)
            {
            if(sonar->latlon_flag)
                {
                fprintf(sonar->navfp,
                    "%03d %02d %02d %02d %02d %8.5f %4d %8.5f\n",
                    navigation.jday, navigation.hour, navigation.min,
                    navigation.sec, navigation.latdeg, navigation.latmin,
                    navigation.londeg, navigation.lonmin);
                }

            if(sonar->eastnorth_flag)
                {
                fprintf(sonar->navfp,"%03d %02d %02d %02d %8.2f %8.2f\n",
                    navigation.jday, navigation.hour, navigation.min,
                    navigation.sec, navigation.latitude,
                    navigation.longitude);
                }

            }


    /*
     *    Set the file label on the Setup display if popped up.
     *    Reset the name of sonar->infile if the operation was cancelled.
     */

    fileLabel = XtNameToWidget(sonar->toplevel, "*SetupDialog*FileNameLabel");
    if(cancelCheck == True || !result)
        strcpy(sonar->infile, originalFile);
    else
        strcpy(sonar->infile, outputfile);
        
    if(fileLabel != NULL)
        {
        strcpy(fileLabelName, "File is: ");
        strcat(fileLabelName, sonar->infile);
        n = 0;
        XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR(fileLabelName, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(fileLabel, args, n);
        }

    close(infd);
    close(outfd);

    close_demux(sonar, demuxDialog, inbuf, outbuf, outdata);

    if(cancelCheck == True)
        {
        sprintf(warningmessage, 
               "\nDemux cancelled at your request\n");
        messages(sonar, warningmessage);
        }
    else if(!result)
        ;  /* do nothing, other warnings will have popped up */
    else 
        {
        sprintf(warningmessage, 
          "\nOutput scan size %s is %d\nOutput Resolution is %f\n", 
          outputfile, imageSize + HEADSIZE, 
          (float) sonar->swath / (float) imageSize);

        strcat(warningmessage,"Remember to check navigation before processing");
        messages(sonar, warningmessage);
        sonar->fileType = XSONAR;
        }

    XtFree((char *)header);

    fprintf(stderr, "Min stbd value = %d\n", stbdMinValue);
    fprintf(stderr, "Average stbd value = %.1f\n", stbdValue / stbdCount);
    fprintf(stderr, "Max stbd value = %d\n", stbdMaxValue);


    return;
}

/*    c_and_out()
*
*     compacts the sonar data and outputs them to outfd
*/

void c_and_out(MainSonar *sonar, int *c_count, unsigned char *outdata,
	       unsigned char *outbuf, Telemetry *telemetry, int imageSize, 
	       NavParams *navigation, unsigned char *inbuf, int *lastsec, 
	       int *navFileUpdate, 
	        UNUSED Widget parent,
	       unsigned char *c_temp, 
	       int dspData, int outfd, int *result, int totalDataSize)
{

    char warningmessage[500];

    /*float temp_azi;*/
    float outputResolution;
    /*float dspMagX, dspMagY, dspMagZ;*/

    int i, j;
    int outbytes;
    int navCheckInterval;
    int navOutputSeconds;

    /* short magx, magy, magz; */
    /*short get_short();*/

    /* int tempSwath; */

    outputResolution = (float) sonar->swath / imageSize;

    /*
     *    Load up the temporary buffer used to calculate median values
     *    for desampling the data.
     */

    for (i = 0; i < imageSize * sonar->across_track; i++)
        c_temp[(i * sonar->along_track) + *c_count] = outbuf[i];

    /* 
     *    Calculate (in this order, same for QMIPS and DSP sonar):
     *
     *    telemFishDepth
     *    telemFishHeading 
     *    telemFishPitch 
     *    telemFishRoll 
     *    telemFishAlt  
     */

    if(sonar->fileType == QMIPS)
        {
        telemetry->raw_depth[*c_count] = get_float(inbuf,totalDataSize + 62,
                                                              False, sonar);
        telemetry->raw_course[*c_count] = get_float(inbuf,totalDataSize +66,
                                                              False, sonar);
        telemetry->raw_pitch[*c_count] = get_float(inbuf,totalDataSize + 70,
                                                              False, sonar);
        telemetry->raw_roll[*c_count] = get_float(inbuf, totalDataSize + 74,
                                                              False, sonar);

        if(!dspData)
            {
            telemetry->raw_ap_alt[*c_count] = 
                           get_float(inbuf, totalDataSize + 44, False, sonar);
            telemetry->raw_fish_alt[*c_count] = 
                           get_float(inbuf, totalDataSize + 78, False, sonar);
            }
        else
            {
            telemetry->raw_ap_alt[*c_count] = 
                           get_float(inbuf, totalDataSize + 78, False, sonar);
            telemetry->raw_fish_alt[*c_count] = 
                           get_float(inbuf, totalDataSize + 82, False, sonar);
            }

        }
    else
        {
        telemetry->raw_depth[*c_count] = 0;
        telemetry->raw_course[*c_count] = 0;
        telemetry->raw_pitch[*c_count] = 0;
        telemetry->raw_roll[*c_count] = 0;
        telemetry->raw_ap_alt[*c_count] = 0;
        telemetry->raw_fish_alt[*c_count] = 0;
        }
        

        /*
         *    Get the navigation params
         */

        if(sonar->fileType == QMIPS)
            *result = getQmipsNav(navigation, sonar, inbuf, totalDataSize,
                                                               navFileUpdate);
        if(sonar->fileType == EGG)
            getEggNav(navigation, sonar, inbuf);
        if(sonar->fileType == SEGY)
            getSegyNav(navigation, sonar, inbuf);
        if(sonar->fileType == EGGMIDAS)
            getMidasNav(navigation, sonar, inbuf);

        if(*result == 2)
            {
            sprintf(warningmessage, "Latitude and/or longitude out of bounds");
            strcat(warningmessage, "\nNavigation may be in eastings/northings");            messages(sonar, warningmessage);
            *result = 0;
            return;
            }

        if(sonar->latlon_flag)
            {
            ddtodegmin(navigation->latitude, &navigation->latdeg,
                                                       &navigation->latmin);
            ddtodegmin(navigation->longitude, &navigation->londeg,
                                                       &navigation->lonmin);
            }

        navCheckInterval = (int)(sonar->navInterval * 60.0);

        navOutputSeconds = navigation->min * 60 + navigation->sec;

        if(*navFileUpdate == 0 || (!(navOutputSeconds % navCheckInterval)
            && navigation->sec != *lastsec))
            {
            if(navigation->sec < 60)
                {
                if(sonar->latlon_flag)
                    fprintf(sonar->navfp,
                        "%03d %02d %02d %02d %02d %8.5f %4d %8.5f\n",
                        navigation->jday, navigation->hour, navigation->min,
                        navigation->sec, navigation->latdeg, navigation->latmin,                        navigation->londeg, navigation->lonmin);

                if(sonar->eastnorth_flag)
                    {
                    fprintf(sonar->navfp,"%03d %02d %02d %02d %8.2f %8.2f\n",
                        navigation->jday, navigation->hour, navigation->min,
                        navigation->sec, navigation->latitude,
                        navigation->longitude);
                    }
                }

            }

    if(navigation->sec < 60)
        *lastsec = navigation->sec;

    ++*navFileUpdate;


    /*
     *    if ALONG_DEC buffers have been acquired, compact the sonar
     *    data in the c_out structure by calculating a median value for
     *    the ALONG_DEC X ACROSS_DEC boxcar and output it
     */


    if(*c_count == (sonar->along_track - 1))
        {
        j = 0;
        for (i = 0; i < imageSize; i++) 
            {
            outdata[i] = c_median (&c_temp[j], 
                            sonar->along_track * sonar->across_track);
            j = (i * (sonar->along_track * sonar->across_track));
            }


        sonar->ping_header.djday = hmtodjd(navigation->jday, navigation->hour,
                navigation->min, ((float)navigation->sec +
                (double) navigation->sectenths));

        if(sonar->fileType == QMIPS)
            getQmipsTelemetry(sonar, telemetry, totalDataSize, inbuf,
                                               imageSize, navigation, dspData);
        else if(sonar->fileType == EGG)
            getEggTelemetry(sonar, telemetry, totalDataSize, inbuf,
                                                       imageSize, navigation);
#if 0
        else
            ;
#endif
/*
            getSegyTelemetry(sonar, telemetry, totalDataSize, inbuf,
                                                       imageSize, navigation);
*/

        sonar->ping_header.utm_n = sonar->ping_header.utm_e =
                                      sonar->ping_header.utm_azi = 0.0;

        sonar->ping_header.sdatasize = imageSize + HEADSIZE;

        sonar->ping_header.swath_width = sonar->swath;

/*
        tempSwath = (int) (get_float(inbuf, totalDataSize + 92, False) + 0.5, sonar);

        if(tempSwath * 2 != sonar->ping_header.swath_width)
            {
            sprintf(warningmessage, 
             "Warning:\nInput swath = %d\nRecorded swath = %d", 
             sonar->swath, tempSwath * 2);
            strcat(warningmessage, "\n\nRecorded swath width from file is");
            strcat(warningmessage, "\ndifferent from swath width entered");
            strcat(warningmessage, "\nin the Setup dialog.  Please re-enter.");
            messages(sonar, warningmessage);
            *result = 0;
            return;
            }
*/

        if(sonar->fish_azi_flag)
            correct_fish_azi(telemetry, sonar);
        else
            sonar->ping_header.fish_azi = 0.0;


        sonar->ping_header.merged = False;
        sonar->ping_header.fileType = XSONAR;
        if(INTEL)
            sonar->ping_header.byteOrder = INTEL;
        if(MOTOROLA)
            sonar->ping_header.byteOrder = MOTOROLA;

        sonar->ping_header.portBeamLimit = sonar->swath / 2;
        sonar->ping_header.stbdBeamLimit = sonar->swath / 2;

        if((outbytes = write(outfd, &sonar->ping_header, 256)) != 256
            || (outbytes = write(outfd, outdata, imageSize)) != imageSize)
            {
            sprintf(warningmessage, "Error writing output file");
            messages(sonar, warningmessage);
            *result = 0;
            return;
            }

        *c_count = 0;
        }
    else
        ++*c_count;

    return;
}
        
/*    getQmipsNav()
*
*       Function to extract navigation from the input buffer.
*       Also sets latitude and longitude in the output structure.
*
*/

int getQmipsNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf, int totalDataSize, int *navFileUpdate)
{
    double starday;
    float eastings, northings;

    int i;

    unsigned char *ptr;

    /*float get_float();*/

    /* char hem = 'n'; */

    short calender[6];
    /*short get_short();*/

    /*long julian();*/
    long stardate_factor;

    /*void utmgeo();*/

    if(sonar->latlon_flag)
        {
        navigation->longitude = get_double(inbuf, totalDataSize + 238, 
                                                               False, sonar);
        navigation->latitude = get_double(inbuf, totalDataSize + 206, 
                                                               False, sonar);

/*
        if(navigation->longitude > 180 || navigation->longitude < -180)
            return(2);
        if(navigation->latitude > 90 || navigation->latitude < -90)
            return(2);
*/

        sonar->ping_header.longitude = (float) navigation->longitude;
        sonar->ping_header.latitude = (float) navigation->latitude;
        }

    if(sonar->eastnorth_flag)
        {
        eastings = get_float(inbuf, totalDataSize + 206, False, sonar);
        northings = get_float(inbuf, totalDataSize + 210, False, sonar);

/*
        utmgeo (&navigation->latitude, &navigation->longitude, sonar->c_lon, 
                        eastings, northings, 'n', sonar->ellipsoid);

*/
        sonar->ping_header.longitude = eastings;
        sonar->ping_header.latitude = northings;
        navigation->longitude = eastings;
        navigation->latitude = northings;
        }

    ptr = &inbuf[totalDataSize];
    navigation->day = (int) *ptr;
    navigation->month = (int) *++ptr;
    navigation->year = (int) *++ptr;

    if(*navFileUpdate == 0)
        {
        navigation->previousYear = navigation->year;
        navigation->previousMonth = navigation->month;
        }

/*
    if(navigation->year != navigation->previousYear || navigation->month != navigation->previousMonth)
        return(0);
*/

    /*  If the julian day was not saved with the file
     *  (julian_flag not set), calculate the julian day
     *  and return it.
     */

    if((navigation->jday = (int) get_short(inbuf, 
        totalDataSize + 252, False, sonar)) == 0)
        {
        calender[0] = (navigation->year + 1900);
        calender[1] = 1;
        for(i = 2; i < 6; i++)
            calender[i] = 0;
        stardate_factor = julian(calender);
        calender[1] = *--ptr;
        calender[2] = *--ptr;

        starday = stardate(calender);
        navigation->jday = (long)starday - stardate_factor;
        *(ptr += 2);
        }

    navigation->hour = (int) *++ptr;
    navigation->min = (int) *++ptr;
    navigation->sec = (int) *++ptr;
    /*  tenthsSeconds  */
    navigation->sectenths = ( (float) get_short(inbuf, 
                                     totalDataSize + 6, False, sonar) / 1000.0);


    sonar->ping_header.year = navigation->year;
    sonar->ping_header.julian_day = navigation->jday;
    sonar->ping_header.sec = (float) (navigation->hour * 3600) + 
                             (float) (navigation->min * 60) + 
                             (float) navigation->sec + navigation->sectenths;


    navigation->previousMonth = navigation->month;
    navigation->previousYear = navigation->year;

    return(1);

}

void getMidasNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf)
{

    int i;

    short calender[6];
    /*short get_short();*/

    /*long julian();*/
    long stardate_factor;
    /*double stardate();*/
    double starday;

    /* double *latPointer, *lonPointer; */
    /* short *millisecPointer; */
#if 0
    short *yearPointer, *monthPointer, *dayPointer;
    short *hourPointer, *minutePointer, *secondPointer;
#endif

    navigation->latitude =  
                 (double) get_double(inbuf, 36000 + 304, False, sonar)/ 60.0;
    navigation->longitude = 
                 (double) get_double(inbuf, 36000 + 312, False, sonar)/ 60.0;


    sonar->ping_header.longitude = (float) navigation->longitude;
    sonar->ping_header.latitude = (float) navigation->latitude;

/*
    yearPointer = (short *) ntohs(inbuf[36000 + 224]);
    monthPointer = (short *) ntohs(inbuf[36000 + 222]);
    dayPointer = (short *) ntohs(inbuf[36000 + 220]);
    hourPointer = (short *) ntohs(inbuf[36000 + 226]);
    minutePointer = (short *) ntohs(inbuf[36000 + 228]);
    secondPointer = (short *) ntohs(inbuf[36000 + 230]);
    millisecPointer = (short *) ntohs(inbuf[36000 + 232]);
*/

    navigation->year = get_short(inbuf, 36000 + 204, False, sonar);
    navigation->hour = get_short(inbuf, 36000 + 240, False, sonar);
    navigation->min = get_short(inbuf, 36000 + 242, False, sonar);
    navigation->sec = get_short(inbuf, 36000 + 244, False, sonar) / 100;
/*
    navigation->sectenths = (double) 
                       get_short(inbuf, 36000 + 232, False, sonar) / 100.0;
*/
    
    navigation->sectenths = 0;

    calender[0] = (navigation->year + 1900);
    calender[1] = 1;
    for(i = 2; i < 6; i++)
        calender[i] = 0;
    stardate_factor = julian(calender);
    calender[1] = get_short(inbuf, 36000 + 202, False, sonar);
    calender[2] = get_short(inbuf, 36000 + 200, False, sonar);

    starday = stardate(calender);
    navigation->jday = (long)starday - stardate_factor;

    sonar->ping_header.year = navigation->year;
    sonar->ping_header.julian_day = navigation->jday;
    sonar->ping_header.sec = (float) (navigation->hour * 3600) +
                             (float) (navigation->min * 60) +
                             (float) navigation->sec + navigation-> sectenths;


    navigation->previousMonth = navigation->month;
    navigation->previousYear = navigation->year;

/*
fprintf(stdout, "nav hour = %d\n", get_short(inbuf, 36000 + 240, False, sonar));
fprintf(stdout, "zda min = %d\n", get_short(inbuf, 36000 + 242, False, sonar));
fprintf(stdout, "zda sec = %d\n", get_short(inbuf, 36000 + 244, False, sonar));
fprintf(stdout, "zda year = %d\n", get_short(inbuf, 36000 + 204, False, sonar));
fprintf(stdout, "zda day = %d\n", get_short(inbuf, 36000 + 200, False, sonar));
fprintf(stdout, "zda month = %d\n", get_short(inbuf, 36000 + 202, False, sonar));
fprintf(stdout, "cpu hour = %d\n", get_short(inbuf, 36000 + 226, False, sonar));
fprintf(stdout, "cpu day = %d\n", get_short(inbuf, 36000 + 220, False, sonar));
fprintf(stdout, "cpu month = %d\n", get_short(inbuf, 36000 + 222, False, sonar));
fprintf(stdout, "nav hour = %d\n", get_short(inbuf, 36000 + 240, False, sonar));

fprintf(stdout, "rec num = %d\n", get_int(inbuf, 36000 + 4, False, sonar));
fprintf(stdout, "rec length = %d\n", get_int(inbuf, 36000 + 16, False, sonar));
fprintf(stdout, "100 kHz link = %d\n", get_short(inbuf, 36000 + 21, False, sonar));
fprintf(stdout, "range link = %d\n", get_short(inbuf, 36000 + 20, False, sonar));
fprintf(stdout, "layback = %d\n", get_short(inbuf, 36000 + 370, False, sonar));
fprintf(stdout, "alt = %f\n", get_float(inbuf, 36000 + 424, False, sonar));
fprintf(stdout, "fish depth = %f\n", get_float(inbuf, 36000 + 420, False, sonar));
*/

    return;

}



void getSegyNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf)
{


    ShotHeader *shotHeader;
    /*int get_int();*/

    shotHeader = (ShotHeader *) inbuf;

    fprintf(stdout, "hour = %d\tmin = %d\tsec = %d\n", ntohs(shotHeader->hour),
ntohs(shotHeader->minute), ntohs(shotHeader->second));
    fprintf(stdout, "xsc = %d\tysc = %d\n", get_int(inbuf, 72, TRUE, sonar), get_int(inbuf, 76, TRUE, sonar));
    fprintf(stdout, "xrc = %d\tyrc = %d\n", ntohl(shotHeader->xrc), ntohl(shotHeader->yrc));


    navigation->longitude = (double) ntohl(shotHeader->xsc);;
    navigation->latitude = (double) ntohl(shotHeader->ysc);;

    navigation->day = 1;
    navigation->month = 1;
    navigation->year = 97;

    navigation->jday = ntohs(shotHeader->julday);;

    navigation->hour = ntohs(shotHeader->hour);
    navigation->min = ntohs(shotHeader->minute);
    navigation->sec = ntohs(shotHeader->second);

    navigation->sectenths = 0.0;
    sonar->ping_header.year = navigation->year;
    sonar->ping_header.julian_day = navigation->jday;
    sonar->ping_header.sec = (float) (navigation->hour * 3600) +
                             (float) (navigation->min * 60) +
                             (float) navigation->sec + navigation->sectenths;

    return;

}




void getEggNav(NavParams *navigation, MainSonar *sonar, unsigned char *inbuf)
{

  /*double stardate();*/
    double starday;

    int i;

    char hem;

    short calender[6];

    /*long julian();*/
    long stardate_factor;

    int latDegrees, lonDegrees;
    double latMinutes, lonMinutes;

    hem = 'n';

    navigation->longitude = (inbuf[19] & 0x7f) * .001 + 
                            (inbuf[20] & 0x7f) * .1 + 
                            (inbuf[21] & 0x7f) * 10 + 
                            (inbuf[22] & 0x7f) * 1000;
    navigation->latitude = (inbuf[24] & 0x7f) * .001 + 
                           (inbuf[25] & 0x7f) * .1 + 
                           (inbuf[26] & 0x7f) * 10 + 
                           (inbuf[27] & 0x7f) * 1000;

    lonDegrees = ( (int) (navigation->longitude / 100.0)) * 100;
    lonMinutes = navigation->longitude - (double) lonDegrees;
    lonMinutes /= 60.0;
    navigation->longitude = (double) lonDegrees / 100.0 + lonMinutes;

    if((inbuf[18] & 0x7f) == 'W' || (inbuf[18] & 0x7f) == 'w')
        navigation->longitude *= -1.0;   /* Make it west */
    else if(sonar->c_lon < 0)
        navigation->longitude *= -1.0;   /* Make it west */
#if 0
    else
        ;                                /* do nothing for now */
#endif    
    latDegrees = ( (int) (navigation->latitude / 100.0)) * 100;
    latMinutes = navigation->latitude - (double) latDegrees;
    latMinutes /= 60.0;
    navigation->latitude = (double) latDegrees / 100.0 + latMinutes;

    sonar->ping_header.longitude = navigation->longitude;
    sonar->ping_header.latitude = navigation->latitude;

    navigation->day = inbuf[6] & 0x7f;
    navigation->month = inbuf[5] & 0x7f;
    navigation->year = inbuf[4] & 0x7f;

    if(navigation->year != 0)
        {
        calender[0] = (navigation->year + 1900);
        calender[1] = 1;
        for(i = 2; i < 6; i++)
            calender[i] = 0;
        stardate_factor = julian(calender);
        calender[1] = navigation->month;
        calender[2] = navigation->day;

        starday = stardate(calender);
        navigation->jday = (long)starday - stardate_factor;
        }
    else
        navigation->jday = 0;

    navigation->hour = inbuf[3] & 0x7f;
    navigation->min = inbuf[2] & 0x7f;
    navigation->sec = inbuf[1] & 0x7f;

/*
fprintf(stdout, "jday = %d\tyear = %d\tmonth = %d\tday = %d\t18 = %c\n", navigation->jday, navigation->year, navigation->month, navigation->day, inbuf[18] & 0x7f);
fprintf(stdout, "hour = %d\tmin = %d\tsec = %d\n", navigation->hour, navigation->min, navigation->sec);
fprintf(stdout, "year = %d\tmonth = %d\tday = %d\n", navigation->year, navigation->month, navigation->day);
*/

    navigation->sectenths = 0.0;
    sonar->ping_header.year = navigation->year;
    sonar->ping_header.julian_day = navigation->jday;
    sonar->ping_header.sec = (float) (navigation->hour * 3600) + 
                             (float) (navigation->min * 60) +
                             (float) navigation->sec + navigation->sectenths;

    return;

}



/*    correct_fish_azi()
*
*     Used to compensate for magnetic declination off TRUE north.
*     To correct for this in a particular area, convert the magnetic
*     declination to radians and assign it to mag_dec.
*/  

void correct_fish_azi(Telemetry *telemetry, MainSonar *sonar)
{
    double temp_azi;
    /*float f_median();*/

    /*    Correct for magnetic declination of towfish compass.
     *    Magnetic declination is measured in RADIANS.
     *    Default is 0 (no correction). 
     */

    temp_azi = (double) f_median(telemetry->raw_course, sonar->along_track);

    temp_azi /= (double) RAD_TO_DEG;

    /*    This takes care of the offset of the AMS compass from MAGNETIC
     *    north ( in RADIANS).
     *    Comment this out if no offset of the fish compass is noted.

    if(temp_azi < (M_PI / 2.0))
        temp_azi += 0.1222;
    if(temp_azi > (M_PI / 2.0))
        temp_azi -= 0.1222;
     */

    sonar->ping_header.fish_azi = (float) temp_azi;

}


void close_demux(MainSonar *sonar, Widget w, unsigned char *inbuf, unsigned char *outbuf, unsigned char *outdata)
{


     Widget parent_widget;

     parent_widget = w;

     XtSetSensitive(XtNameToWidget(sonar->toplevel, "*DemuxButton"), True);

     if(inbuf != NULL)
         XtFree((char *)inbuf);
     if(outbuf != NULL)
         XtFree((char *)outbuf);
     if(outdata != NULL)
         XtFree((char *)outdata);

     if(sonar->navfp != NULL)
          {
          fclose(sonar->navfp);
          sonar->navfp = NULL;
          }
     if(sonar->magnetometer_flag)
        fclose(sonar->magfp);


     while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);

     XtPopdown(parent_widget);
     XtDestroyWidget(parent_widget);
 

     return;

}

int readSegyHeader(MainSonar *sonar, unsigned char *header, int infd, 
		   int *sonarDataSize, int *halfScan, int *pixelsPerChannel,
		   int *scanSize, int *totalDataSize,
		    UNUSED Widget demuxDialog,
		   int *numChannels, int *bytesPerPixel, int *numSonarChannels)
{
    char warningmessage[300];

    int inbytes;

    /* off_t position; */

    BCDHeader bcdheader;


    /*
     *    Skip the EBCDIC header in the data file.
     */

    inbytes = read(infd, header, EBCHDLEN);

    if(inbytes != EBCHDLEN)
        {
        sprintf(warningmessage, "Could not get past SEGY EBCDIC header");
        messages(sonar, warningmessage);
        XtFree((char *)header);
        return (0);
        }

    /*
     *    Read in the BCD header and get the required info.
     */

    inbytes = read(infd, &bcdheader, BCDHDLEN);

    if(inbytes != BCDHDLEN)
        {
        sprintf(warningmessage, "Could read the BCD header");
        messages(sonar, warningmessage);
        XtFree((char *)header);
        return (0);
        }

    *pixelsPerChannel = ntohs(bcdheader.nt);
fprintf(stdout, "pixels per channel = %d\n", *pixelsPerChannel);
    *numSonarChannels = *numChannels = ntohs(bcdheader.ntr);

    if(ntohs(bcdheader.dform) == 3)
        *bytesPerPixel = 2;

    *sonarDataSize = *pixelsPerChannel * 2;
    *halfScan = *sonarDataSize / 2;

    *totalDataSize = *numChannels * *pixelsPerChannel * *bytesPerPixel;
    *totalDataSize += TRHDLEN * *numChannels;

    *scanSize = *totalDataSize;


     return(1);

}




int readQmipsHeader(MainSonar *sonar, unsigned char *header, int infd, int *sonarDataSize, int *halfScan, int *pixelsPerChannel, int *scanSize, int *totalDataSize, int *new_flag, int *dsp_data, 
		     UNUSED Widget demuxDialog,
		    int *numChannels, int *bytesPerPixel, int *numSonarChannels)
{

    int inbytes;
    char warningmessage[300];


    off_t position;
    /*off_t lseek(int, off_t, int);*/

    /*void messages();*/

    /*short get_short();*/


    /*    
     *    Scan file version number from QMIPS data file.
     */

    inbytes = read(infd, header, FILEHEADERSIZE);

    if(inbytes != FILEHEADERSIZE)
        {
        sprintf(warningmessage, "Could not read QMIPS header");
        messages(sonar, warningmessage);
        XtFree((char *)header);
        return (0);
        }

    if(header[0] != 50)
        {
        position = lseek(infd, 1024L, SEEK_SET);
        inbytes = read(infd, header, FILEHEADERSIZE);
        *new_flag++;
        }

    if(header[0] != 50)
        {
        sprintf(warningmessage, 
          "Could not decipher QMIPS header.\nCheck your input file selection.");
        messages(sonar, warningmessage);
        XtFree((char *)header);
        return (0);
        }


    if(header[1] == 84 || header[1] == 202)
        *dsp_data = TRUE;


    *bytesPerPixel = (get_short(header, 32, False, sonar) / EIGHT_BITS);
    *pixelsPerChannel = get_short(header, 34, False, sonar);

    *numChannels = get_short(header, 30, False, sonar);
    *numSonarChannels = get_short(header, 46, False, sonar);

    *sonarDataSize = *pixelsPerChannel * 2;
    *halfScan = *sonarDataSize / 2;
    *totalDataSize = *numChannels * *pixelsPerChannel * *bytesPerPixel;
    *scanSize = *totalDataSize + 256;

    return (1);

}


void getQmipsTelemetry(MainSonar *sonar, Telemetry *telemetry, 
		       int totalDataSize, unsigned char *inbuf,
		        UNUSED int imageSize, NavParams *navigation, int dspData)
{


    float dspMagX, dspMagY, dspMagZ;

    short magx, magy, magz;

    sonar->ping_header.depth = f_median(telemetry->raw_depth,
                                                      sonar->along_track);
    sonar->ping_header.pitch = f_median(telemetry->raw_pitch,
                                                      sonar->along_track);
    sonar->ping_header.roll = f_median(telemetry->raw_roll,
                                                      sonar->along_track);

    /*
     *    Altitude
     */

    sonar->ping_header.alt = f_median(telemetry->raw_ap_alt,
                                                  sonar->along_track);
    /*
     *    Nav Ship Gyro and ships speed (same for DSP and QMIPS data)
     */

    sonar->ping_header.course = get_float(inbuf,totalDataSize + 230, 
                                                            False, sonar);
    sonar->ping_header.shipsSpeed = (float) get_short(inbuf,
                                 totalDataSize + 26, False, sonar) / 100.0;

    /*
     *    Conductivity and temperature from CTD (when available,
     *    otherwise zero).
     *
     */

    sonar->ping_header.conductivity = get_float(inbuf,
                                       totalDataSize + 218, False, sonar);

    sonar->ping_header.temperature = (float) get_short(inbuf,
                              totalDataSize + 180, False, sonar) / 100.0;

    /*
     *    Water pressure in psia
     */

    sonar->ping_header.waterPressure = get_float(inbuf,
                                       totalDataSize + 140, False, sonar);

    /*
     *    Slant range to fish
     */

    sonar->ping_header.range = (double) get_short(inbuf, totalDataSize + 152, False, sonar) / 10.0;
/*
    if(sonar->ping_header.range == 0)
        sprintf(warningmessage, "WARNING:  SLANT RANGE VALUE IS 0");
        messages(sonar, warningmessage);
*/

/*
fprintf(stdout, "byte 152 = %d\n", get_short(inbuf, totalDataSize + 152, False, sonar));
fprintf(stdout, "byte 246 = %d\n", get_short(inbuf, totalDataSize + 246, False, sonar));
*/

    sonar->ping_header.total_depth = get_float(inbuf,
                                         totalDataSize + 28, False, sonar);

    /*
     *    Get magnetometer settings and write to file if
     *    magnetometer_flag is set.
     */

    if(sonar->magnetometer_flag)
        {
        if(!dspData)
            {
            magx = get_short(inbuf, totalDataSize + 100, False, sonar);
            magy = get_short(inbuf, totalDataSize + 122, False, sonar);
            magz = get_short(inbuf, totalDataSize + 144, False, sonar);
            fprintf(sonar->magfp, "%f %f %f %d %d %d\n",
                sonar->ping_header.djday,
                navigation->latitude, navigation->longitude,
                magx, magy, magz);
            }
        else
            {
            dspMagX = get_float(inbuf, totalDataSize + 8, False, sonar);
            dspMagY = get_float(inbuf, totalDataSize + 12, False, sonar);
            dspMagZ = get_float(inbuf, totalDataSize + 16, False, sonar);
            fprintf(sonar->magfp, "%f %f %f %f %f %f\n",
                sonar->ping_header.djday,
                navigation->latitude, navigation->longitude,
                dspMagX, dspMagY, dspMagX);
            }

        }

    return;

}


void getEggTelemetry(MainSonar *sonar, 
		     UNUSED Telemetry *telemetry,
		     UNUSED int totalDataSize,
		     unsigned char *inbuf,
		     UNUSED int imageSize,
		     UNUSED NavParams *navigation)
{


    sonar->ping_header.depth = 0;
    sonar->ping_header.pitch = 0;
    sonar->ping_header.roll = 0;

    /*
     *    Altitude
     */

    sonar->ping_header.alt = (float) (inbuf[8] & 0x7f) * 10.0  + 
                            (float) (inbuf[7] & 0x7f) * 0.1;

    if(sonar->ping_header.alt == 0)
         sonar->ping_header.alt = 0.2;

    /*
     *    Nav Ship Gyro and ships speed
     */

    sonar->ping_header.course = (float) (inbuf[14] & 0x7f) * 100.0  + 
                               (float) (inbuf[13] & 0x7f);

    sonar->ping_header.shipsSpeed = (float) (inbuf[12] & 0x7f) * 10.0  +  
                                   (float) (inbuf[11] & 0x7f) * 0.1;

    /*
     *    Swath width
     */

    sonar->ping_header.swath_width = ((int) (inbuf[16] & 0x7f)  +
                            (int) (inbuf[17] & 0x7f) * 100) * 2;


    /*
     *    Conductivity and temperature from CTD (when available,
     *    otherwise zero).
     */

    sonar->ping_header.conductivity = 0;
    sonar->ping_header.temperature = 0;

    /*
     *    Water pressure in psia
     */

    sonar->ping_header.waterPressure = 0;

    /*
     *    Slant range to fish
     */

    sonar->ping_header.range = 0;

    sonar->ping_header.total_depth = 0;


    return;

}

void getMidasSizes(int *sonarDataSize, int infd)
{

    int sampleCounter;
    int oldSampleCounter;
    int finalSampleCount;
    int frameCounter;
    int pingNum;
    int firstTrig;
    int i, j;
    int bytesread;

    off_t position;
    /*off_t lseek(int, off_t, int);*/

    union {
    unsigned char data[36864];
    short data2[18432];
    } test;

    sampleCounter = 0;
    oldSampleCounter = 0;
    finalSampleCount = 0;
    pingNum = 0;
    frameCounter = 0;
    firstTrig = 0;

    position = lseek(infd, 0L, SEEK_SET);

    bytesread = read(infd, test.data, 36864);
    for(j = 0; j < 100; j++)
        {

        for(i = 0; i < 1000; i++)
            {
            if(firstTrig)
                 sampleCounter+=6;

            if(ntohs(test.data2[4+i*18]) > 0)
                {
               
                if(firstTrig)
                    {
                    if((finalSampleCount < sampleCounter) && 
                        (sampleCounter - oldSampleCounter < 100))
                        finalSampleCount = sampleCounter;
                    }

                oldSampleCounter = sampleCounter;
                sampleCounter = 0;

                firstTrig++;

                }
            frameCounter++;
            }

        bytesread = read(infd, test.data, 36864);
        }

    fprintf(stdout, "final sample count for 100 kHz is %d\n", finalSampleCount);
    *sonarDataSize = finalSampleCount;
    position = lseek(infd, 0L, SEEK_SET);


}



/*
fprintf(stderr, "header[0] = %d\n", header[0]);
fprintf(stderr,"pixels  per channel = %d\n", pixelsPerChannel);
fprintf(stderr,"bytesPerPixel  = %d\n", bytesPerPixel);
fprintf(stderr,"num  channels = %d\n", numChannels);
fprintf(stderr,"number  of sonar channels = %d\n", numSonarChannels);
fprintf(stderr,"bits  per pixel = %d\n", get_short(header, 32));
fprintf(stderr,"async  chan # = %d\n", get_short(header, 44));
fprintf(stderr,"ch1_processing  = %d\n", get_short(header, 48));
fprintf(stderr,"ch2_processing  = %d\n", get_short(header, 50));
fprintf(stderr,"ch3_processing  = %d\n", get_short(header, 52));
fprintf(stderr,"ch4_processing  = %d\n", get_short(header, 54));
fprintf(stdout,"range to demux = %d\n", sonar->meterRange);
fprintf(stdout,"datasize  = %d, total datasize = %d, scansize = %d\n",
QMIPS_DAT
ASIZE, QMIPS_TOTAL_DATASIZE, QMIPS_SCANSIZE);
fprintf(stdout,"pixelRange  = %d\n", pixelRange);
fprintf(stdout,"pixels  to cut = %d\n", pixelsToCut);
fprintf(stdout,"imagesize  = %d\n", imageSize);
fprintf(stdout,"inputResolution  = %f\n", inputResolution);
fprintf(stdout,"pixelRange  = %d\n", pixelRange);
fprintf(stdout,"swath  = %d\n", sonar->swath);
*/
/*
tempPixel = (short) ((float)port16Bit100KhzValue / 5119.0 * 255.0);
if(tempPixel > 255)
outbuf[j + pixelRange] = 255;
else
outbuf[j + pixelRange] = (unsigned char) tempPixel;
tempPixel = (short) ((float)stbd16Bit100KhzValue / 5119.0 * 255.0);
if(tempPixel > 255)
outbuf[k] = 255;
else
outbuf[k] = (unsigned char) tempPixel;
*/
/*
if(stbd16Bit100KhzValue > 6144)
fprintf(stdout, "MAX on stbd, value = %d, range = %d, ping = %d\n", stbd16Bit100KhzValue, j, i); 
*/
/*
fprintf(stdout, "\n******************* Start scan *******************\n");
fprintf(stdout, "input stbd value = %d\n", tempValue);

fprintf(stdout, "\n******************* Start scan *******************\n");
fprintf(stdout, "input port value = %d\n", port16Bit100KhzValue);
fprintf(stdout, "input port value = %d, shifted = %d\toutput port value = %d\n", port16Bit100KhzValue, port16Bit100KhzValue >> sonar->portBitShift, outbuf[j + pixelRange]);
                if(outbuf[j] > 255)
                    outbuf[j] = 255;
                if(outbuf[j] < 0)
                    outbuf[j] = 0;

                if(outbuf[j + pixelsPerChannel] < 0)
                    outbuf[j + pixelsPerChannel] = 0;
fprintf(stdout, "raw stbd pix %d = %d\n", halfScan + j + sonar->stbdOffset, stbd16Bit100KhzValue);
fprintf(stdout, "out stbd pix %d = %d\n", k, outbuf[k]);
fprintf(stdout, "raw stbd pix %d = %d\n", j, stbd16Bit100KhzValue);

*/
/*
fprintf(stdout, "start ping = %d, end ping = %d\n", sonar->startScanForRemoveRampPort,sonar->endScanForRemoveRampPort);
  fprintf(stderr, "start position is %d, cur pos = %d\n", startPosition, position);
        fprintf(stdout, "%d, %d, %d\n", j, portCurveModel[j], stbdCurveModel[j]);
*/
/*
fprintf(stdout, "ping = %d, j = %d, buffer value = %d, model value = %d\n", stbdPingsRead, j, curveModel.tempBuffer[j], stbdCurveModel[j]);
fprintf(stdout, "start position is %d, cur pos = %d\n", startPosition, position);
fprintf(stdout, "bytesread = %d\n", inbytes);
fprintf(stdout, "ping = %d, j = %d, k = %d, buffer value = %d, model value = %d\n", portPingsRead, j, k, curveModel.tempBuffer[j], portCurveModel[j]);
fprintf(stdout, "ping = %d, short value = %hd, char value = %d, model value = %d\n", pingsRead, curveModel.tempBuffer[j], curveModel.tempCharBuffer[k], portCurveModel[j]);
fprintf(stdout, "value = %d\n", portCurveModel[j]);
fprintf(stdout, "ping = %d, short value = %hd, char value = %d, model value = %d\n", pingsRead, curveModel.tempBuffer[j + pixelsPerChannel], curveModel.tempCharBuffer[k], stbdCurveModel[j]);
*/

