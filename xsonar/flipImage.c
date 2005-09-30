/* 
 *
 *     flipImage.c
 *
 *     Program that takes the ones complement of an image and
 *     writes the result to the same file.
 *
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>

#include <map.h>
#include "xsonar.h"
#include "sonar_struct.h"

void close_flip(MainSonar *sonar, Widget w);

void flipImage(MainSonar *sonar, int infd)
{


     Widget flipDialog;
     Widget statusWindow;
     Widget statusMessage;
     Widget separator1;
     Widget displayPings;

     Arg args[10];
     Cardinal n;

     char pingmessage[40];
     char warningmessage[100];
     char statusmessage[100];

     unsigned char *image;

     int i;
     int image_width, image_height;
     int pixel_number = 0;
     int inbytes;
     /*int outfd;*/

     long position;

     /*void messages();*/

     /*
      *   Create a form for holding two windows:  A status window for 
      *   messages, and a ping window to indicate how many pings have
      *   been processed.
      */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, False); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     XtSetArg(args[n], XmNx, 400); n++;
     XtSetArg(args[n], XmNy, 400); n++;
     flipDialog = XtCreatePopupShell("FlipDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                      flipDialog, args, n);
     XtManageChild(statusWindow);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Inverting Image",
          XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNheight, 20); n++;
     XtSetArg(args[n], XmNwidth, 300); n++;
     XtSetArg(args[n], XmNrecomputeSize, False); n++;
     statusMessage = XtCreateWidget("StatusMessage",
                         xmLabelWidgetClass, statusWindow, args, n);
     XtManageChild(statusMessage);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, statusMessage); n++;
     separator1 = XtCreateWidget("Separator1", xmSeparatorWidgetClass,
                                   statusWindow, args, n);
     XtManageChild(separator1);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNheight, 20); n++;
     XtSetArg(args[n], XmNwidth, 300); n++;
     XtSetArg(args[n], XmNrecomputeSize, False); n++;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Processing Pixel # 0",
          XmSTRING_DEFAULT_CHARSET)); n++;
     displayPings = XtCreateWidget("DisplayPings",
                    xmLabelWidgetClass, statusWindow, args, n);
     XtManageChild(displayPings);

     XtRealizeWidget(flipDialog);
     XtPopup(flipDialog, XtGrabNone);

     /*
      *     Open input and output files.
      */

     image_width = 1728;
     image_height = 1728;

     image = (unsigned char *) 
                  XtCalloc(image_width * image_height, sizeof(unsigned char));

     if(image == NULL)
          {
          sprintf(warningmessage, "Error initializing image array");
          messages(sonar, warningmessage);
          close_flip(sonar, flipDialog);  /* FIX had to cast sonar to an int. SCARY */
          return;
          }

     if((inbytes = read (infd, image, image_width * image_height)) != 
                                             image_width * image_height)
          {
          sprintf(warningmessage, "Error reading image data");
          messages(sonar, warningmessage);
          close_flip(sonar, flipDialog);  /* FIX had to cast sonar to an int. SCARY */
          return;
          }


     for(i = 0; i < (image_width * image_height); i++)
          {
          ++pixel_number;

          if(!(pixel_number % 200000))
               {
               sprintf(pingmessage, "Processing Pixel # %d", pixel_number);
               n = 0;
               XtSetArg(args[n], XmNlabelString,
                              XmStringCreateLtoR(pingmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
               XtSetValues(displayPings, args, n);
               XmUpdateDisplay(flipDialog);
               }

          image[i] = ~image[i];

          }

     position = lseek(infd, 0L, SEEK_SET);

     sprintf(statusmessage, "Writing image");

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                                              XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetValues(statusMessage, args, n);
     XmUpdateDisplay(flipDialog);

     if((write(infd, image, image_width * image_height)) != 
         image_width * image_height)
         {
         sprintf(warningmessage, "Error writing image data");
         messages(sonar, warningmessage);
         close_flip(sonar, flipDialog);  /* FIX had to cast sonar to an int. SCARY */
         return;
         }


     XtFree((char *) image);

     /*close_flip(flipDialog);*/
     close_flip(sonar, flipDialog); /* FIX: how did this work with only one arg? */
     close(infd);

     return;     
}

/* FIX: sonar should be MainSonar * ??? */
void close_flip(UNUSED MainSonar *sonar, Widget w)
{
     Widget parent_widget;

     parent_widget = w;

/*
     while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);
*/

     XtPopdown(parent_widget);
     XtDestroyWidget(parent_widget);
}
