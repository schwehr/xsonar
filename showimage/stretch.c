/* 
 *    Callback to accomplish histogram equalization of data displayed 
 *    in the zoom window.
 *
 */

#include     "showimage2.h"
#include "externals.h"

#define     MAX_GRAY 256            /* range of pixel values */

void stretch_image(w,client_data,callback_data)
Widget             w;
XtPointer         client_data;
XmAnyCallbackStruct         *callback_data; 
{
    struct draw *drawarea = (struct draw * ) client_data;
    GC gc;

    Widget zoomWindow, zoomArea;

    unsigned int width, height;

    void calculate_stretch();
    void message_display();

    if(fp1 == -1)
        {
        message_display(drawarea, NULL);
        return;
        }

    if(image == NULL)    
        {
        message_display(drawarea, NOSONAR);
        return;
        }

    zoomWindow = XtNameToWidget(drawarea->shell, "*ZoomWindow");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
    height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;


    if(w == XtNameToWidget(drawarea->shell, "*MainStretch"))
        {
        height = (inbytes / scansize) / data_reducer;
        width = 512;

        calculate_stretch(w, height, width, drawarea);    

        XClearWindow(XtDisplay(w), XtWindow(drawarea->graphics));
        XPutImage(XtDisplay(w), XtWindow(drawarea->graphics),
            drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);
        XPutImage(XtDisplay(drawarea->shell), drawarea->imagePixmap,
            drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);
        }


    if(w == XtNameToWidget(drawarea->shell, "*ZoomStretch"))
        {

        if(subimage != NULL)
            {

            if(drawarea->magnify < 2)
                {
                calculate_stretch(w, height, width, drawarea);    

                XPutImage(XtDisplay(zoomWindow), XtWindow(zoomArea), 
                    drawarea->zoom_gc, subimage, 0, 0, 0, 0,
                    width, height);

                XPutImage(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                    drawarea->zoom_gc, subimage, 0, 0, 0, 0, width, height);

                }
            else if(magnified_image != NULL)
                {

                width *= drawarea->magnify;
                height *= drawarea->magnify;

                calculate_stretch(w, height, width, drawarea);    

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

    return;
}


void calculate_stretch(widget, height, width, drawarea)
Widget widget;
int height, width;
struct draw *drawarea;
{
    int i, x, y;
    long pixel_value;
    int min_pixel, max_pixel;
    int histogram_dist[MAX_GRAY];
    int image_width, image_height, image_size;
    int got_min_pixel, got_max_pixel;
    int doMain, doSub, doMagnify;

    int two_pt_stretch();

    double gray_level_fraction, pixel_sum;
    float percentDone;

    Widget progressWindow;
    Widget shellWidget;
    GC progressGC;

    Dimension progressWindowWidth;
    Dimension progressWindowHeight;
    Dimension drawAmount;
    Dimension allReadyDrawn;

    char messageBuffer[50];

    void showProgress();

    /*
     *    Initialize flags which indicate the calling window.
     */

    doMain = doSub = doMagnify = 0;

    /*    
     *    Initialize histogram and sums
     */

    for (i = 0; i < MAX_GRAY; i++)
        histogram_dist[i] = 0;

    gray_level_fraction = pixel_sum = 0.0;
    got_min_pixel = got_max_pixel = 0;

    /*
     *   Fill in progress monitor while reading the data.
     *   Also determine which shell called the equalization, zoom or main.
     */

    if(widget == XtNameToWidget(drawarea->shell, "*ZoomStretch"))
         shellWidget = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    else
         shellWidget = drawarea->shell;

    sprintf(messageBuffer, "Calculating stretch...");
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

    XmUpdateDisplay(drawarea->shell);

    /*    
     *    Read input image and compute histogram
     */

    allReadyDrawn = 0;

    if(widget == XtNameToWidget(drawarea->shell, "*MainStretch"))
         doMain++;
    if(widget == XtNameToWidget(drawarea->shell, "*ZoomStretch"))
         if(drawarea->magnify == 1)
             doSub++;
         else
             doMagnify++;



    for(y = 0; y < height; y++)
        {
        for(x = 0; x < width; x++)
            {
            if(doMain)
                pixel_value = XGetPixel(image, x, y);
            if(doSub)
                pixel_value = XGetPixel(subimage, x, y);
            if(doMagnify)
                pixel_value = XGetPixel(magnified_image, x, y);

            histogram_dist[pixel_value]++;
            pixel_sum += pixel_value;

            }

        percentDone = (float) y / ((float) height * 2);
        drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

        XDrawLine(XtDisplay(drawarea->shell), XtWindow(progressWindow), 
                       progressGC, allReadyDrawn + drawAmount, 0,
                       allReadyDrawn + drawAmount, progressWindowHeight);

        XmUpdateDisplay(drawarea->shell);

        }

    allReadyDrawn = drawAmount;

    /*    
     *    Output stretched data
     */

    for(i = 192; i < MAX_GRAY; i++)
        {

        gray_level_fraction += ( (double) histogram_dist[i] * i) / pixel_sum;
/*
printf("gray fraction = %f\thist[%d] = %d\n", gray_level_fraction, i, histogram_dist[i]);
*/
        if(histogram_dist[i])
            {
            if(!got_min_pixel)
                if(gray_level_fraction > 0.1)
                    {
                    min_pixel = i;
                    got_min_pixel++;
                    }
            if(got_min_pixel && !got_max_pixel)
                {
                if(gray_level_fraction > 0.9)
                    {
                    max_pixel = i;
                    got_max_pixel++;
                    }
                }
            }

        }

	/*
    if(max_pixel > 255)
        max_pixel = 255;
		*/


    for(y = 0; y < height; y++)
        {
        for(x = 0; x < width; x++)
            {

            if(doMain)
                {
                pixel_value = two_pt_stretch(XGetPixel(image, x, y), 
                                            min_pixel, max_pixel);
                XPutPixel(image, x, y, pixel_value);
                }
            if(doMagnify)
                {
                pixel_value = two_pt_stretch(XGetPixel(magnified_image, x, y), 
                                            min_pixel, max_pixel);

                XPutPixel(magnified_image, x, y,pixel_value);
                }
            if(doSub) 
                {
                pixel_value = two_pt_stretch(XGetPixel(subimage, x, y),
                                        min_pixel, max_pixel);
                XPutPixel(subimage, x, y, pixel_value);
                }
            }
        percentDone = (float) y / ((float) height * 2);
        drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

        XDrawLine(XtDisplay(drawarea->shell), XtWindow(progressWindow), 
                      progressGC, allReadyDrawn + drawAmount, 0,
                      allReadyDrawn + drawAmount, progressWindowHeight);

        XmUpdateDisplay(drawarea->shell);

        }

    XFreeGC(XtDisplay(drawarea->shell), progressGC);

    XtPopdown(XtNameToWidget(drawarea->shell, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(drawarea->shell, "*StatusDialog"));

    return;
}

int two_pt_stretch(pixval, min_pixel, max_pixel)
long pixval;
int min_pixel, max_pixel;
{
    int outpix;
    if(pixval <= min_pixel)
        outpix = 192;
    else if(pixval < max_pixel)
        {
        outpix = (int) ((( (float) (pixval - min_pixel) /
                    (float) (max_pixel - min_pixel)) * 63.0) + 192);
        if(outpix < 192)
            outpix = 192;
        }
    else
        outpix = 255;
    
    return(outpix);
}
