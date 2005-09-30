#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "xsonar.h"

#include "sonar_struct.h"

void convertXsonar(FileFormat *fileToConvert, MainSonar *sonar)
{
    PingHeader outheader;

    Widget progressWindow;

    GC progressGC;

    Dimension progressWidth;
    Dimension progressHeight;
    Dimension drawAmount;

    //Arg args[10];
    //Cardinal n;

    struct stat file_params;

    unsigned char *outbuffer;
    unsigned char *inbuffer;

    char warningmessage[255];
    char messageBuffer[100];
    char parentDialog[100];

    int datasize;
    int headersize;
    int scansize;
    int updateScanSize;
    int inbytes;
    int swath;
    int checkFileStatus;
    int i;
    int conv_recs;

    float percentDone;
    long position;
    long fileSize;

    /*
     *    Initialize variables.
     */

    headersize = 256;
    conv_recs = 0;
    updateScanSize = 0;

    inbuffer = (unsigned char *) calloc(headersize, sizeof(unsigned char));


    /*
     *   Read the first ping header to determine the scansize and swath.
     */

    if((inbytes = read(fileToConvert->inputFileFP, inbuffer, headersize)) 
        != headersize)
        {
        sprintf(warningmessage, "Error reading header, check permissions");
        messages(sonar, warningmessage);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        return;
        }
    else
        scansize = get_int(inbuffer, 84, True, sonar);

    if(!(scansize % 512))
        {
        sprintf(warningmessage, "Could not determine scansize ... ");
        messages(sonar, warningmessage);
        getScanSize(sonar, &scansize, &headersize);
        updateScanSize++;
        }


    swath = get_int(inbuffer, 88, True, sonar);
    if(!swath)
        {
        sprintf(warningmessage, "Could not read swath.\n");
        strcat(warningmessage, "Using swath from Setup");
        messages(sonar, warningmessage);
        swath = sonar->swath;
        }


    position = lseek(fileToConvert->inputFileFP, 0L, SEEK_SET);

    datasize = scansize - headersize;

    free(inbuffer);

     /*
      *     Find file size, and compute number of pings.
      *     Then compute total number of minutes (time) for file.
      */

    if((checkFileStatus = stat(sonar->infile, &file_params)) == -1)
        {
        sprintf(warningmessage, "Cannot read data file size");
        messages(sonar, warningmessage);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        return;
        }

    fileSize = file_params.st_size;


fprintf(stderr, "scansize is %d\n", scansize);
fprintf(stdout, "datasize is %d\n", datasize);
fprintf(stdout, "headersize is %d\n", headersize);
fprintf(stderr, "swath is %d\n", swath);
fprintf(stderr, "filesize is %d\n", (int)fileSize);

    /*
     *    Allocate space for the input and output buffers.
     */

    inbuffer = (unsigned char *) calloc(scansize, sizeof(unsigned char));
    outbuffer = (unsigned char *) calloc(datasize, sizeof(unsigned char));

    /*
     *    Everything looks OK, set up the progress window and begin ....
     */

    strcpy(messageBuffer, "Converting XSONAR file between\n");
    strcat(messageBuffer, "INTEL and MOTOROLA byte formats...");
    strcpy(parentDialog, "*MainForm");

    progressWindow = setUpProgressWindow(sonar, parentDialog,
                   messageBuffer, &progressWidth, &progressHeight, &progressGC);

    while((inbytes = read(fileToConvert->inputFileFP, inbuffer, scansize)) 
        == scansize)
        {

        outheader.djday        =     get_double(inbuffer, 0, True, sonar);
        outheader.utm_n        =     get_double(inbuffer, 8, True, sonar);
        outheader.utm_e        =     get_double(inbuffer, 16, True, sonar);
        outheader.utm_azi      =     get_double(inbuffer, 24, True, sonar);
        outheader.fish_azi     =     get_float(inbuffer, 32, True, sonar);
        outheader.latitude     =     get_float(inbuffer, 36, True, sonar);
        outheader.longitude    =     get_float(inbuffer, 40, True, sonar);
        outheader.course       =     get_float(inbuffer, 44, True, sonar);
        outheader.pitch        =     get_float(inbuffer, 48, True, sonar);
        outheader.roll         =     get_float(inbuffer, 52, True, sonar);
        outheader.depth        =     get_float(inbuffer, 56, True, sonar);
        outheader.alt          =     get_float(inbuffer, 60, True, sonar);
        outheader.total_depth  =     get_float(inbuffer, 64, True, sonar);
        outheader.range        =     get_float(inbuffer, 68, True, sonar);
        outheader.sec          =     get_float(inbuffer, 72, True, sonar);
        outheader.year         =     get_int(inbuffer, 76, True, sonar);
        if(outheader.year < 0 || outheader.year > 99)
            {
            outheader.year = 70;
            outheader.djday = 1.0;
            }
        outheader.julian_day   =     get_int(inbuffer, 80, True, sonar);
        if(updateScanSize)
            outheader.sdatasize =    scansize;
        else
            outheader.sdatasize =     get_int(inbuffer, 84, True, sonar);
        outheader.swath_width  =     swath;

        outheader.pulse_width  =     get_float(inbuffer, 92, True, sonar);
        outheader.power        =     get_int(inbuffer, 96, True, sonar);
        outheader.clon         =     get_float(inbuffer, 100, True, sonar);
        outheader.merged       =     get_int(inbuffer, 104, True, sonar);
        outheader.fileType     =     XSONAR;

        if(INTEL)
            outheader.byteOrder = INTEL;
        if(MOTOROLA)
            outheader.byteOrder = MOTOROLA;

        outheader.temperature    =    get_float(inbuffer, 116, True, sonar);
        outheader.conductivity   =    get_float(inbuffer, 120, True, sonar);
        outheader.waterPressure  =    get_float(inbuffer, 124, True, sonar);
        outheader.shipsSpeed     =    get_float(inbuffer, 128, True, sonar);

        if(outheader.utm_e > 0.0 && outheader.utm_e < 1000000.0)
            outheader.merged = 1;

        for(i = 0; i < datasize; i++)
            outbuffer[i] = inbuffer[i + headersize];

        if(write(fileToConvert->outputFileFP, &outheader, headersize) 
            != headersize)
            {
            sprintf(warningmessage,"Can't write header to output file\n"); 
            messages(sonar, warningmessage);
            close(fileToConvert->inputFileFP);
            close(fileToConvert->outputFileFP);
            return;
            }

        if(write(fileToConvert->outputFileFP, outbuffer, datasize) != datasize)
            {
            sprintf(warningmessage,"Can't write data to output file\n"); 
            messages(sonar, warningmessage);
            close(fileToConvert->inputFileFP);
            close(fileToConvert->outputFileFP);
            return;
            }

        percentDone = ((float) conv_recs * scansize / (float) fileSize);

        drawAmount = (Dimension) (percentDone * (float)progressWidth);

        XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

        XmUpdateDisplay(sonar->toplevel);

        conv_recs++;
        }

    close(fileToConvert->inputFileFP);
    close(fileToConvert->outputFileFP);

    free(inbuffer);
    free(outbuffer);

    /*
     *  Free up the gc created, close down the input and output files,
     *  popdown the progress monitor and free up allocated space.
     */

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));


    return;

}

