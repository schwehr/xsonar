/*    program whips.c
 *
 *    Program to write out an image file and the associated ASCII
 *    navigation file taken from the internal format used for Xsonar.
 *    The output files can be used to reformat the file into a netCDF
 *    file used in the USGS WHIPS system by the WHIPS program "mipssonar".
 *
 *    William W. Danforth, U.S. Geological Survey
 *    July, 1993.
 *
 *
 */

#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h> 

#include <map.h>

#include "xsonar.h"

#include "sonar_struct.h"

void whipsWrite(FileFormat *fileToConvert, MainSonar *sonar,
		 UNUSED  int inputNavFP, FILE *whipsNavFP)
{

  /*Widget fileLabel;*/
  /*Widget setup;*/
    Widget progressWindow;
    /*Widget setUpProgressWindow();*/

    GC progressGC;

    Dimension progressWidth;
    Dimension progressHeight;
    Dimension drawAmount;

    /*Arg args[10];*/
    /*Cardinal n;*/

    struct stat file_params;

    unsigned char *outdata, *indata;

    short calender[6];
    /*short *calender_ptr;*/
    /*short invstardate();*/

    /*long julian();*/
    long stardate_factor;
    long position;
    long fileSize;

    char parentDialog[100];
    char warningmessage[500];
    char headerBuffer[80];
    char messageBuffer[100];
    /*char tempText[100];*/
    char hem;

    int checkFileStatus;
    int i;/*, j, c, z;*/
    int scansize, imagesize;
    int halfscan;
    int /*inbytes,*/ outbytes;
    int scannum;
    int headsize;
    /*int cancelCheck;*/
    int eventCheck;
    int hour, min;
    int azimuth;
    int altitude;

    double sec;
    double latitude, longitude;
    /*double stardate();*/
    double fminute;

    float percentDone;
    /*float getcentlon();*/

    /*void messages();*/
    /*void utmgeo();*/

    /*
     *    Initialize some local variables 
     */

    headsize = HEADSIZE;     /*   defined in sonar_struct.h  */
    scannum = 0;
    hem = 'n';


    /*
     *    allocate memory for input and output ping
     */

     if(read (fileToConvert->inputFileFP, 
         &sonar->ping_header, headsize) != headsize)
         {
         sprintf(warningmessage, "cannot read first header");
         messages(sonar, warningmessage);
         close(fileToConvert->inputFileFP);
         close(fileToConvert->outputFileFP);
         fclose(whipsNavFP);
         return;
         }

    position = lseek(fileToConvert->inputFileFP, 0L, SEEK_SET);

    scansize = sonar->ping_header.sdatasize;

    if(scansize == (int) NULL || (scansize - headsize) % 512)
        getScanSize(sonar, &scansize, &headsize);

    imagesize = scansize - HEADSIZE;
    halfscan = imagesize / 2;

    outdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(outdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for output sonar array");
        messages(sonar, warningmessage);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        fclose(whipsNavFP);
        return;
        }

    indata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(indata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for input sonar array");
        messages(sonar, warningmessage);
        XtFree((char *)outdata);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        fclose(whipsNavFP);
        return;
        }

    if(!sonar->c_lon)
        {
        sprintf(warningmessage,
                 "Please enter a central longitude in setup.");
        messages(sonar, warningmessage);
        XtFree((char *)indata);
        XtFree((char *)outdata);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        fclose(whipsNavFP);
        return;
        }

    if(!sonar->swath)
        {
        sprintf(warningmessage,
                 "Please enter a swath width in setup.");
        messages(sonar, warningmessage);
        XtFree((char *)indata);
        XtFree((char *)outdata);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        fclose(whipsNavFP);
        return;
        }

    /*
     *    Everything looks OK, set up the progress window and begin ....
     */

    strcpy(messageBuffer, "Converting to WHIPS format ...");
    strcpy(parentDialog, "*MainForm");

    progressWindow = setUpProgressWindow(sonar, parentDialog,
                   messageBuffer, &progressWidth, &progressHeight, &progressGC);

     /*
      *     Find file size, and compute number of pings.
      *     Then compute total number of minutes (time) for file.
      */

    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;

    if((checkFileStatus = stat(sonar->infile, &file_params)) == -1)
        {
        sprintf(warningmessage, "Cannot read data file size");
        messages(sonar, warningmessage);
        XtFree((char *)indata);
        XtFree((char *)outdata);
        close(fileToConvert->inputFileFP);
        close(fileToConvert->outputFileFP);
        fclose(whipsNavFP);
        return;
        }

    fileSize = file_params.st_size;

    /*
     *    Main loop
     */

    eventCheck = 1;

    while((read (fileToConvert->inputFileFP, &sonar->ping_header, 256) == 256) 
        && (read (fileToConvert->inputFileFP, indata, imagesize) == imagesize))
        {

        /*
         *    Begin processing the data.
         */


        if((outbytes = write(fileToConvert->outputFileFP, 
            indata, imagesize)) != imagesize)
            {
            sprintf(warningmessage,
                     "Error in image output, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            close(fileToConvert->inputFileFP);
            close(fileToConvert->outputFileFP);
            fclose(whipsNavFP);
            return;
            }

        percentDone = ((float) scannum * scansize / (float) fileSize);

        drawAmount = (Dimension) (percentDone * (float)progressWidth);

        XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

        XmUpdateDisplay(sonar->toplevel);

        scannum++;

        utmgeo(&latitude, &longitude, sonar->c_lon, sonar->ping_header.utm_e, 
                              sonar->ping_header.utm_n, hem, sonar->ellipsoid);

        djdtohm(sonar->ping_header.djday, &hour, &min, &sec);
        fminute = (float) min  + (sec / 60.0);

        calender[0] = (sonar->ping_header.year + 1900);
        calender[1] = 1;
        for(i = 2; i < 6; i++)
             calender[i] = 0;
        stardate_factor = julian(calender);

        invstardate(((double)sonar->ping_header.djday + stardate_factor), 
                                                                    calender);

        azimuth = (int)(sonar->ping_header.utm_azi * RAD_TO_DEG);
        altitude = (int) sonar->ping_header.alt;

        sprintf(headerBuffer,
                    "%6d %2d %02d %02d %02d %8.5f %9.6f %10.6f %2d %2d %3d\n",
                    scannum, sonar->ping_header.year, calender[1], calender[2],
                    hour, fminute, latitude, longitude, azimuth, 0, altitude);

        if((outbytes = fwrite(headerBuffer, 
            1, strlen(headerBuffer), whipsNavFP)) != (int)strlen(headerBuffer))
            {
            sprintf(warningmessage,
                     "Error in header output, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            close(fileToConvert->inputFileFP);
            close(fileToConvert->outputFileFP);
            fclose(whipsNavFP);
            return;
            }

        }

    close(fileToConvert->inputFileFP);
    close(fileToConvert->outputFileFP);

    sprintf(warningmessage, "Parameters for MIPSSONAR (netCDF conversion):\n     Number of lines = %d\n     Number of samples = %d\nOutput resolution is %.3f m/pixel", scannum, imagesize, sonar->swath / (float)imagesize);

    messages(sonar, warningmessage);


    /*
     *  Free up the gc created, close down the input and output files,
     *  popdown the progress monitor and free up allocated space.
     */

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));

    XtFree((char *)indata);
    XtFree((char *)outdata);

    fclose(whipsNavFP);

    return;

}
