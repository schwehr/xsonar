/* 
 *    Callback to accomplish histogram equalization of data displayed 
 *    in the zoom window.
 *
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include "xsonar.h"

#include "sonar_struct.h"


#define     MAX_GRAY 256            /* range of pixel values */

void equalize( UNUSED  Widget w,
	      XtPointer client_data,
	       UNUSED  XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;
    /*GC gc;*/
    /*unsigned int width, height;*/

    Widget equalizeDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget separator2;
    Widget displayPings;
    Widget cancelButton;
    Widget buttonHolder;
    Widget fileLabel;

    Arg args[10];
    Cardinal n;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    char outputfile[100];
    char pingmessage[40];
    char warningmessage[100];
    char statusmessage[100];
    char *fileNamePtr;
    char tempText[100];

    unsigned char *indata;
    unsigned char *outdata;

    int scansize;
    int imagesize;
    int scannum; /* FIX: can be used uninitialized */
    int inbytes;
    int outbytes;
    int i;
    int pixel_count;
    int cancelCheck;
    int histogram_table[MAX_GRAY];
    int histogram_dist[MAX_GRAY];
    int infd;
    int outfd;

    double gray_level_fraction, pixel_sum;

    long pixel_value;
    long position;

    /*
     *   If equalize flag was not set in setup, return.
     */

    if(!sonar->equalize_flag)
          return;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    equalizeDialog = XtCreatePopupShell("EqualizeDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                     equalizeDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Performing Histogram Equalization",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
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
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Processing Ping # 0",
         XmSTRING_DEFAULT_CHARSET)); n++;
    displayPings = XtCreateWidget("DisplayPings",
                   xmLabelWidgetClass, statusWindow, args, n);
    XtManageChild(displayPings);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, displayPings); n++;
    separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                                  statusWindow, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    buttonHolder = XtCreateWidget("ButtonHolder", xmRowColumnWidgetClass,
                        statusWindow, args, n);
    XtManageChild(buttonHolder);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Cancel", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton", xmPushButtonWidgetClass,
                        buttonHolder, args, n);
    XtManageChild(cancelButton);

    cancelCheck = False;

    XtAddCallback(cancelButton, XmNactivateCallback, 
                                            cancelCallback, NULL);
    XtVaSetValues(cancelButton, XmNuserData, cancelCheck, NULL);

    XtRealizeWidget(equalizeDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(equalizeDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(equalizeDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(equalizeDialog, args, n);

    XtPopup(equalizeDialog, XtGrabNone);

    /*
     *   Open input and output files.
     */


    if((infd = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningmessage, "Error opening input file");
        messages(sonar, warningmessage);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }

    sprintf (outputfile, "%se", sonar->infile);

    if((outfd = open(outputfile, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningmessage, "Error opening output file");
        messages(sonar, warningmessage);
        close(infd);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }
    else
        {

        if((fileNamePtr = strrchr(sonar->infile, '/')) == NULL)
              sprintf(statusmessage, "Output file is %se", sonar->infile);
        else
            {
            fileNamePtr++;
            sprintf(statusmessage, "Output file is %se", fileNamePtr);
            }

        n = 0;
        XtSetArg(args[n], XmNlabelString,
                  XmStringCreateLtoR(statusmessage,
                  XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(statusMessage, args, n);
        XmUpdateDisplay(equalizeDialog);
        }

     /*
      *   allocate memory for input and output ping
      */
    if (read (infd, &sonar->ping_header, 256) != 256)
        {
        sprintf(warningmessage, "cannot read first header");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }

    if(sonar->ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }


    position = lseek(infd, 0L, SEEK_SET);

    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;

    outdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(outdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for output sonar array");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }

    indata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(indata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for input sonar array");
        messages(sonar, warningmessage);
        XtFree((char *)outdata);
        close(infd);
        close(outfd);
        closeDialog(sonar, equalizeDialog, True);
        return;
        }

     /*   Initialize histogram and sums
      */
     for (i = 0; i < MAX_GRAY; i++)
          {
          histogram_table[i] = 0;
          histogram_dist[i] = 0;
          }

    gray_level_fraction = pixel_count = pixel_sum = 0.0;

     /*
      *   Main loop.
      *   Read input image and compute histogram
      */

     while((inbytes = read (infd, &sonar->ping_header, 256) == 256) &&
           (inbytes = read (infd, indata, imagesize) == imagesize))
          {
          ++scannum;

          if(!(scannum % 100))
               {
               sprintf(pingmessage, "Processing Ping # %d", scannum);
               n = 0;
               XtSetArg(args[n], XmNlabelString,
                              XmStringCreateLtoR(pingmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
               XtSetValues(displayPings, args, n);
               XmUpdateDisplay(equalizeDialog);
               }

        if(XtAppPending(sonar->toplevel_app))
            {
            XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
            XmUpdateDisplay(sonar->toplevel);
            }

        /*
         *    Check on the status of the cancel button.
         */

        XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
        if(cancelCheck)
            {
            XtFree((char *)outdata);
            XtFree((char *)indata);
            close(infd);
            close(outfd);
            closeDialog(sonar, equalizeDialog, True);
            return;
            }

        for(i = 0; i < imagesize; i++)
            {
            pixel_value = indata[i];
            if(pixel_value > 0)
                {
                histogram_dist[pixel_value]++;
                pixel_sum += pixel_value;
                pixel_count++;
                }
            }

        }

     /*
     *   Compute look-up table
      */

     for (i = 0; i < MAX_GRAY; i++)
          {
/*
          gray_level_fraction += (double) (histogram_dist[i] * i) / pixel_sum;
*/
          gray_level_fraction += (double) histogram_dist[i] / (double) pixel_count;
          histogram_table[i] = (int)(0.5 + gray_level_fraction * 255.0);
/*
printf("gray frac = %f\thist_table[%d] = %d\n", gray_level_fraction, i, histogram_table[i]);
*/
          if (histogram_table[i] > 255)
               histogram_table[i] = 255;
          if(histogram_table[i] < 0)
               histogram_table[i] = 0;
          }

     /*   
     *    Reread the data and equalize it.
     *    First, rewind the data file.
      */

    scannum = 0;
    position = lseek(infd, 0L, SEEK_SET);

    while((inbytes = read (infd, &sonar->ping_header, 256) == 256) &&
           (inbytes = read (infd, indata, imagesize) == imagesize))
        {
        ++scannum;

        if(!(scannum % 100))
            {
            sprintf(pingmessage, "Writing Ping # %d", scannum);
            n = 0;
            XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(pingmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
            XtSetValues(displayPings, args, n);
            XmUpdateDisplay(equalizeDialog);
            }

        if(XtAppPending(sonar->toplevel_app))
            {
            XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
            XmUpdateDisplay(sonar->toplevel);
            }

        /*
         *    Check on the status of the cancel button.
         */

        XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
        if(cancelCheck)
            {
            XtFree((char *)outdata);
            XtFree((char *)indata);
            close(infd);
            close(outfd);
            closeDialog(sonar, equalizeDialog, True);
            return;
            }


        /*
         *    Update pixel values for current ping from 
         *    the histogram lookup table.
         */

        for(i = 0; i < imagesize; i++)
            {
            pixel_value = indata[i];
            if(pixel_value > 0)
                indata[i] = histogram_table[pixel_value];
            }

        /*
         *    Write out the current ping.
         */
            
        if((outbytes = write(outfd, &sonar->ping_header, 256)) != 256
          || (outbytes = write(outfd, indata, imagesize)) != imagesize)
            {
            sprintf(warningmessage,
                         "Error on output, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            return;
            }

        }

    XtFree((char *)indata);
    XtFree((char *)outdata);

    /*
     *    Set the file label on the Setup display if popped up.
     */

    strcpy(sonar->infile, outputfile);

    fileLabel = XtNameToWidget(sonar->toplevel, "*FileNameLabel");
    if(fileLabel)
        {
        strcpy(tempText, "File is: ");
        strcat(tempText, sonar->infile);

        n = 0;
        XtSetArg(args[n], XmNlabelString,
            XmStringCreateLtoR(tempText, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(fileLabel, args, n);
        }

    close(infd);
    close(outfd);
    closeDialog(sonar, equalizeDialog, False);

    return;
}
