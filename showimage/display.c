#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <X11/Xutil.h>

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void updateProgressWin(Widget win, GC gc, const Dimension width, const Dimension height, const int scans, const int counter)
/*Widget win;
GC gc;
Dimension width, height;
int scans, counter;*/
{
    float percentDone;
    Dimension drawAmount;

    percentDone = (float)counter / (float)scans;
    drawAmount = (Dimension) (percentDone * (float)width);

    XDrawLine(XtDisplay(win), XtWindow(win), gc, drawAmount, 
                                                0, drawAmount, height);

    XmUpdateDisplay(win);

    return;
}


void show_image(Widget w, XtPointer client_data, XtPointer call_data)
/*Widget         w;
XtPointer     client_data;
XtPointer     call_data;*/
{


    struct draw *drawarea = (struct draw * ) client_data;

    /*Widget message;*/
    Widget progressWindow;

    GC progressGC;

    /*Arg args[40];*/

    /*XEvent event;*/
    /*XWindowAttributes attributes;*/
 
    int i, j, k, z;
    /*int i, j, k, l, n, y, z;*/
    /*int numlines;*/
    int number_of_scans;
    /*int scansizeInBytes;*/
    /*int data_offset;*/
    /*int qmipsReadOffset;*/
    int display_offset;
    /*int width, height;*/
    /*int pix;*/
    int displayDataPad;
    /*int pix_location;*/
    /*int bufferBytes;*/
    /*int fileSize;*/
    /*int fileStatus;*/
    int currentInbytes;
    /*int bytesToRead;*/
    int bytesPerPixel;

    int displayDepth;

    int colorIndex;

    unsigned int subbottomSize;

    unsigned short shortValue;
    unsigned char convertData;

    off_t position;
    /*off_t filePosition;*/

    /*float percentDone;*/

    Dimension progressWindowWidth;
    Dimension progressWindowHeight;
    /*Dimension drawAmount;*/

#if 0
    void showProgress();
    void message_display();
    void updateProgressWin();
    void checkForNans();
    void showAltitude();
    void showBeamCorrectionLimits();
    void perror();
#endif

    char messageBuffer[40];

    /*unsigned char *headerPtr;*/
#if 0
    int getHeaderInfo();
    unsigned short get_short();
#endif

    if(fp1 == -1 || fp1 == EOF)
        {
        message_display(drawarea, fp1);
        return;
        }

    if(drawarea->fileType == NOTYPE)
        {
        message_display(drawarea, NOTYPE);
        return;
        }

    if(lseek(fp1, 0L, SEEK_CUR) == 0 || drawarea->bitsPerPixel == 0)
        drawarea->bitsPerPixel = getHeaderInfo(drawarea);


    /*
     *   return if bitsPerPixel were set to NULL while getting header
     *   info.  Indicates that the action was cancelled or that the wrong
     *   file type was selected.
     */

    if(!drawarea->bitsPerPixel)
        return;

    if(w == XtNameToWidget(drawarea->shell, "*PortBitShiftScale") ||
        w == XtNameToWidget(drawarea->shell, "*StbdBitShiftScale"))
        {
        XmScaleGetValue(XtNameToWidget(drawarea->shell, "*PortBitShiftScale"), 
                                                      &drawarea->portBitShift);
        XmScaleGetValue(XtNameToWidget(drawarea->shell, "*StbdBitShiftScale"), 
                                                      &drawarea->stbdBitShift);
        position = lseek(fp1, (off_t)
                  (drawarea->top_ping * scansize + fileHeaderSize), SEEK_SET);
        }

/*
    if((datasize * datasize) > MAX_MEMORY)
        {
        message_display(drawarea, NOMEMORY);
        return;
        }
*/

    data_reducer = datasize / 512;


    if(datasize % 512)
        data_reducer++;


    if(image != NULL)
        {
        XFree((char *)image);
        XtFree((char *)display_data);
        image = NULL;
        display_data = NULL;
        }

    number_of_scans = 512 * data_reducer;

/*
    number_of_scans = datasize;
*/

    if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED)
        drawarea->top_ping = lseek(fp1, 0L, SEEK_CUR) / scansize;


    if(drawarea->fileType == QMIPS || drawarea->fileType == RASTER)
        drawarea->top_ping = (lseek(fp1, 0L, SEEK_CUR) - fileHeaderSize) 
                                                                   / scansize;


    bytesPerPixel = drawarea->bitsPerPixel / 8;

    if(drawarea->visual->class == TrueColor)
        display_data = (unsigned char *) XtCalloc(512*512, sizeof(int));

    if(drawarea->visual->class == PseudoColor)
        display_data = (unsigned char *) XtCalloc(512*512, sizeof(char));

    if(display_data == NULL)
        {
        fprintf(stdout, "Not enough memory for data display.\n");
        exit(-1);
        }
    else       /*    Zero out the data buffer       */
        for(i = 0; i < 512*512; i++)
            display_data[i] = 0;

    if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED 
                     || drawarea->fileType == RASTER)
        sonar_data = (unsigned char *) XtCalloc(datasize, sizeof(char));


    if(drawarea->fileType == QMIPS)
        {


        sonar_data = (unsigned char *) XtCalloc(datasize, bytesPerPixel);
        if(sonar_data == NULL)
            {
            fprintf(stdout, "Not enough memory for data display.\n");
            perror("Reason");
            exit(-1);
            }


        if(drawarea->numImageryChannels > 2)
            {
            subbottomSize = drawarea->pixelsPerChannel * 
                                       (drawarea->numImageryChannels - 2);
            subbottom_data = (unsigned char *) XtCalloc(subbottomSize, 
                                                              bytesPerPixel);
            if(subbottom_data == NULL)
                {
                fprintf(stdout, "Not enough memory for subbottom data.\n");
                exit(-1);
                }
            }

        trailer = (unsigned char *) XtCalloc(256, sizeof(char));

        if(trailer == NULL)
            {
            fprintf(stdout, "Not enough memory for QMIPS trailer.\n");
            exit(-1);
            }
        }

    if(sonar_data == NULL)
       {
       fprintf(stdout, "Not enough memory for sonar data.\n");
       exit(-1);
       }



    /*
     *   Fill in progress monitor while reading the data.
     */

    sprintf(messageBuffer, "Reading data...");
    showProgress(drawarea, drawarea->shell, messageBuffer);

    progressWindow = XtNameToWidget(drawarea->shell, 
                                           "*StatusDialog*DrawProgress");
    XtVaGetValues(progressWindow, XmNwidth, &progressWindowWidth, NULL);
    XtVaGetValues(progressWindow, XmNheight, &progressWindowHeight, NULL);

    progressGC = XCreateGC(XtDisplay(drawarea->shell),
                                       XtWindow(progressWindow),None,NULL);

    XSetForeground(XtDisplay(drawarea->shell), progressGC,
           WhitePixelOfScreen(XtScreen(drawarea->shell)));

    XSetBackground(XtDisplay(drawarea->shell), progressGC,
           BlackPixelOfScreen(XtScreen(drawarea->shell)));

/*
fprintf(stderr, "Calculating top ping\n");
fprintf(stderr, "position is %ld\n", lseek(fp1, 0L, SEEK_CUR));
fprintf(stderr, "top ping is %d\n", drawarea->top_ping);
fprintf(stdout, "trailer:\n0 = %d, 1 = %d, 2 = %d, 3 = %d, 4 = %d, 5 = %d\n", trailer[0], trailer[1], trailer[2], trailer[3], trailer[4], trailer[5]);
fprintf(stderr, "Reading the QMIPS DATA .....\n");
fprintf(stderr, "current inbytes = %d\n", currentInbytes);
fprintf(stderr, "current inbytes = %d\n", currentInbytes);
fprintf(stderr, "current inbytes = %d, scansize = %d\n", currentInbytes, scansize);
fprintf(stdout, "scan # %d, value = %d, char value = %d\n", i, shortValue, convertData);

*/

    /*
     *   Set some variables before entering the main loop.
     *   Read data from file fp1 until the number of scans has been
     *   reached.
     */

    inbytes = 0;

    displayDataPad = (512 - datasize / data_reducer) / 2;
    display_offset = 0;

    displayDepth = DefaultDepth(XtDisplay(drawarea->shell),
                             DefaultScreen(XtDisplay(drawarea->shell)));

    /*
     *   Here the trick to create the XImage is to make sure the 
     *   bitmap_pad and bytes_per_line members are correct.  For the
     *   PseudoColor display, all is byte aligned and the width 
     *   is the same as the bytes_per_line.  For TrueColor, you still use
     *   a char array for the data, but the bitmap_pad is now 32 (int)
     *   and the bytes_per_line is 4X the data width.  I also use
     *   XPutPixel to populate the XImage in the case of TrueColor.
     */


    /*printf ("Display Depth = %d\n",displayDepth);*/
    /* switch better? */
#if 1
    if(displayDepth == 8)
        image = XCreateImage(XtDisplay(drawarea->shell),
            DefaultVisualOfScreen(XtScreen(drawarea->graphics)), 
            displayDepth, ZPixmap, 0, (char *) display_data, 512, 512, 
            8, 512);

    else if(displayDepth == 24 || displayDepth == 16 || displayDepth == 15)
        image = XCreateImage(XtDisplay(drawarea->shell),
            DefaultVisualOfScreen(XtScreen(drawarea->graphics)), 
            displayDepth, ZPixmap, 0, (char *) display_data, 512, 512, 
            32, 2048);
    else {
      fprintf (stderr,"ERROR: display depth of %d not supported",displayDepth);
      exit(EXIT_FAILURE);
    }
#endif

    assert(NULL != image);

#if 0
    fprintf(stdout, "Number of scans to read = %d\n", number_of_scans);
    fprintf(stdout, "data reducer is %d\n", data_reducer);
    fprintf(stdout, "data pad is %d\n", displayDataPad);
#endif

    for(i = 0, z = 0; i < number_of_scans; i++)
        {

        updateProgressWin(progressWindow, progressGC, progressWindowWidth, 
                                    progressWindowHeight, number_of_scans, i);

        if(drawarea->fileType == USGS)
            {
            currentInbytes = read(fp1, scans[i], headsize);
            checkForNans(i);
            currentInbytes += read(fp1, sonar_data, datasize);
            }
        else if(drawarea->fileType == QMIPS)
            {
            currentInbytes = read(fp1, sonar_data, datasize * bytesPerPixel);
            if(drawarea->numImageryChannels > 2)
                currentInbytes += read(fp1, subbottom_data, subbottomSize
                                                              * bytesPerPixel);
            currentInbytes += read(fp1, trailer, 256);
            }
        else
            currentInbytes = read(fp1, sonar_data, scansize);

        inbytes += currentInbytes;

        if(!(i % data_reducer) && currentInbytes == scansize)
            {
            for(j = 0, k = 0; j < 512; j++)
                {
                if(j > (displayDataPad - 1) && j < (512 - displayDataPad))
                    {
                    if(drawarea->bitsPerPixel == 8)
                        {
                        if(drawarea->visual->class == PseudoColor)
                             display_data[j + display_offset] = 
                                              (sonar_data[k] * 63) / 255 + 192;
                        if(drawarea->visual->class == TrueColor)
                             {
                             colorIndex = (sonar_data[k] * 49) / 255;
			     assert(image);
                             XPutPixel(image, j, 
                                    z, drawarea->grayPixels[colorIndex]);
                             }
                        }

                    if(drawarea->bitsPerPixel == 16)
                        {
                        shortValue = get_short(sonar_data, k);
                        sixteenBitData[j + display_offset] = shortValue;
/*
printf("offset = %d, real sixteen bit value = %d\n", j + display_offset, sixteenBitData[j + display_offset]);
*/
                        if(j < 256)
                            {
                            if(shortValue >> drawarea->stbdBitShift < 256)
                                convertData = (unsigned char) 
                                        (shortValue >> drawarea->stbdBitShift);
                            else
                                convertData = 255;
                            }
                        else
                            {
                            if(shortValue >> drawarea->portBitShift < 256)
                                convertData = (unsigned char) 
                                        (shortValue >> drawarea->portBitShift);
                            else
                                convertData = 255;
                            }

                        if(drawarea->visual->class == PseudoColor)
                            display_data[j + display_offset] = 
                                            (convertData * 63) / 255 + 192;

                        if(drawarea->visual->class == TrueColor)
                             {
                             colorIndex = (convertData * 49) / 255;
                             XPutPixel(image, j, 
                                    z, drawarea->grayPixels[colorIndex]);
                             }
                        }


                    k += data_reducer * bytesPerPixel;

                    }
                else
                    {
                    if(drawarea->visual->class == PseudoColor)
                        display_data[j + display_offset] =
                             BlackPixelOfScreen(XtScreen(drawarea->shell));

                    if(drawarea->visual->class == TrueColor)
                         XPutPixel(image, j, z,
                                BlackPixelOfScreen(XtScreen(drawarea->shell)));

                    }
                }
            z++;
            display_offset += 512;
            }



        /*
         *    If the amount of data available has been read, fill the
         *    remainder of the screen with black.
         */

        if(currentInbytes < scansize)
            {
            while(z < 512)
                {

                /*
                 *   This fills the rest of the screen with black for a
                 *   uniform display.
                 */

                for(j = 0; j < 512; j++)
                    {
                    if(drawarea->visual->class == PseudoColor)
                        display_data[j + display_offset] =
                             BlackPixelOfScreen(XtScreen(drawarea->shell));

                    if(drawarea->visual->class == TrueColor)
                         XPutPixel(image, j, z,
                                BlackPixelOfScreen(XtScreen(drawarea->shell)));
                    }

                display_offset += 512;
                updateProgressWin(progressWindow, progressGC, 
                                     progressWindowWidth, progressWindowHeight,
                                     number_of_scans, i);
                i += data_reducer;
                z++;
                }
            }


        }



    XFreeGC(XtDisplay(drawarea->shell), progressGC);

    XtPopdown(XtNameToWidget(drawarea->shell, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(drawarea->shell, "*StatusDialog"));

	/*
    image->bits_per_pixel = 8;
	*/

    XClearWindow(XtDisplay(drawarea->shell), XtWindow(drawarea->graphics));

    XPutImage(XtDisplay(drawarea->shell), XtWindow(drawarea->graphics),
        drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);

    XPutImage(XtDisplay(drawarea->shell), drawarea->imagePixmap,
        drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);

    if(inbytes < (scansize * number_of_scans))
        message_display(drawarea, 0);

    XtFree((char *)sonar_data);
    if(drawarea->fileType == QMIPS)
        {
        XtFree((char *)subbottom_data);
        XtFree((char *)trailer);
        }

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*PortAltOnButton")))
        showAltitude(drawarea, PORTALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*StbdAltOnButton")))
        showAltitude(drawarea, STBDALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*PortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, PORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*StbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, STBDBEAMLIMIT);

    return;
}



