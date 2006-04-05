
#include "showimage2.h"
#include "sonar.h"
#include "externals.h"
#define TRUE 1
#define FALSE 0

           
void traceBeamLimits(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data; 
{


     struct draw *drawarea = (struct draw * ) client_data;

     void processButtonPress();
     void processButtonMotion();
     void processButtonRelease();
     void interpolateBeamPos();
     void message_display();

     Widget zoomArea;

     float calculateFirstBeamPos();
     float calculateBeamPos();

     int y, z;
     int status;
     int correcting;
     int accel_num, accel_denom, thresh;
     int first_scan, current_scan, last_scan;
     int file_index;
     int currentPosition, dist_to_nadir;
     unsigned int zoomheight;

     float zoom_alt_to_pixel;
     float position;
     float *beamTrace;

     unsigned long lastx, lasty, anchor_x, anchor_y;
     XEvent next_event;

     if((currentPosition = lseek(fp1, 0L, SEEK_CUR)) == 0)
          {
          message_display(drawarea, NOSONAR);
          return;
          }

     if(fp1 == -1)
          {
          message_display(drawarea, fp1);
          return;
          }

     if(subimage == NULL)
          {
          message_display(drawarea, NOZOOM);
          return;
          }

     if(w ==  XtNameToWidget(drawarea->shell, "*ZoomBeamStbdButton") && 
         drawarea->base_x > 256)
         {
         message_display(drawarea, NOSTBDIMAGE);
         return;
         }

     if(w ==  XtNameToWidget(drawarea->shell, "*ZoomBeamPortButton") && 
         drawarea->zoom_x < 256)
         {
         message_display(drawarea, NOPORTIMAGE);
         return;
         }

     zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

     XmUpdateDisplay(drawarea->shell);

     XSelectInput(XtDisplay(w),XtWindow(zoomArea),
         Button1MotionMask| Button2MotionMask|Button3MotionMask|
         ExposureMask|ButtonPressMask| ButtonReleaseMask);

     status = XGrabPointer(XtDisplay(w), XtWindow(zoomArea), FALSE,
         ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, XtWindow(zoomArea),
         XCreateFontCursor(XtDisplay(zoomArea), XC_pencil), CurrentTime);

     correcting = TRUE;

     XGetPointerControl(XtDisplay(zoomArea), &accel_num, &accel_denom, &thresh);
     XChangePointerControl(XtDisplay(zoomArea), True, True, 1, 4, 4);

/*
     printf("acceleration numerator = %d\n", accel_num);
     printf("acceleration denomenator = %d\n", accel_denom);
     printf("threshold = %d\n", thresh);
*/


     zoom_alt_to_pixel = 1 / ((float)swath_width / datasize);
     zoomheight = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;
     dist_to_nadir = (256 - drawarea->base_x) * data_reducer;
     
     beamTrace = (float *) XtCalloc(zoomheight, sizeof(float));

     if(beamTrace == NULL)
          printf("allocation failed for beam limits\n");

     if(drawarea->magnify > 1)
          {
          dist_to_nadir *= drawarea->magnify;
          zoom_alt_to_pixel *= drawarea->magnify;
          }

     if(w ==  XtNameToWidget(drawarea->shell, "*ZoomBeamPortButton"))
         {
         for(y = drawarea->base_y * data_reducer, z= 0; 
                        z < zoomheight && y < inbytes / scansize; y++, z++)
             beamTrace[z] = scans[y]->portBeamLimit;
         }

     if(w ==  XtNameToWidget(drawarea->shell, "*ZoomBeamStbdButton"))
         {
         for(y = drawarea->base_y * data_reducer, z= 0;
                        z < zoomheight && y < inbytes / scansize; y++, z++)
             beamTrace[z] = scans[y]->stbdBeamLimit;
         }



/*
printf("zoomheight = %d\n", zoomheight);
fprintf(stderr, "new alts %d = %.1f\n", z, new_alts[z]);
printf("zoom alt to pixel = %f\n", zoom_alt_to_pixel);
printf("distance to nadir = %d\n", dist_to_nadir);
printf("base x = %d, base y = %d\n", drawarea->base_x,drawarea->base_y);
printf("zoom x = %d, zoom y = %d\n", drawarea->zoom_x,drawarea->zoom_y);
printf("y = %d, z = %d\n", --y, --z);
fprintf(stdout, "anchor x = %lu\n", anchor_x);

*/

     while(correcting)
          {
          XNextEvent(XtDisplay(zoomArea),&next_event);

          if(subimage != NULL)
               {
               switch(next_event.type)
                   {
                   case ButtonPress:

                        /*
                         *  The next if is redundant and should
                         *  never happen, however XWin32 for the Wintel
                         *  machines does not properly restrict the
                         *  to the grabbed window .....!  2/18/98  bwd
                         */

			   /*
                        if(next_event.xbutton.y < 0)
                            next_event.xbutton.y = 0;
			    */

                        processButtonPress(zoomArea, drawarea, &next_event,
                                   &anchor_x, &anchor_y, &lastx, &lasty);


                        if(next_event.xbutton.button == Button1)
                            {
                            position = calculateFirstBeamPos(w, &file_index,
                                             &first_scan, &current_scan,
                                             &last_scan, anchor_x, anchor_y,
                                             dist_to_nadir, zoom_alt_to_pixel,
                                             drawarea);

                            beamTrace[anchor_y / drawarea->magnify] = position;
 
                            }
                        break;

                   case ButtonRelease:

                       if(next_event.xbutton.y < 0 || next_event.xbutton.y 
                           >= zoomheight * drawarea->magnify)
                           break;

                        processButtonRelease(zoomArea, drawarea, &next_event,
                                                   &lastx, &lasty, &correcting);

                        if(next_event.xbutton.button == Button1)
                              {
                              position = calculateBeamPos(w, &current_scan, lastx,
                                                  lasty, dist_to_nadir,
                                                  zoom_alt_to_pixel, drawarea);


                              beamTrace[lasty / drawarea->magnify] = position;
                              }

                        if(next_event.xbutton.button == Button3)
                              {

                              for(y = drawarea->base_y * data_reducer, z= 0;
                                  z < zoomheight && y < inbytes / scansize; 
                                  y++, z++)
                                  {
                                  if(w == XtNameToWidget(drawarea->shell, 
                                               "*ZoomBeamPortButton"))
                                      scans[y]->portBeamLimit = beamTrace[z];
                                  if(w == XtNameToWidget(drawarea->shell, 
                                               "*ZoomBeamStbdButton"))
                                      scans[y]->stbdBeamLimit = beamTrace[z];
                                  }

                              }
                                   
                        break;

                    case MotionNotify:

                       if(next_event.xmotion.y < 0 || next_event.xmotion.y >= 
                           zoomheight * drawarea->magnify)
                           break;


                        processButtonMotion(zoomArea, drawarea, &next_event,
                                                        &lastx, &lasty);

                        if(next_event.xmotion.state == Button1MotionMask ||
                            next_event.xmotion.state == Button1MotionMask +
                            EnterWindowMask)
                              {
                              position = calculateBeamPos(w, &current_scan, lastx,
                                             lasty, dist_to_nadir,
                                             zoom_alt_to_pixel, drawarea);

                              if(current_scan > last_scan)
                                   beamTrace[lasty / drawarea->magnify] = position;

                              if(current_scan - last_scan > 1)
                                   interpolateBeamPos(current_scan, last_scan,
                                        beamTrace, lasty / drawarea->magnify);     

                              last_scan = current_scan;

                              }
                        break;
                    case KeyPress:
/*
                        XSync(XtDisplay(zoomArea), True);
*/
                        break;
                    case KeyRelease:
/*
                        XSync(XtDisplay(zoomArea), True);
*/
                        break;
                    default:
                         break;

                    }
               }
          else
               {

               XUngrabPointer(XtDisplay(zoomArea), CurrentTime);
               message_display(NOZOOM, drawarea);
               XSync(XtDisplay(zoomArea), False);
               correcting = FALSE;

               }
          }

     free(beamTrace);

     XChangePointerControl(XtDisplay(zoomArea), True, True, accel_num, accel_denom, thresh);

     return;
}
    
void processButtonPress(w, drawarea, ev, anchor_x, anchor_y, lastx, lasty)
Widget w;
struct draw *drawarea;
XEvent * ev;
unsigned long *anchor_x, *anchor_y, *lastx, *lasty;
{

     long pixel_value;

     switch(ev->xbutton.button)
          {
          case Button1:
/*
               pixel_value = XGetPixel(subimage, ev->xbutton.x, ev->xbutton.y);
               printf("pixel value = %ld, x = %d, y = %d\n",
                         pixel_value, ev->xbutton.x, ev->xbutton.y);
*/
               *anchor_x = ev->xbutton.x;
               *anchor_y = ev->xbutton.y;
               *lastx = *anchor_x;
               *lasty = *anchor_y;
               break;
          case Button2:
               break;
          case Button3:
               break;
          default:
               break;
          }
}

void processButtonMotion(w, drawarea, ev, lastx, lasty)
Widget w;
struct draw *drawarea;
XEvent *ev;
unsigned long *lastx, *lasty;
{

     switch(ev->xmotion.state)
          {
          case Button1MotionMask:
               XDrawLine(XtDisplay(w),XtWindow(w),
                         drawarea->zoom_gc, *lastx, *lasty,
                         ev->xmotion.x, ev->xmotion.y);
               XDrawLine(XtDisplay(w), drawarea->zoomPixmap,
                         drawarea->zoom_gc, *lastx, *lasty,
                         ev->xmotion.x, ev->xmotion.y);
               if(*lasty < ev->xmotion.y)
                    {
                    *lastx = ev->xmotion.x;
                    *lasty = ev->xmotion.y;
                    }

               break;
          case Button1MotionMask + EnterWindowMask:
               XDrawLine(XtDisplay(w),XtWindow(w),
                         drawarea->zoom_gc, *lastx, *lasty,
                         ev->xmotion.x, ev->xmotion.y);
               XDrawLine(XtDisplay(w), drawarea->zoomPixmap,
                         drawarea->zoom_gc, *lastx, *lasty,
                         ev->xmotion.x, ev->xmotion.y);
               if(*lasty < ev->xmotion.y)
                    {
                    *lastx = ev->xmotion.x;
                    *lasty = ev->xmotion.y;
                    }

               break;
          case Button2MotionMask:
               break;
          case Button3MotionMask:
               break;
          default:
               break;
           }
     
     return;

}

void processButtonRelease(w, drawarea, ev, lastx, lasty, test)
Widget w;
struct draw *drawarea;
XEvent * ev;
unsigned long *lastx, *lasty;
int *test;
{


     switch(ev->xbutton.button)
          {
          case Button1:

               XDrawLine(XtDisplay(w),XtWindow(w),
                    drawarea->zoom_gc, *lastx, *lasty,
                    ev->xbutton.x, ev->xbutton.y);
     
               *lastx = ev->xbutton.x;
               *lasty = ev->xbutton.y;

               break;
          case Button2:
               break;
          case Button3:
               *test = FALSE;
               XUngrabPointer(XtDisplay(XtNameToWidget(drawarea->shell, 
                                                   "*ZoomArea")), CurrentTime);
               break;
          default:
               break;
           }
     
     return;

}

float calculateFirstBeamPos(w, file_entry, first_scan, current_scan, last_scan, anchor_x, anchor_y, dist_to_nadir, zoom_alt_to_pixel, drawarea)
Widget w;
int *file_entry, *first_scan, *current_scan, *last_scan;
unsigned long anchor_x, anchor_y;
int dist_to_nadir;
float zoom_alt_to_pixel;
struct draw *drawarea;
{

     float position;
     int pixelDistance;


     *first_scan = (int) (drawarea->base_y * data_reducer +
                    anchor_y / drawarea->magnify);

     *file_entry = *first_scan + drawarea->top_ping;

     *last_scan = *current_scan = *first_scan;

     pixelDistance = abs((int)anchor_x - dist_to_nadir);

     position = (float)pixelDistance / zoom_alt_to_pixel;

     return(position);
}

float calculateBeamPos(w, current_scan, lastx, lasty, dist_to_nadir, zoom_alt_to_pixel, drawarea)
Widget w;
int *current_scan;
unsigned long lastx, lasty;
int dist_to_nadir;
float zoom_alt_to_pixel;
struct draw *drawarea;
{

     float position;
     int pixelDistance;

     *current_scan = (int) (drawarea->base_y * data_reducer +
                         lasty / drawarea->magnify);

     pixelDistance = abs((int)lastx - dist_to_nadir);

     position = (float)pixelDistance / zoom_alt_to_pixel;

/*
     if(w == XtNameToWidget(drawarea->shell, "*ZoomBeamPortButton"))
          position = ((int)lastx - dist_to_nadir) / zoom_alt_to_pixel;
     else
          position = (dist_to_nadir - (int)lastx) / zoom_alt_to_pixel;
*/

     return(position);
}

void interpolateBeamPos(current_scan, last_scan, beamPos, lasty)
int current_scan, last_scan, lasty;
float *beamPos;
{

     int i, j;
     int scan_diff;
     int firsty;

     float alt_diff;
     float intercept;
     float slope;

     scan_diff = current_scan - last_scan;
     firsty = lasty - scan_diff;
     alt_diff = beamPos[lasty] - beamPos[firsty];
     slope = alt_diff / scan_diff;
     intercept = beamPos[lasty] - slope * lasty;

     for(i = firsty + 1, j = 1; i < lasty; i++, j++)
          beamPos[firsty + j] = i * slope + intercept;

     return;
          
}
