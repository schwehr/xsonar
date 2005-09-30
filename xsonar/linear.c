/* 
 *    linear.c 
 *
 *    program to accomplish linear stretch of a compacted 
 *    data file. Output file name will be file.l for 
 *    "linearly stretched"
 *
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include "sonar_struct.h"

#include "xsonar.h"

int two_pt_stretch(int pixval, int minpixel, int maxpixel);
int three_pt_stretch(int pixval, int minpixel, int midpixel, int maxpixel);

void stretch(UNUSED Widget w,
	     XtPointer client_data,
	     UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget linearDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget separator2;
    Widget displayPings;
    Widget cancelButton;
    Widget buttonHolder;
    Widget fileLabel;

    Arg args[30];
    Cardinal n;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    char outputfile[255];
    char pingmessage[70];
    char warningmessage[200];
    char statusmessage[200];
    char tempText[255];

    unsigned char *indata, *outdata;

    int i, pixval;
    int minpixel, midpixel, maxpixel;
    int scannum;
    int two_point_stretch_flag;
    int three_point_stretch_flag;
    ssize_t inbytes;
    int outbytes;
    int scansize;
    int imagesize;
    int headsize;
    int halfscan;
    int cancelCheck;
    int infd;
    int outfd;

    PingHeader ping_header;

    off_t position;

#if 0
    void closeDialog();
    void messages();

    void cancelCallback();
#endif
    two_point_stretch_flag = 0;
    three_point_stretch_flag = 0;
    scannum = 0;
    headsize = HEADSIZE;

     /*
      *   If linear flag was not set in setup, return.
      */

    if(!sonar->linear_flag)
          return;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    linearDialog = XtCreatePopupShell("LinearDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                     linearDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Applying Linear Stretch",
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

    XtRealizeWidget(linearDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(linearDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(linearDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(linearDialog, args, n);

    XtPopup(linearDialog, XtGrabNone);

    /*
     *    Open input and output files.
     */


     if((infd = open(sonar->infile, O_RDWR)) == -1)
          {
          sprintf(warningmessage, "Error opening input file");
          messages(sonar, warningmessage);
          closeDialog(sonar, linearDialog, True);
          return;
          }

     /*sprintf(outputfile, "%sl\0", sonar->infile); FIX: was the \0 needed for anything?*/
     sprintf(outputfile, "%sl", sonar->infile);

     if((outfd = open(outputfile, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
          {
          sprintf(warningmessage, "Error opening output file");
          messages(sonar, warningmessage);
          close(infd);
          closeDialog(sonar, linearDialog, True);
          return;
          }

    /*
     *    If stretch params not set during program call, warn user and quit 
     */

    if(!sonar->stretch_params[1] || !sonar->stretch_params[2])
        {
        sprintf(warningmessage, "Call setup and set stretch values");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, linearDialog, True);
        return;
        }

    /*
     *     Check to see if user wants a 3-point stretch
     */

    if(!sonar->stretch_params[1])
        two_point_stretch_flag++;
    else
        three_point_stretch_flag++;

    minpixel = sonar->stretch_params[0];
    midpixel = sonar->stretch_params[1];
    maxpixel = sonar->stretch_params[2];

/*
        if ((write (out_fd, &demuxed_file_header, FILE_HEADER_SIZE)) != FILE_HEADER_SIZE)
                {
                fprintf (stdout, "%s: error writing file header.\n", progname);
                err_exit();
                }
*/

    /*
     *   allocate memory for input and output ping
     */

    if (read (infd, &ping_header, 256) != 256)
        {
        sprintf(warningmessage, "Cannot read first ping header");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, linearDialog, True);
        return;
        }

    if(ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, linearDialog, True);
        return;
        }


    position = lseek(infd, 0L, SEEK_SET);

    scansize = ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;
    halfscan = imagesize / 2;

    outdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(outdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for output sonar array");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, linearDialog, True);
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
        closeDialog(sonar, linearDialog, True);
        return;
        }

    /*
     *    Main loop -- Read input file and output stretched data
     */

    sprintf(statusmessage, "Applying three point linear stretch ...");
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
            XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetValues(statusMessage, args, n);
    XmUpdateDisplay(linearDialog);

    /*
     *    Main loop.
     */


     while((inbytes = read (infd, &ping_header, headsize)) == headsize &&
        (inbytes = read (infd, indata, imagesize)) == imagesize)
        {
        ++scannum;

        if(!(scannum % 100))
            {
            sprintf(pingmessage, "Processing Ping # %d", scannum);
            n = 0;
            XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(pingmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
            XtSetValues(displayPings, args, n);
            XmUpdateDisplay(linearDialog);
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
            closeDialog(sonar, linearDialog, True);
            return;
            }


        for (i = 0; i < imagesize; i++)
            {
            pixval = indata[i];
            
            if(!sonar->stretch_params[1])
                indata[i] = two_pt_stretch(pixval, minpixel, maxpixel);
            else
                indata[i] = three_pt_stretch(pixval,
                                    minpixel, midpixel, maxpixel);

            }

        if((outbytes = write(outfd, &ping_header, 256)) != 256
        || (outbytes = write(outfd, indata, imagesize)) != imagesize)
            {
            sprintf(warningmessage,
                       "Error on output, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            closeDialog(sonar, linearDialog, True);
            return;
            }
        }

    if(inbytes != 0)
        {
        sprintf(warningmessage, "Error on input, record length: %d", inbytes);
        messages(sonar, warningmessage);
        }
    else
        {


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
        }

    XtFree((char *)indata);
    XtFree((char *)outdata);

    close(infd);
    close(outfd);

    closeDialog(sonar, linearDialog, False);

    return;    
}

int two_pt_stretch(int pixval, int minpixel, int maxpixel)
{
    int outpix;
    if(pixval <= minpixel)
        outpix = 0;
    else if(pixval < maxpixel)
        outpix = (int) ((( (float) (pixval - minpixel) / 
        (float) (maxpixel - minpixel)) * 255.0) + 0.5); 
    else
        outpix = 255;

    return(outpix);
}

int three_pt_stretch(int pixval, int minpixel, int midpixel, int maxpixel)
{
    int outpix;

    if(pixval <= minpixel)
        outpix = 0.0;
    else if(pixval <= midpixel)
        outpix = (int) ((( (float) (pixval - minpixel) / (float) (midpixel - minpixel)) * 128.0) + 0.5); 
    else if(pixval < maxpixel)
        outpix = (int) ((( (float) (pixval - midpixel) / (float) (maxpixel - midpixel)) * 128.0) + 127.5); 
    else
        outpix = 255;


    return(outpix);

}

