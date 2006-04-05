
#include "showimage2.h"
#include "externals.h"

void flip_color_map(w,client_data,callback_data)
Widget       w;
XtPointer    client_data;
XtPointer    callback_data; 
{


    struct draw *drawarea = (struct draw * ) client_data;

    XSetWindowAttributes main_window_attr;    /* window attribute struct */
    unsigned long main_window_valuemask;    /* used to select colormap */
    int gray_value;                    /* used to invert colormap */
    int starting_gray_value;
    unsigned long cell;
    unsigned short intensity;
    unsigned short red_value, green_value, blue_value;

    /*XColor tempGrayMap[50];*/   /* for the TrueColor Visual colormap flipping */
    /*int invertCell;*/         /* temp counter for TrueColor colormap invert */

    int i, j;
    int currentCell, newCell;
    unsigned long pixVal;

    int width, height;
    int doMain, doSub, doMagnify;

    void showAltitude();
    void showBeamCorrectionLimits();

    XImage *flipImage;
    Widget zoomArea;

    /*
    *    PseudoColor 
    *
    *    flip the color cells in the installed gray map.    
    *    Start at cell (pixel) 192 to preserve the lower 192 color entries.
    *    This will keep the display from going crazy.
    *    Get the starting gray value to determine which direction
    *    that the gray value will get incremented (+ or -).
    */

    if(drawarea->visual->class == PseudoColor)
        {
        red_value = drawarea->gray[255].red;
        green_value = drawarea->gray[255].green;
        blue_value = drawarea->gray[255].blue;

        starting_gray_value = (int)(red_value / 255);

        if(starting_gray_value > 255)
            starting_gray_value = 255;

        gray_value = starting_gray_value;
        for(cell = 192; cell < 256; cell++)
            {
            /*
             *    Insure that the high and low values are set to
             *    intensity limits for an 8 plane system.
             */

            if(cell == 255 && starting_gray_value == 255)
                gray_value = 0;
            if(cell == 255 && starting_gray_value == 0)
                gray_value = 255;

            intensity = gray_value * 65535L / 255L;
            drawarea->gray[cell].red = drawarea->gray[cell].green = 
                drawarea->gray[cell].blue = intensity;    
            drawarea->gray[cell].pixel = cell;
            drawarea->gray[cell].flags = DoRed|DoGreen|DoBlue;
            if(starting_gray_value < 254)
                gray_value+=4;
            else
                gray_value-=4;
            }
        
    /*        
     *   Install the colormap.  It is specific only to this application.
     */

        XStoreColors(XtDisplay(drawarea->shell), drawarea->graphics_colormap,
                                drawarea->gray, 256);

        main_window_attr.colormap = drawarea->graphics_colormap;    
        main_window_valuemask = CWColormap;

        XChangeWindowAttributes(XtDisplay(drawarea->shell), 
               XtWindow(drawarea->shell), main_window_valuemask, 
               &main_window_attr);

        }

    /*
    *    TrueColor 
    *
    *    flip the color cells in the installed gray map.    
    *
    */

    doMain = doSub = doMagnify = 0;

    if(w == XtNameToWidget(drawarea->shell, "*flipColorMap"))
        doMain++;
    if(w == XtNameToWidget(drawarea->shell, "*ZoomFlipColorButton")) {
        if(drawarea->magnify == 1)
            doSub++;
        else
            doMagnify++;
    }

    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    if(drawarea->visual->class == TrueColor)
        {
        if(doMain)
            {
            if(image == NULL)
                {
                message_display(drawarea, NOSONAR);
                return;
                }


            flipImage = image;
            width = 512; 
            height = 512; 
            }

        if(doSub)
            {
            if(subimage == NULL)
                {
                message_display(drawarea, NOZOOM);
                return;
                }

            flipImage = subimage;
            width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
            height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;
            }

        if(doMagnify)
            {
            flipImage = magnified_image;
            width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
            height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;
            height *= drawarea->magnify;
            width *= drawarea->magnify;
            }

        for(i = 0; i < height; i++)
            for(j = 0; j < width; j++)
               {
               currentCell = 0;
               pixVal = XGetPixel(flipImage, j, i);

               while(drawarea->grayMap[currentCell].pixel != pixVal)
                   currentCell++;

               newCell = 49 - currentCell;

               XPutPixel(flipImage, j, i, drawarea->grayPixels[newCell]);
               }
	
        if(doMain)
            {
            XPutImage(XtDisplay(drawarea->shell), XtWindow(drawarea->graphics),
                  drawarea->main_gc, flipImage, 0, 0, 0, 0, width, height);

            XPutImage(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                  drawarea->main_gc, flipImage, 0, 0, 0, 0, width, height);
            }

        if(doSub || doMagnify)
            {
            XPutImage(XtDisplay(drawarea->shell), XtWindow(zoomArea),
                  drawarea->main_gc, flipImage, 0, 0, 0, 0, width, height);

            XPutImage(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                  drawarea->main_gc, flipImage, 0, 0, 0, 0, width, height);
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

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*PortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, PORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*StbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, STBDBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMPORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMSTBDBEAMLIMIT);

    return;
}
