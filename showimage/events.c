
#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void drawarea_input(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data; 
{


    struct draw *drawarea = (struct draw * ) client_data;


    XmDrawingAreaCallbackStruct
         *input_data = (XmDrawingAreaCallbackStruct *) call_data; 

    extern int doingEvent;

    void message_display();
    void handle_buttonPress();

    if(!doingEvent)
        ;   /* Keep on going, doing nothing in here yet  */
    else
        return;  /*  Must be doing something here, get out and return */

    doingEvent = True;  /* set the event monitor so that other mouse
                           presses by a frustrated user are not processed! */

    if(image != NULL && fp1 != -1)
        {
        switch(input_data->event->xany.type)
            {
            case ButtonPress:
                handle_buttonPress(w, drawarea, input_data->event);
                break;
            case ButtonRelease:
                break;
            case MotionNotify:
                break;
            case KeyPress:
                break;
            case KeyRelease:
                break;
            default:
                break;

            }
        }
    else
        {
        if(input_data->event->type == ButtonRelease 
                         && !XtNameToWidget(drawarea->shell, "*MessageDialog"))
            {
            if(fp1 == -1)
                message_display(drawarea, fp1);
            else
                message_display(drawarea, NOSONAR);
            }
        }

    doingEvent = False;  /* reset the event monitor  */

    return;
}
    
void handle_buttonPress(w, drawarea, ev)
Widget w;
struct draw *drawarea;
XEvent * ev;
{

    XEvent next_event;    
    Arg args[40];
    GC the_rubber_GC;                 /* the graphics context */
    GC gc;
    XGCValues the_GC_values;           /* for handling the GC */

    Widget testShell;

    void do_zoom();
    void print_telem();
    void message_display();
/*
    void plot_trace();
*/

    int i, n;
    int status;
    int fore;
    int startx, starty;
    int motionCount;
    int printingTelem;
    int anchor_x, anchor_y, lastx, lasty;

    int num_scans;
    int ping;

    unsigned int oldRecWidth, oldRecHeight;
    unsigned int newRecWidth, newRecHeight;

    long pixelValue;
    long pixelIndex;

    float metersPerPix;

    char messageBuf[512];



/*
    if(drawarea->fileType == QMIPS &&
        w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
        {
        XSync(XtDisplay(w), 1);
        return;
        }
*/

    if(w == drawarea->graphics)
        status = XGrabPointer(XtDisplay(w), XtWindow(w), True,
            ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, XtWindow(w),
            XCreateFontCursor(XtDisplay(drawarea->graphics), XC_dotbox),
            CurrentTime);

    if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
        status = XGrabPointer(XtDisplay(w), XtWindow(w), True,
            ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, XtWindow(w),
            XCreateFontCursor(XtDisplay(drawarea->graphics), XC_dotbox),
            CurrentTime);


/*
printf("return status from grab is %d\n", status);
printf("Window status of XtWindow(w) is %d\n", (unsigned int)XtWindow(w));
printf("Window status of XtWindow(graphics) is %x\n", XtWindow(drawarea->graphics));
*/

    if(w == drawarea->graphics)
        gc = drawarea->main_gc;
    if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
        gc = drawarea->zoom_gc;

/*
    This next part establishes a rubber-band GC. 
*/

    n = 0;
    XtSetArg(args[n], XmNforeground, &the_GC_values.foreground); n++;
    XtSetArg(args[n], XmNbackground, &the_GC_values.background); n++;
    XtGetValues(w, args, n);
    the_GC_values.function = GXxor;

    the_rubber_GC = XCreateGC(XtDisplay(drawarea->shell),
            XtWindow(drawarea->graphics), GCFunction |GCForeground| 
            GCBackground, &the_GC_values);

    if(drawarea->visual->class == TrueColor)
        {
        XSetForeground(XtDisplay(drawarea->shell), the_rubber_GC, 
             WhitePixelOfScreen(XtScreen(drawarea->shell)));

        XSetBackground(XtDisplay(drawarea->shell), the_rubber_GC,
             WhitePixelOfScreen(XtScreen(drawarea->shell)));
        }
    else
        {
        XSetForeground(XtDisplay(drawarea->shell), the_rubber_GC, 192);

        XSetBackground(XtDisplay(drawarea->shell), the_rubber_GC, 255);
        }

    XSetLineAttributes(XtDisplay(w),the_rubber_GC, 2,
                LineSolid, CapButt, JoinRound);
    XSetLineAttributes(XtDisplay(w),gc, 1,
                LineSolid, CapButt, JoinRound);

    switch(ev->xbutton.button)
    {
    default:
        break;
    case Button1:
        anchor_x = ev->xbutton.x;
        anchor_y = ev->xbutton.y;
        lastx = anchor_x;
        lasty = anchor_y;

        num_scans = inbytes / scansize;


        if(drawarea->fileType == QMIPS)
            {
            if(w == drawarea->graphics)
                pixelValue = sixteenBitData[ev->xbutton.y * 512 + 
                   ev->xbutton.x];
            if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                pixelValue = zoomSixteenBitData[ ev->xbutton.y * 
                   drawarea->zoomWidth + ev->xbutton.x];
            }
        else if(drawarea->visual->class == PseudoColor)
            {
            if(w == drawarea->graphics)
                pixelValue = 
                    XGetPixel(image, ev->xbutton.x, ev->xbutton.y) - 192;

            if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                pixelValue = 
                    XGetPixel(subimage, ev->xbutton.x, ev->xbutton.y) - 192;
            pixelValue = (int) ((float)pixelValue / 63.0 * 255.0);
            }
        else /* Must be TrueColor */
            {
            if(w == drawarea->graphics)
                pixelValue = 
                    XGetPixel(image, ev->xbutton.x, ev->xbutton.y);

            if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                pixelValue = 
                    XGetPixel(subimage, ev->xbutton.x, ev->xbutton.y);

            for(i = 0; i < 50; i++)
                if(pixelValue == drawarea->grayPixels[i])
                    pixelIndex = i;

            pixelValue = pixelIndex * 255 / 49;
            }

        for(i = 0; i < 512; i++)
            sprintf(&messageBuf[i], " ");

        if(drawarea->fileType != UNDEFINED && drawarea->fileType != RASTER)
            {
            metersPerPix = (float)swath_width / datasize * data_reducer;

            if(w == drawarea->graphics)
                {
                ping = ev->xbutton.y * data_reducer;

                if(ping > num_scans)
                    {
      /*
       *  Check to see if the message display shell has been realized.  If so,
       *  do not pop up another one, just return.
       */

                    XUngrabPointer(XtDisplay(drawarea->graphics), CurrentTime);

                    XFreeGC(XtDisplay(drawarea->graphics), 
                                                the_rubber_GC);

                    testShell = XtNameToWidget(drawarea->shell, 
                        "*MessageDialog");

                    if(testShell == NULL)
                        message_display(drawarea, NODATA);

                    XCopyArea(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                           XtWindow(drawarea->graphics), drawarea->main_gc,
                           0, 0, 512, 512, 0, 0);

                    return;
                    }

                sprintf(messageBuf,
                   "X = %.3d  Y = %.3d DN = %.3d Range = %5.1f Scan number = %d  ",
                    ev->xbutton.x, ev->xbutton.y, pixelValue,
                    (float) abs(ev->xbutton.x -256) * metersPerPix,
		    ping + drawarea->top_ping);

                XDrawImageString(XtDisplay(drawarea->graphics),
                    XtWindow(drawarea->graphics), gc, 0, 512,
                    messageBuf, 512);
                }
            if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                {
                if(ev->xbutton.x < drawarea->zoomWidth) 
                    sprintf(messageBuf,"X = %.3d  Y = %.3d DN = %.3d",
                        ev->xbutton.x, ev->xbutton.y, pixelValue);

                XDrawImageString(XtDisplay(drawarea->graphics),
                    XtWindow(w), gc, 0, drawarea->zoomHeight,
                    messageBuf, strlen(messageBuf));
                }

            }
        else
            {
            sprintf(messageBuf, "X = %.3d  Y = %.3d DN = %.3d",
                 ev->xbutton.x, ev->xbutton.y, pixelValue);

            XDrawImageString(XtDisplay(drawarea->graphics),
                 XtWindow(drawarea->graphics), gc, 0, 512,
                messageBuf, 512);
/*                messageBuf, strlen(messageBuf));
*/
            }

        while(1)
            {
            XNextEvent(XtDisplay(w),&next_event);

            switch(next_event.type)
                {
                case  ButtonRelease:
                    if (next_event.xbutton.button == Button1)
                        {
/*    
                              if(XtIsRealized(zoomDialog) &&
                                                  w == drawarea->graphics)
                                   plot_trace(next_event.xbutton.y);
                              else if(w == drawarea->graphics)
                                   {
                                   printf("no trace dialog\n");
                                   message_display(9);
                                   }
                              else
                                   ;
*/

                        XUngrabPointer(XtDisplay(drawarea->graphics),
                                                    CurrentTime);

                        XFreeGC(XtDisplay(drawarea->graphics), 
                                                the_rubber_GC);

                        XCopyArea(XtDisplay(drawarea->shell), 
                           drawarea->imagePixmap,
                           XtWindow(drawarea->graphics), drawarea->main_gc,
                           0, 0, 512, 512, 0, 0);

                        return;
                        }
                    else
                        ;
                    break;
                case MotionNotify:


                    if(drawarea->fileType == QMIPS)
                        {
                        if(w == drawarea->graphics)
                            pixelValue = sixteenBitData[next_event.xbutton.y 
                            * 512 + next_event.xbutton.x];
                        if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                            pixelValue = 
                                zoomSixteenBitData[next_event.xbutton.y 
                                * drawarea->zoomWidth + next_event.xbutton.x];
                        }
                    else if(drawarea->visual->class == PseudoColor)
                        {
                        if(w == drawarea->graphics)
                            pixelValue = XGetPixel(image, next_event.xbutton.x,
                            next_event.xbutton.y) - 192;
                        if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                            pixelValue = XGetPixel(subimage, 
                            next_event.xbutton.x, next_event.xbutton.y) - 192;
                        pixelValue = (int) ((float)pixelValue / 63.0 * 255.0);
                        }
                    else
                        {
                        if(w == drawarea->graphics)
                            pixelValue = XGetPixel(image, 
                                next_event.xbutton.x, next_event.xbutton.y);

                        if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                            pixelValue = XGetPixel(subimage, 
                                next_event.xbutton.x, next_event.xbutton.y);

                        for(i = 0; i < 50; i++)
                            if(pixelValue == drawarea->grayPixels[i])
                                pixelIndex = i;

                        pixelValue = pixelIndex * 255 / 49;
                        }

                    for(i = 0; i < 512; i++)
                        sprintf(&messageBuf[i], " ");

                    if(drawarea->fileType != UNDEFINED &&
                        drawarea->fileType != RASTER)
                        {
                        metersPerPix = (float)swath_width / 
                                               datasize * data_reducer;

                        if(w == drawarea->graphics)
                            {
                            ping = next_event.xbutton.y * data_reducer;

                            if(ping > num_scans)
                                {
      /*
       *  Check to see if the message display shell has been realized.  If so,
       *  do not pop up another one, just return.
       */
        
                                XUngrabPointer(XtDisplay(drawarea->graphics), 
                                    CurrentTime);

                                XFreeGC(XtDisplay(drawarea->graphics), 
                                                the_rubber_GC);

                                testShell = XtNameToWidget(drawarea->shell, 
                                    "*MessageDialog");
                                if(testShell == NULL)
                                    message_display(drawarea, NODATA);
                                testShell = XtNameToWidget(drawarea->shell, 
                                    "*MessageDialog");
                                if(testShell == NULL)
                                    message_display(drawarea, NODATA);

                                XCopyArea(XtDisplay(drawarea->shell), 
                                   drawarea->imagePixmap,
                                   XtWindow(drawarea->graphics), 
                                   drawarea->main_gc,
                                   0, 0, 512, 512, 0, 0);

                                return;
                                }

                            sprintf(messageBuf,
                                "X = %.3d  Y = %.3d DN = %.3d Range = %5.1f Scan number = %d  ",
                                next_event.xbutton.x, next_event.xbutton.y,
                                pixelValue, (float) abs(next_event.xbutton.x 
                                - 256) * metersPerPix, 
                                ping + drawarea->top_ping);
                            XDrawImageString(XtDisplay(drawarea->graphics),
                                XtWindow(drawarea->graphics), gc, 0, 512,
                                messageBuf, 512);

                            }
                        if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
                            {
                            if(next_event.xbutton.x < drawarea->zoomWidth) 
                                sprintf(messageBuf,
                                    "X = %.3d  Y = %.3d DN = %.3d   ",
                                    next_event.xbutton.x, next_event.xbutton.y,
                                    pixelValue);
                            XDrawImageString(XtDisplay(w), XtWindow(w), gc, 0, 
				drawarea->zoomHeight,
                                messageBuf, strlen(messageBuf));

                            }

                        }
                    else
                        {
                        sprintf(messageBuf, "X = %.3d  Y = %.3d DN = %.3d   ",
                             next_event.xbutton.x, next_event.xbutton.y,
                             pixelValue);

                        XDrawImageString(XtDisplay(drawarea->graphics),
                             XtWindow(drawarea->graphics), gc, 0, 512,
                             messageBuf, 512);
/*                           messageBuf, strlen(messageBuf));
*/
                        }

                    break;
                default:
                    break;
                }

            }
        break;
    case Button2:
        anchor_x = ev->xbutton.x;
        anchor_y = ev->xbutton.y;
        lastx = anchor_x;
        lasty = anchor_y;
        motionCount = 0;
        while(1)
            {
            XNextEvent(XtDisplay(w),&next_event);

            switch(next_event.type)
                {
                case  ButtonRelease:

                    if (next_event.xbutton.button == Button2)
                        {

                        /*
                         *  The next two if's are redundant and should
                         *  never happen, however XWin32 for the Wintel
                         *  machines does not properly restrict the
                         *  to the grabbed window .....!  2/18/98  bwd

                        if(next_event.xbutton.y < 0)
                            next_event.xbutton.y = 0;

                        if(next_event.xbutton.y > 511)
                            next_event.xbutton.y = 511;
                         */

                        if(anchor_x > lastx && anchor_y > lasty)
                            {
                            drawarea->base_x = next_event.xbutton.x;
                            drawarea->zoom_x = anchor_x;
                            drawarea->base_y = next_event.xbutton.y;
                            drawarea->zoom_y = anchor_y;
                            newRecWidth = anchor_x - next_event.xbutton.x;
                            newRecHeight = anchor_y - next_event.xbutton.y;
                            XDrawRectangle(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,lastx,lasty,
                                anchor_x - lastx,anchor_y - lasty);
                            }
                        else if(anchor_x < lastx && anchor_y > lasty)
                            {
                            drawarea->base_x = anchor_x;
                            drawarea->zoom_x = next_event.xbutton.x;
                            drawarea->base_y = next_event.xbutton.y;
                            drawarea->zoom_y = anchor_y;
                            newRecWidth = next_event.xbutton.x - anchor_x;
                            newRecHeight = anchor_y - next_event.xbutton.y;
                            XDrawRectangle(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,lasty,
                                lastx - anchor_x,anchor_y - lasty);
                            }
                        else if(anchor_x > lastx && anchor_y < lasty)
                            {
                            drawarea->base_x = next_event.xbutton.x;
                            drawarea->zoom_x = anchor_x;
                            drawarea->base_y = anchor_y;
                            drawarea->zoom_y = next_event.xbutton.y;
                            newRecWidth = anchor_x - next_event.xbutton.x;
                            newRecHeight = next_event.xbutton.y - anchor_y;
                            XDrawRectangle(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,lastx,anchor_y,
                                anchor_x - lastx,lasty - anchor_y);
                            }
                        else
                            {
                            drawarea->base_x = anchor_x;
                            drawarea->zoom_x = next_event.xbutton.x;
                            drawarea->base_y = anchor_y;
                            drawarea->zoom_y = next_event.xbutton.y;
                            newRecWidth = next_event.xbutton.x - anchor_x;
                            newRecHeight = next_event.xbutton.y - anchor_y;
                            XDrawRectangle(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,anchor_y,
                                lastx - anchor_x, lasty - anchor_y);
                            }


                        XFreeGC(XtDisplay(drawarea->graphics), 
                                                the_rubber_GC);

                        if(newRecWidth < 1 || newRecHeight < 1)
                            {
                            XUngrabPointer(XtDisplay(drawarea->graphics),
                                                    CurrentTime);
                            return;
                            }

                        XDrawRectangle(XtDisplay(w),XtWindow(w),
                            gc,drawarea->base_x,drawarea->base_y,
                            newRecWidth, newRecHeight);

                        XDrawRectangle(XtDisplay(w),drawarea->imagePixmap,
                            gc,drawarea->base_x,drawarea->base_y,
                            newRecWidth, newRecHeight);

                        drawarea->magnify = 1;

                        XUngrabPointer(XtDisplay(drawarea->graphics),
                                                    CurrentTime);

     /*
      *  Check to see if the message display shell has been realized.  If so,
      *  do not pop up another one.
      */

                        testShell = XtNameToWidget(drawarea->shell, 
                                                           "*MessageDialog");
                        if(!XtIsSensitive(XtNameToWidget(drawarea->shell, 
                            "*ZoomButton")) && w == drawarea->graphics)
                            do_zoom(drawarea);
                        else if(w == drawarea->graphics && testShell == NULL)
                            message_display(drawarea, NOZOOMWIN);
                        else
                            ;

                        return;

                        }


                case MotionNotify:
                
                    if(next_event.xmotion.state == Button2MotionMask ||
                        next_event.xmotion.state == Button2MotionMask +
                        EnterWindowMask)
                        {
                        /*
                         *  The next two if's are redundant and should
                         *  never happen, however XWin32 for the Wintel
                         *  machines does not properly restrict the
                         *  to the grabbed window .....!  2/18/98  bwd

                        if(next_event.xmotion.y < 0)
                            next_event.xmotion.y = 0;

                        if(next_event.xmotion.y > 511)
                            next_event.xmotion.y = 511;
                         */

                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,anchor_y,
                                anchor_x,lasty);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,lasty,
                                lastx,lasty);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x, anchor_y,
                                lastx,anchor_y);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,lastx,lasty,
                                lastx,anchor_y);

                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,anchor_y,
                                anchor_x,next_event.xmotion.y);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x,next_event.xmotion.y,
                                next_event.xmotion.x,next_event.xmotion.y);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,anchor_x, anchor_y,
                                next_event.xmotion.x,anchor_y);
                         XDrawLine(XtDisplay(w),XtWindow(w),
                                the_rubber_GC,
                                next_event.xmotion.x,next_event.xmotion.y,
                                next_event.xmotion.x,anchor_y);

                        lastx = next_event.xmotion.x;
                        lasty = next_event.xmotion.y;
             
                        }

                    break;

                }

            }

        break;
    case Button3:

        if(w == XtNameToWidget(drawarea->shell, "*ZoomArea"))
            {
            XUngrabPointer(XtDisplay(drawarea->graphics), CurrentTime);
            break; /* get out for now, may enable this for something else */
            }

        printingTelem = True;

        while(printingTelem)
            {
            XNextEvent(XtDisplay(w),&next_event);


            if(w == drawarea->graphics)
                {

                if(drawarea->fileType == UNDEFINED 
                    || drawarea->fileType == RASTER)
                    {
                    printingTelem = False;
                    XUngrabPointer(XtDisplay(drawarea->graphics), CurrentTime);
                    }
                else if(XtIsSensitive(XtNameToWidget(drawarea->shell, 
                                                         "*TelemetryButton")))
                    {
     /*
      *  Check to see if the message display shell has been realized.  If so,
      *  do not pop up another one.
      */
    
                    testShell = XtNameToWidget(drawarea->shell, 
                                                            "*MessageDialog");

                    XUngrabPointer(XtDisplay(drawarea->graphics), CurrentTime);

                    if(testShell == NULL)
                        message_display(drawarea, NOTELEM);

                    printingTelem = False;
                    }
                else
                    {
                    switch(next_event.type)
                        {
/*
                        case  MotionNotify:
                            print_telem(next_event.xmotion.x,
                                            next_event.xmotion.y, drawarea);
                            break;
*/
                
                        case ButtonRelease:

                            XUngrabPointer(XtDisplay(drawarea->graphics), 
                                                                  CurrentTime);
                            print_telem(next_event.xbutton.x,
                                            next_event.xbutton.y, drawarea);

                            printingTelem = False;
                            break;

                        } /* end of switch */
                    }  /* end of else  */

               }  /*  end of if */

            }  /* end of while  */


        XFreeGC(XtDisplay(drawarea->graphics), the_rubber_GC);
                            

        break;
     }

    return;

}

