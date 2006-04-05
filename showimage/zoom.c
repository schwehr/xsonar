
#include "showimage2.h" 
#include "externals.h" 
void zoom_area(w,client_data,callback_data) 
Widget                     w; 
XtPointer                  client_data; 
XmAnyCallbackStruct        *callback_data; 
{ 

	struct draw *drawarea = (struct draw *) client_data; 
    Widget zoomDialog;
    Widget zoomArea;

    Arg args[40];
    int n;
    int number_of_colormaps;

    XFontStruct *label_font;
    Atom ATOM_WM_COLMAP_WIN;
    Window drawing_window;

    XSetWindowAttributes zoomWindowAttr;    /* window attribute struct */
    unsigned long zoomWindowValueMask;

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    XtSetSensitive(XtNameToWidget(drawarea->shell, "*ZoomButton"), FALSE);
    XtPopup(zoomDialog, XtGrabNone);

    zoomWindowAttr.cursor = XCreateFontCursor(XtDisplay(drawarea->shell), 
                                                                 XC_cross);
    zoomWindowAttr.colormap = drawarea->graphics_colormap;
    zoomWindowValueMask = CWColormap | CWCursor;
    XChangeWindowAttributes(XtDisplay(zoomDialog), XtWindow(zoomDialog),
                      zoomWindowValueMask, &zoomWindowAttr);

    XChangeWindowAttributes(XtDisplay(zoomDialog), XtWindow(zoomArea),
                       zoomWindowValueMask, &zoomWindowAttr);

    drawarea->zoom_gc = XCreateGC(XtDisplay(zoomDialog), 
                                   XtWindow(zoomArea), 0, NULL);

    label_font = XLoadQueryFont(XtDisplay(drawarea->graphics),
            "-adobe-helvetica-*-r-*-*-*-120-*-*-*-*-*-*");

    XSetFont(XtDisplay(drawarea->graphics), drawarea->zoom_gc,
                                                 label_font->fid);

    XSetBackground(XtDisplay(zoomDialog), drawarea->zoom_gc,
                  BlackPixelOfScreen(XtScreen(zoomDialog)));

    XSetForeground(XtDisplay(zoomDialog), drawarea->zoom_gc,
                  WhitePixelOfScreen(XtScreen(zoomDialog)));

    XListInstalledColormaps(XtDisplay(zoomDialog),
                   XtWindow(zoomArea), &number_of_colormaps);


    return;
}


void zoom_magnify(w,client_data,callback_data)
Widget                 w;
XtPointer              client_data;
XmAnyCallbackStruct    *callback_data; 
{

     struct draw *drawarea = (struct draw * ) client_data;

     Widget zoomDialog;
     Widget zoomArea;

     unsigned char *data_ptr;

     int width, height;
     int a, b, i, j, y, z;
     int n;
     int displayDepth;
     int colorIndex;

     unsigned int magnified_width, magnified_height;

     void message_display();
     void showAltitude();
     void showBeamCorrectionLimits();

     Arg args[40];

     zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
     zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

     if(subimage == NULL)
          {
          message_display(drawarea, NOZOOM);
          return;
          }

     if(drawarea->magnify < 2)
          drawarea->magnify *= 2;
     else
          drawarea->magnify += 2;

     width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
     height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;
     magnified_height = height * drawarea->magnify;
     magnified_width = width * drawarea->magnify;

     n = 0;
     XtSetArg(args[n], XmNwidth, magnified_width); n++;
     XtSetArg(args[n], XmNheight, magnified_height); n++;
     XtSetValues(XtNameToWidget(drawarea->shell, "*ZoomFrame"), args, n);
     XtSetValues(zoomArea, args, n);

     XmUpdateDisplay(XtNameToWidget(drawarea->shell, "*ZoomDialog"));

     /*
      *   Create pixmaps for the magnified area.
      *   Fill the pixmap with black for display integrity.
      */

     XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                            BlackPixelOfScreen(XtScreen(drawarea->shell)));

     if(drawarea->zoomPixmap != (int) NULL)
         XFreePixmap(XtDisplay(drawarea->shell), drawarea->zoomPixmap);

     drawarea->zoomPixmap = XCreatePixmap(XtDisplay(drawarea->shell),
                    XtWindow(zoomArea), magnified_width, magnified_height,
                    DefaultDepth(XtDisplay(drawarea->shell),
                    DefaultScreen(XtDisplay(drawarea->shell))));


     XFillRectangle(XtDisplay(drawarea->shell), drawarea->zoomPixmap, 
                  drawarea->zoom_gc, 0, 0, magnified_width, magnified_height);

     XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                            WhitePixelOfScreen(XtScreen(drawarea->shell)));

     if(drawarea->visual->class == PseudoColor)
         zoom_data = (unsigned char *) 
                     XtCalloc(magnified_width * magnified_height, sizeof(char));

     if(drawarea->visual->class == TrueColor)
         zoom_data = (unsigned char *) 
                     XtCalloc(magnified_width * magnified_height, sizeof(int));

     if(zoom_data == NULL)
          {
          fprintf(stdout, "Unable to allocate memory for zoomed image.\n");
          exit(-1);
          }

     displayDepth = DefaultDepth(XtDisplay(drawarea->shell),
                              DefaultScreen(XtDisplay(drawarea->shell)));

     /*
      *    Create the magnified XImage pointer.....
      */

     if(drawarea->visual->class == PseudoColor)
         magnified_image = XCreateImage(XtDisplay(zoomDialog),
                    DefaultVisualOfScreen(XtScreen(zoomArea)), 
                    displayDepth, ZPixmap, 0, (char *) zoom_data,
                    magnified_width, magnified_height, 8, (int)magnified_width);

     if(drawarea->visual->class == TrueColor)
         magnified_image = XCreateImage(XtDisplay(zoomDialog),
                    DefaultVisualOfScreen(XtScreen(zoomArea)), 
                    displayDepth, ZPixmap, 0, (char *) zoom_data,
                    magnified_width, magnified_height, 32, 
                    (int)magnified_width * 4);

     for(i = 0, y = 0; i < height; i++, y += drawarea->magnify)
          for(a = 0; a < drawarea->magnify; a++)
               for(j = 0, z = 0; j < width; j++, z+=drawarea->magnify)
                    for(b = 0; b < drawarea->magnify; b++)
                         {
                         if(drawarea->visual->class == PseudoColor)
                             zoom_data[((y + a) * magnified_width) + z + b] = 
                                    sub_sample_sonar[(i * width) + j];
                         if(drawarea->visual->class == TrueColor)
                              {
                              colorIndex = 
                                 (preMagnifySonar[(i * width) + j] * 49) / 255;
                              XPutPixel(magnified_image, z + b,
                                 y + a, 
                                 drawarea->grayPixels[colorIndex]);
							  }

                         }

     /*
      *    and put the image into the zoom window and pixmap.
      */

     XPutImage(XtDisplay(zoomDialog), XtWindow(zoomArea),
                    drawarea->zoom_gc, magnified_image,
                    0, 0, 0, 0, magnified_width, magnified_height);

     XPutImage(XtDisplay(zoomDialog), drawarea->zoomPixmap,
                    drawarea->zoom_gc, magnified_image,
                    0, 0, 0, 0, magnified_width, magnified_height);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortAltOnButton")))
        showAltitude(drawarea, ZOOMPORTALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdAltOnButton")))
        showAltitude(drawarea, ZOOMSTBDALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMPORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMSTBDBEAMLIMIT);

     return;


}

     
void do_zoom(drawarea)
struct draw *drawarea;
{
     int n, i, j, k, x, y;
     int displayDataPad;
     int tempInbytes;
     int fileSize;
     int bytesPerPixel;
     int displayDepth;
     int colorIndex;

     unsigned int width, height;
     unsigned int width_offset, height_offset;
 
     unsigned char convertData;
     unsigned short shortValue;

     long position, filePosition, currentPosition;
 
     int getFileSize();              /*  in utilities.c  */
     unsigned short get_short();

     void showAltitude();
     void showBeamCorrectionLimits();

     Widget zoomDialog;
     Widget zoomArea;

     Arg args[40];

     zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
     zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

     displayDataPad = (512 - datasize / data_reducer) / 2;

     bytesPerPixel = drawarea->bitsPerPixel / 8;

     x = (drawarea->base_x - displayDataPad) * data_reducer;
     y = drawarea->base_y * data_reducer;
     width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
     height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

     /*
      * Set the width structure in drawarea for event checking
      * in events.c 
      */

     drawarea->zoomWidth = width;
     drawarea->zoomHeight = height;

     XtSetSensitive(XtNameToWidget(drawarea->shell, "*ZoomEqualize"), True);

     n = 0;
     XtSetArg(args[n], XmNwidth, width); n++;
     XtSetArg(args[n], XmNheight, height); n++;
     XtSetValues(XtNameToWidget(drawarea->shell, "*ZoomFrame"), args, n);
     XtSetValues(zoomArea, args, n);

     XmUpdateDisplay(zoomDialog);

     XClearWindow(XtDisplay(zoomDialog), XtWindow(zoomArea));
    
     /*
      *   Create pixmaps for the zoom area.
      *   Fill the pixmap with black for display integrity.
      */

     XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                                 BlackPixelOfScreen(XtScreen(drawarea->shell)));

     if(drawarea->zoomPixmap != (int) NULL)
         XFreePixmap(XtDisplay(drawarea->shell), drawarea->zoomPixmap);

     drawarea->zoomPixmap = XCreatePixmap(XtDisplay(drawarea->shell),
                         XtWindow(zoomArea), width, height,
                         DefaultDepth(XtDisplay(drawarea->shell),
                         DefaultScreen(XtDisplay(drawarea->shell))));

     XFillRectangle(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                   drawarea->zoom_gc, 0, 0, width, height);

     XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                   WhitePixelOfScreen(XtScreen(drawarea->shell)));

     /*
      *    Check all the XImage and data pointers.  Free them up if
      *    need be.  These actions are taken when the zoom window is 
      *    popped up and the user draws another rectangle in the 
      *    graphics window.
      */

     if(magnified_image != NULL)
          {
          XFree((char *)magnified_image);
          magnified_image = NULL;
          }

     if(zoom_data != NULL)
          {
          XtFree((char *)zoom_data);
          zoom_data = NULL;
          }

     if(zoomSixteenBitData != NULL)
          {
          XtFree((char *) zoomSixteenBitData);
          zoomSixteenBitData = NULL;
          }

     if(subimage != NULL)
          {
          XFree((char *)subimage);
          subimage = NULL;
          }

     if(sub_sample_sonar != NULL)
          {
          XtFree((char *)sub_sample_sonar);
          sub_sample_sonar = NULL;
          }

     if(preMagnifySonar != NULL)
          {
          XtFree((char *) preMagnifySonar);
          preMagnifySonar = NULL;
          }

     /*
      *    Check to see if the image pointer is NULL.  If so, do
      *    nothing.  Should have already checked this in events.c.
      */

     if(image != NULL)
          {
          displayDepth = DefaultDepth(XtDisplay(drawarea->shell),
                               DefaultScreen(XtDisplay(drawarea->shell)));

          if(drawarea->visual->class == PseudoColor)
              {
              sub_sample_sonar = (unsigned char *) XtCalloc(width * height, 
                                                           sizeof(char));
              subimage = XCreateImage(XtDisplay(zoomDialog),
                    DefaultVisualOfScreen(XtScreen(zoomArea)), 
                    displayDepth, ZPixmap, 0, (char *) sub_sample_sonar,
                    width, height, 8, width);
              }

          if(drawarea->visual->class == TrueColor)
              {
              sub_sample_sonar = (unsigned char *) XtCalloc(width * height, 4);
              preMagnifySonar = (unsigned char *) XtCalloc(width * height, 4);
              subimage = XCreateImage(XtDisplay(zoomDialog),
                    DefaultVisualOfScreen(XtScreen(zoomArea)), 
                    displayDepth, ZPixmap, 0, (char *) sub_sample_sonar,
                    width, height, 32, width * 4);
              }


          if(sub_sample_sonar == NULL && preMagnifySonar == NULL)
               {
               fprintf(stdout, "Not enough memory for zoom image.\n");
               return;
               }

          height_offset = scansize * (y + drawarea->top_ping);
          width_offset = headsize + x * bytesPerPixel;

          sonar_data = (unsigned char *) XtCalloc(width, bytesPerPixel);
          zoomSixteenBitData = (unsigned short *) 
              XtCalloc(width * height, sizeof(short));

          currentPosition = lseek(fp1, 0L, SEEK_CUR);

          fileSize = getFileSize(fp1);

          for(i = 0; i < height; i++)
               {
               filePosition = (long) (height_offset + (scansize * i)) 
                                                             + width_offset;
               filePosition += fileHeaderSize;
               position = lseek(fp1, filePosition, SEEK_SET);
               tempInbytes = read(fp1, sonar_data, width * bytesPerPixel);

               for(j = 0, k = 0; j < width; j++, k += 2)
                   {
                   if(drawarea->visual->class == PseudoColor)
                       {
                       if((j + width_offset) < 0 || filePosition > fileSize)
                           sub_sample_sonar[(i * width) + j] =  
                               BlackPixelOfScreen(XtScreen(drawarea->shell));
                       else if(drawarea->bitsPerPixel == 8)
                           sub_sample_sonar[(i * width) + j] = 
                                            (sonar_data[j] * 63) / 255 + 192;
                       else  /* must be 16 bit */
                           {
                           shortValue = get_short(sonar_data, k);
                           zoomSixteenBitData[(i * width) + j] = shortValue;

                           if(x + j < datasize / 2)
                               {
                               if(shortValue >> drawarea->stbdBitShift < 256)
                                   convertData = (unsigned char)
                                        (shortValue >> drawarea->stbdBitShift);
                               else
                                   convertData = 255;
                               }
                           else
                               {
                               if(shortValue >> drawarea->stbdBitShift < 256)
                                   convertData = (unsigned char)
                                        (shortValue >> drawarea->portBitShift);
                               else
                                   convertData = 255;
                               }

                           sub_sample_sonar[(i * width) + j] = 
                                                (convertData * 63) / 255 + 192;
                           }

                       }
				   

                   if(drawarea->visual->class == TrueColor)
                       {

                       if((j + width_offset) < 0 || filePosition > fileSize)
                           {
                           XPutPixel(subimage, j, i,
                                 BlackPixelOfScreen(XtScreen(drawarea->shell)));
                           preMagnifySonar[(i * width) + j] =  
                               BlackPixelOfScreen(XtScreen(drawarea->shell));
                           }
                       else if(drawarea->bitsPerPixel == 8)
                           {
                           preMagnifySonar[(i * width) + j] = sonar_data[j];
                           colorIndex = (sonar_data[j] * 49) / 255;
                           XPutPixel(subimage, j, i, 
                                   drawarea->grayPixels[colorIndex]);
                           }
                       else  /* must be 16 bit */
                           {
                           shortValue = get_short(sonar_data, k);
                           zoomSixteenBitData[(i * width) + j] = shortValue;

                           if(x + j < datasize / 2)
                               {
                               if(shortValue >> drawarea->stbdBitShift < 256)
                                   convertData = (unsigned char)
                                        (shortValue >> drawarea->stbdBitShift);
                               else
                                   convertData = 255;
                               }
                           else
                               {
                               if(shortValue >> drawarea->stbdBitShift < 256)
                                   convertData = (unsigned char)
                                        (shortValue >> drawarea->portBitShift);
                               else
                                   convertData = 255;
                               }

                           colorIndex = (convertData * 49) / 255;
                           XPutPixel(subimage, j, i, 
                                   drawarea->grayPixels[colorIndex]);
                           }
                       }



                   }
               }

          position = lseek(fp1, currentPosition, SEEK_SET);

          XPutImage(XtDisplay(zoomDialog), XtWindow(zoomArea),
               drawarea->zoom_gc, subimage, 0, 0, 0, 0, width, height);

          XPutImage(XtDisplay(zoomDialog), drawarea->zoomPixmap,
               drawarea->zoom_gc, subimage, 0, 0, 0, 0, width, height);

          }

     XtFree((char *)sonar_data);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortAltOnButton")))
        showAltitude(drawarea, ZOOMPORTALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdAltOnButton")))
        showAltitude(drawarea, ZOOMSTBDALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMPORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMSTBDBEAMLIMIT);

     return;
}

void zoom_reset(w,client_data,callback_data)
Widget                     w;
XtPointer                     client_data;
XmAnyCallbackStruct           *callback_data; 
{

     Arg args[40];
     int n;
     unsigned int width, height;

     Widget zoomDialog;
     Widget zoomArea;

     void message_display();
     void showAltitude();
     void showBeamCorrectionLimits();

     struct draw *drawarea = (struct draw * ) client_data;

     if(subimage == NULL)
          {
          message_display(drawarea, NOZOOM);
          return;
          }

     zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
     zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

     drawarea->magnify = 1;

     width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
     height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

     n = 0;
     XtSetArg(args[n], XmNwidth, width); n++;
     XtSetArg(args[n], XmNheight, height); n++;
     XtSetValues(zoomArea, args, n);

     XClearWindow(XtDisplay(zoomDialog), XtWindow(zoomArea));

/*
     if(magnified_image != NULL)
          {
          XFree((char *)magnified_image);
          XtFree((char *)zoom_data);
          magnified_image = NULL;
          }
*/

     XPutImage(XtDisplay(zoomDialog), XtWindow(zoomArea), drawarea->zoom_gc,
               subimage, 0, 0, 0, 0, width, height);

     XFreePixmap(XtDisplay(drawarea->shell), drawarea->zoomPixmap);

     drawarea->zoomPixmap = XCreatePixmap(XtDisplay(drawarea->shell), 
                    XtWindow(zoomArea), width, height,
                    DefaultDepth(XtDisplay(drawarea->shell),
                    DefaultScreen(XtDisplay(drawarea->shell))));

     XPutImage(XtDisplay(zoomDialog), drawarea->zoomPixmap, drawarea->zoom_gc,
                                    subimage, 0, 0, 0, 0, width, height);

     XtSetSensitive(XtNameToWidget(drawarea->shell, "*ZoomEqualize"), True);

     if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortAltOnButton")))
         showAltitude(drawarea, ZOOMPORTALT);

     if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdAltOnButton")))
         showAltitude(drawarea, ZOOMSTBDALT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomPortShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMPORTBEAMLIMIT);

    if(!XtIsSensitive(XtNameToWidget(drawarea->shell, "*ZoomStbdShowBeamLimitOnButton")))
        showBeamCorrectionLimits(drawarea, ZOOMSTBDBEAMLIMIT);

     return;

}

void zoom_done(w,client_data,callback_data)
Widget                     w;
XtPointer                  client_data;
XmAnyCallbackStruct        *callback_data; 
{
     
     unsigned int width, height;
     Arg args[40];
     int n;

     struct draw *drawarea = (struct draw * ) client_data;

     drawarea->magnify = 1;
     
     width = 30;
     height = 30;

     XtSetSensitive(XtNameToWidget(drawarea->shell, "*ZoomEqualize"), True);

     if(subimage != NULL)
          {
          XFree((char *)subimage);
          XtFree((char *)sub_sample_sonar);
          subimage = NULL;
          sub_sample_sonar = NULL;
          }

     if(magnified_image != NULL)
          {
          XFree((char *)magnified_image);
          XtFree((char *)zoom_data);
          magnified_image = NULL;
          zoom_data = NULL;
          }


     n = 0;
     XtSetArg(args[n], XmNwidth, width); n++;
     XtSetArg(args[n], XmNheight, height); n++;
     XtSetValues(XtNameToWidget(drawarea->shell, "*ZoomArea"), args, n);

     XFreeGC(XtDisplay(XtNameToWidget(drawarea->shell, "*ZoomDialog")), 
                                                          drawarea->zoom_gc);
     if(drawarea->zoomPixmap != (int) NULL)
         XFreePixmap(XtDisplay(drawarea->shell), drawarea->zoomPixmap);

     drawarea->zoomPixmap = (int) NULL;

     XtPopdown(XtNameToWidget(drawarea->shell, "*ZoomDialog"));
     XtSetSensitive(XtNameToWidget(drawarea->shell, "*ZoomButton"), TRUE);

     XtUnrealizeWidget(XtNameToWidget(drawarea->shell, "*ZoomDialog"));

     XmUpdateDisplay(drawarea->shell);

     return;

}
