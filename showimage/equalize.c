/* 
 *    Callback to accomplish histogram equalization of data displayed 
 *    in the zoom window.
 *
 */

#include <unistd.h>

#include "showimage2.h"
#include "externals.h"

#define     MAX_GRAY 256            /* range of pixel values */

void equalize_image(w,client_data,callback_data)
Widget                 w;
XtPointer             client_data;
XmAnyCallbackStruct        *callback_data; 
{


    struct draw *drawarea = (struct draw * ) client_data;
    /*GC gc;*/
    unsigned int width, height;

    Widget zoomWindow;
    Widget zoomArea;

    void message_display();
    void calculate_hist();
    void showAltitude();

    int displayDataPad;

     if(fp1 == -1 || fp1 == EOF)
          {
          message_display(drawarea, fp1);
          return;
          }

     if(image == NULL)
          {
          message_display(drawarea, NOSONAR);
          return;
          }

     if(subimage == NULL && 
                w == XtNameToWidget(drawarea->shell, "*ZoomEqualize"))
         {
         message_display(drawarea, NOZOOM);
         return;
         }

    zoomWindow = XtNameToWidget(drawarea->shell, "*ZoomWindow");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    displayDataPad = (512 - datasize / data_reducer) / 2;

    if(w == XtNameToWidget(drawarea->shell, "*MainEqualize"))
        {

        width  = 512;
        height = (unsigned int) (((float) inbytes / (float) scansize) 
                                             / (float) data_reducer + 0.5);

        calculate_hist(w, height, width, drawarea, displayDataPad);

        XClearWindow(XtDisplay(w), XtWindow(drawarea->graphics));

        XPutImage(XtDisplay(w), XtWindow(drawarea->graphics),
            drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);
        XPutImage(XtDisplay(drawarea->shell), drawarea->imagePixmap,
            drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);

        }

    if(w == XtNameToWidget(drawarea->shell, "*ZoomEqualize"))
        {
        width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
        height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

        if(subimage != NULL)
            {

            if(drawarea->magnify < 2)
                {
                calculate_hist(w, height, width, drawarea, displayDataPad);

                XPutImage(XtDisplay(zoomWindow), XtWindow(zoomArea), 
                    drawarea->zoom_gc, subimage, 0, 0, 0, 0, width, height);
                XPutImage(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                    drawarea->zoom_gc, subimage, 0, 0, 0, 0, width, height);
                }
            else if(magnified_image != NULL)
                {

                width *= drawarea->magnify;
                height *= drawarea->magnify;

                calculate_hist(w, height, width, drawarea, displayDataPad);

                XPutImage(XtDisplay(zoomWindow), XtWindow(zoomArea),
                    drawarea->zoom_gc, magnified_image, 0, 0, 0, 0,
                    width, height);
                XPutImage(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                    drawarea->zoom_gc, magnified_image, 0, 0, 0, 0, 
                    width, height);
                }
            else
                ;
            }
    
        }

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortAltOnButton")))
        showAltitude(drawarea, ZOOMPORTALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdAltOnButton")))
        showAltitude(drawarea, ZOOMSTBDALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*PortAltOnButton")))
        showAltitude(drawarea, PORTALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*StbdAltOnButton")))
        showAltitude(drawarea, STBDALT);

    return;
}


void calculate_hist(widget, height, width, drawarea, displayDataPad)
Widget widget;
unsigned int height, width;
struct draw *drawarea;
int displayDataPad;
{

    Widget progressWindow;
    Widget shellWidget;
    GC progressGC;

    Dimension progressWindowWidth;
    Dimension progressWindowHeight;
    Dimension drawAmount;
    Dimension allReadyDrawn;

    int h, i, j, v, x, y, z;
    int display_offset;
    int data_offset;
    int histogram_table[MAX_GRAY];
    int histogram_dist[MAX_GRAY];
    int doMain, doSub, doMagnify;
    int tempInbytes;
    int number_of_scans;
    int bytesPerPixel;

    unsigned int width_offset, height_offset;
    unsigned short shortValue;

    unsigned char convertData;

    unsigned long pixel_value;
    unsigned long equalizeValue;
    off_t currentPosition, zoomPosition, position;

    off_t lseek();

    double gray_level_fraction, pixel_sum;
    float percentDone;

    unsigned char *hist_data;

    char messageBuffer[50];

    void showProgress();
    void free();
    void *calloc();

    unsigned short get_short();

    /*
     *    Allocate memory for the histogram data.
     */

    if(drawarea->visual->class == PseudoColor)
        hist_data = (unsigned char *)  calloc(width * height, sizeof(char));

    if(drawarea->visual->class == TrueColor)
        hist_data = (unsigned char *)  calloc(width * height, sizeof(int));

    /*
     *   Fill in progress monitor while reading the data.
     *   Also determine which shell called the equalization, zoom or main.
     */

    if(widget == XtNameToWidget(drawarea->shell, "*ZoomEqualize"))
         shellWidget = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    else
         shellWidget = drawarea->shell;

    sprintf(messageBuffer, "Calculating parameters...");
    showProgress(drawarea, shellWidget, messageBuffer);

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
     *    Initialize flags which indicate the calling window.
     */

    doMain = doSub = doMagnify = 0;


    /*
     *    Initialize histogram and sums
     */

    for (i = 0; i < MAX_GRAY; i++)
        {
        histogram_table[i] = 0;
        histogram_dist[i] = 0;
        }

    gray_level_fraction = pixel_sum = 0.0;

    x = drawarea->base_x * data_reducer;
    y = drawarea->base_y * data_reducer;

    height_offset = scansize * y;
    width_offset = headsize + 
                      ((drawarea->base_x - displayDataPad) * data_reducer);

    allReadyDrawn = 0;

    if(widget == XtNameToWidget(drawarea->shell, "*MainEqualize"))
        doMain++;
    if(widget == XtNameToWidget(drawarea->shell, "*ZoomEqualize")) {
        if(drawarea->magnify == 1)
            doSub++;
        else
            doMagnify++;
    }


    bytesPerPixel = drawarea->bitsPerPixel / 8;

    if(doMain)
        {
        currentPosition = lseek(fp1, 0L, SEEK_CUR);
        position = lseek(fp1, 
             (long) (drawarea->top_ping * scansize + fileHeaderSize), SEEK_SET);

        if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED)
            sonar_data = (unsigned char *) 
                                     calloc(scansize, sizeof(unsigned char));
        if(drawarea->fileType == QMIPS)
            sonar_data = (unsigned char *) calloc(datasize, bytesPerPixel);

        tempInbytes = 0;
        data_offset = headsize;

        number_of_scans = inbytes / scansize;

        for(y = 0, z = 0; y < number_of_scans; y++)
            {
            display_offset = z * width;

            if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED)
                tempInbytes += read(fp1, sonar_data, scansize);

            if(drawarea->fileType == QMIPS)
                {
                tempInbytes += read(fp1, sonar_data, datasize * bytesPerPixel);
                position = lseek(fp1, datasize/2 * bytesPerPixel, SEEK_CUR);
                position = lseek(fp1, 256L, SEEK_CUR);
                }

            if(!(y % data_reducer))
                {
                for(x = 0, j = 0; x < width; x++, 
                    j+=data_reducer * bytesPerPixel)
                    {
                    if(drawarea->fileType == USGS || drawarea->fileType == RASTER ||
                                          drawarea->fileType == UNDEFINED)
                        hist_data[display_offset + x] = 
                                                   sonar_data[j + data_offset];

                    if(drawarea->fileType == QMIPS)
                        {
                        shortValue = get_short(sonar_data, j);
                        if(x < 256)
                            convertData = (unsigned char)
                                        (shortValue >> drawarea->stbdBitShift);
                        else
                            convertData = (unsigned char)
                                        (shortValue >> drawarea->portBitShift);


                        hist_data[display_offset + x] = convertData;
                        }
                    }
                z++;
                }


            percentDone = (float) z / ((float) height * 3);
            drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

            XDrawLine(XtDisplay(drawarea->shell), 
                        XtWindow(progressWindow), progressGC,
                        allReadyDrawn + drawAmount, 0, 
                        allReadyDrawn + drawAmount, progressWindowHeight);

            XmUpdateDisplay(drawarea->shell);
            }

        free((void *) sonar_data);

        position = lseek(fp1, currentPosition, SEEK_SET);
        }
    else
        {
        

        currentPosition = lseek(fp1, 0L, SEEK_CUR);
        position = lseek(fp1,
             (long) (drawarea->top_ping * scansize + fileHeaderSize), SEEK_SET);

        if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED)
            sonar_data = (unsigned char *)
                                     calloc(scansize, sizeof(unsigned char));
        if(drawarea->fileType == QMIPS)
            sonar_data = (unsigned char *) calloc(datasize, bytesPerPixel);

        zoomPosition = (long) (height_offset);
        position = lseek(fp1, zoomPosition, SEEK_CUR);

        for(y = 0; y < height; y+=drawarea->magnify)
            {
            if(drawarea->fileType == USGS || drawarea->fileType == UNDEFINED)
                tempInbytes = read(fp1, sonar_data, scansize);

            if(drawarea->fileType == QMIPS)
                {
                tempInbytes += read(fp1, sonar_data, datasize * bytesPerPixel);
                position = lseek(fp1, datasize/2 * bytesPerPixel, SEEK_CUR);
                position = lseek(fp1, 256L, SEEK_CUR);
                }

            for(h = 0; h < drawarea->magnify; h++)
                for(x = 0, v = 0; x < width; x+=drawarea->magnify, v += bytesPerPixel * drawarea->magnify)
                    for(i = 0; i < drawarea->magnify; i++)
                        {
                        if(tempInbytes > 0)
                            {
                            if(drawarea->fileType == USGS || 
                                drawarea->fileType == UNDEFINED)
                                hist_data[((y + h) * width) + x + i] =
                                     sonar_data[width_offset + x / 
                                     drawarea->magnify];

                            if(drawarea->fileType == QMIPS)
                                {

                                if(drawarea->magnify < 2)
                                    hist_data[((y + h) * width) + x + i] =
                                            sub_sample_sonar[y * width + x];
                                else
                                    hist_data[((y + h) * width) + x + i] =
                                            zoom_data[y * width + x];
                                           
                                }

                            }
                        else
                            hist_data[((y + h) * width) + x + i] = 0;
                        }

            percentDone = (float) y / ((float) height * 3);
            drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

            XDrawLine(XtDisplay(drawarea->shell), XtWindow(progressWindow), 
                             progressGC, allReadyDrawn + drawAmount, 0,
                             allReadyDrawn + drawAmount, progressWindowHeight);

            XmUpdateDisplay(drawarea->shell);
            }


        position = lseek(fp1, currentPosition, SEEK_SET);
        free((void *) sonar_data);

        }

     allReadyDrawn = drawAmount;

    /*    
     *    Read input image and compute histogram
     */


    for(y = 0; y < height; y++)
        {
        for(x = 0; x < width; x++)
            if(hist_data[x + y * width] > 0)
                {
                pixel_value = (unsigned long) hist_data[x + y * width];

                histogram_dist[pixel_value]++;
                pixel_sum += (double) pixel_value;
                }

        percentDone = (float) y / ((float) height * 3);
        drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

        XDrawLine(XtDisplay(drawarea->shell), XtWindow(progressWindow), 
                          progressGC, allReadyDrawn + drawAmount, 0,
                          allReadyDrawn + drawAmount, progressWindowHeight);

        XmUpdateDisplay(drawarea->shell);


        }

     allReadyDrawn *= 2;

    /*    Compute look-up table
     */

    for (i = 0; i < MAX_GRAY; i++)
        {

        gray_level_fraction += (double) (histogram_dist[i] * i) / pixel_sum;

        if(drawarea->visual->class == PseudoColor)
            {
            histogram_table[i] = (int)(0.5 + gray_level_fraction * 63.0) + 192;
            if (histogram_table[i] > 255)
                histogram_table[i] = 255;
            if(histogram_table[i] < 192)
                histogram_table[i] = 192;
            }
	
        if(drawarea->visual->class == TrueColor)
            histogram_table[i] = (int)(0.5 + gray_level_fraction * 49);

/*
printf("gray frac = %f\thist_table[%d] = %d\n", gray_level_fraction, i, histogram_table[i]);
*/

        }

    /*    output equalized data
     */

    for(y = 0; y < height; y++)
        {
        for(i = 0, x = displayDataPad; i < width; x++, i++)
            {

            pixel_value = (unsigned long) hist_data[i + y * width];
            equalizeValue = (unsigned long) histogram_table[pixel_value];

            if(drawarea->visual->class == PseudoColor)
                {
                if(equalizeValue > 255)
                    equalizeValue = 255;
                if(equalizeValue < 192)
                    equalizeValue = 192;
                }

            if(drawarea->visual->class == TrueColor)
                equalizeValue = drawarea->grayPixels[equalizeValue];


            if(doMain)
                XPutPixel(image, x, y, equalizeValue);
            if(doSub)
                XPutPixel(subimage, i, y, equalizeValue);
            if(doMagnify) 
                XPutPixel(magnified_image, i, y, equalizeValue);
            }

        percentDone = (float) y / ((float) height * 3);
        drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

        XDrawLine(XtDisplay(drawarea->shell), XtWindow(progressWindow), 
               progressGC, allReadyDrawn + drawAmount, 0, 
               allReadyDrawn + drawAmount, progressWindowHeight);

        XmUpdateDisplay(drawarea->shell);
        }

    free((void *)hist_data);

    XFreeGC(XtDisplay(drawarea->shell), progressGC);

    XtPopdown(XtNameToWidget(drawarea->shell, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(drawarea->shell, "*StatusDialog"));

    return;
}
