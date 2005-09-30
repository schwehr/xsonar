#include <unistd.h>

#include "sonar_struct.h"
#include "mips.h"

#include "xsonar.h"

void convertMips(FileFormat *fileToConvert, MainSonar *sonar, char *fileBuffer)
{
    Widget progressWindow;

    GC progressGC;

    PingHeader pingHeader;

    Dimension progressWidth, progressHeight;
    Dimension drawAmount;

    short header[256];
    short recnum;
    short numlines, numsamps;
    short blocksPerScan;

    /*short alt1;*/
    /*short alt2;*/
    /*short passnum;*/
    /*short linenum;*/
    /*short course;*/
    /*short day;*/
    /*short month;*/
    /*short year;*/
    /*short hour;*/
    /*float minutes;*/
    /*float latitude;*/
    /*float longitude;*/

    /*int i, j;*/
    /*int fp1, fp2;*/
    int writebytes, readbytes;
    int blocksToRead;
    int fileSize;
    int scansize;
    /*int bytesRemaining;*/

    long position;
    long nextScanPosition;

    float percentDone;

    char warningMessage[100];
    char messageBuffer[255];
    char parentDialog[255];

    unsigned char *indata, *trailer;

    if((readbytes = read(fileToConvert->inputFileFP, header, 512)) != 512)
        {
        sprintf(warningMessage,
                 "Error reading input file.\nPlease reselect a file.");
        messages(sonar, warningMessage);
        return;
        }

    if(MOTOROLA)
        {
	  /* FIX: why is header short when it should be char * */
        numlines = get_short((char*)header, 0, False, sonar);
        numsamps = get_short((char*)header, 2, False, sonar);
        blocksPerScan = get_short((char*)header, 4, False, sonar);
        }
    else
        {
        numlines = header[0];
        numsamps = header[1];
        blocksPerScan = header[2];
        }

    blocksToRead = numsamps / 512;

    scansize = blocksPerScan * 512;
    fileSize = scansize * numlines + 5120;

    /*
     *    Everything looks OK, set up the progress window and begin ....
     */

    strcpy(messageBuffer, "Converting from MIPS to XSONAR ...");
    strcpy(parentDialog, "*MainForm");

    progressWindow = setUpProgressWindow(sonar, parentDialog,
                   messageBuffer, &progressWidth, &progressHeight, &progressGC);

    indata = (unsigned char *) XtCalloc(numsamps, sizeof(unsigned char));
    trailer = (unsigned char *) XtCalloc(32, sizeof(unsigned char));

    position = lseek(fileToConvert->inputFileFP, 5120L, SEEK_SET);

    recnum = 0;

    while(recnum < numlines)  
        {

        readbytes = read(fileToConvert->inputFileFP, indata, numsamps);
/*
        for(j = 1; j < blocksToRead; j++)
            {
            readbytes = read(fileToConvert->inputFileFP, indata, 512);
            writebytes = write(fileToConvert->outputFileFP, indata, 512);
            }

        bytesRemaining = numsamps - blocksToRead * 512;

        readbytes = read(fileToConvert->inputFileFP, indata, bytesRemaining);
        writebytes = write(fileToConvert->outputFileFP, indata, bytesRemaining);
*/

        /*
         *  Now read the trailer and set the variables for the XSonar
         *  pingheader.
         */


        readbytes = read(fileToConvert->inputFileFP, trailer, 32);

/*
        passnum = get_short(trailer, 0, False, sonar);
        linenum = get_short(trailer, 2, False, sonar);
        course = get_short(trailer, 4, False, sonar);
        day = get_short(trailer, 8, False, sonar);
        month = get_short(trailer, 10, False, sonar);
        year = (int) get_short(trailer, 12, False, sonar);
        hour = get_short(trailer, 14, False, sonar);
        minutes = getVaxFloat(trailer, 16);
        latitude = getVaxFloat(trailer, 20);
        longitude = getVaxFloat(trailer, 24);
        alt1 = get_short(trailer, 28, False, sonar);
        alt2 = get_short(trailer, 30, False, sonar);
 fprintf(stdout, "ping header = %d bytes\n", sizeof(pingHeader));
 fprintf(stdout, "passnum = %d\tlinenum = %d\tcourse = %d\n", passnum, linenum, course);
 fprintf(stdout, "day = %d\tmonth = %d\tyear = %d\n", day, month, year);
 fprintf(stdout, "alt1 = %d\talt2 = %d\n", alt1, alt2);
 fprintf(stdout, "minutes = %f\n", minutes);
fprintf(stdout, "wrote %d bytes\n", writebytes);
*/

        pingHeader.sdatasize = numsamps + sizeof(pingHeader);
        pingHeader.swath_width = sonar->swath;
        pingHeader.djday = 0;
        pingHeader.utm_n = 0;
        pingHeader.utm_e = 0;
        pingHeader.utm_azi = 0;
        pingHeader.latitude = 0;
        pingHeader.longitude = 0;
        pingHeader.course = 0;
        pingHeader.pitch = 0;
        pingHeader.roll = 0;
        pingHeader.depth = 0;
        pingHeader.alt = 0;
        pingHeader.total_depth = 0;
        pingHeader.range = 0;
        pingHeader.sec = 0;
        pingHeader.year = 0;
        pingHeader.julian_day = 0;
        pingHeader.power = 0;
        pingHeader.clon = 0;
        pingHeader.merged = 0;
        pingHeader.fileType = XSONAR;
        if(INTEL)
            pingHeader.byteOrder = LSB;
        if(MOTOROLA)
            pingHeader.byteOrder = MSB;
        
        writebytes = write(fileToConvert->outputFileFP, 
                                           &pingHeader, sizeof(pingHeader));
        writebytes = write(fileToConvert->outputFileFP, indata, numsamps);

        percentDone = ((float) recnum * scansize / (float) fileSize);

        drawAmount = (Dimension) (percentDone * (float)progressWidth);

        XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

        XmUpdateDisplay(sonar->toplevel);

        recnum++;

        nextScanPosition = recnum * (blocksPerScan * 512) + 5120;
        position = lseek(fileToConvert->inputFileFP, 
                                         nextScanPosition, SEEK_SET);
        
        }

    sprintf(warningMessage, "Converted input MIPS file:\n     Number of lines = %d\n     Number of samples = %d\n", recnum, numsamps);
    strcat(warningMessage, "Output file name is:\n"); 
    strcat(warningMessage, fileBuffer); 

    messages(sonar, warningMessage);

    /*
     *  Free up the gc created, close down the input and output files,
     *  popdown the progress monitor and free up allocated space.
     */

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));

    close(fileToConvert->inputFileFP);
    close(fileToConvert->outputFileFP);

}


#if 0
void getheader(short *recnum, short *passnum)
{
    outdata.buffer1.passnum = passnum;
    outdata.buffer1.linenum = recnum;
    outdata.buffer1.course = (short) atoi(inbuffer.course);
    outdata.buffer1.prp = 0;
    outdata.buffer1.day = (short) atoi(inbuffer.day);
    outdata.buffer1.month = (short) atoi(inbuffer.month);
    outdata.buffer1.year = (short) atoi(inbuffer.year);
    outdata.buffer1.hour = (short) atoi(inbuffer.hour);
    outdata.buffer1.minutes = atof(inbuffer.minutes);
    outdata.buffer1.latitude = atof(inbuffer.latitude);
    outdata.buffer1.longitude = atof(inbuffer.longitude);
    outdata.buffer1.cor_alt = 0;
    outdata.buffer1.uncor_alt = (short) atoi(inbuffer.alt);
}
#endif
