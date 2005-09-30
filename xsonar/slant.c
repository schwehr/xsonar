/*    Program SLANT
 *
 *    Corrects for slant range distortion in the raw record
 *    and removes the water column.  Assumes a flat bottom.
 *    Will track the bottom if the '-a' switch is set, otherwise
 *    the vehicle altitude is taken from the header.
 *
 *    William W. Danforth, U.S. Geological Survey
 *    April, 1989.
 *
 *    Modified July, 1990 by W. Danforth to track the first
 *    sonar return return for    altitude data.
 *
 *    7/25/92:    Updated and revised to run under X11 Release 4
 *                and Motif 1.1.3.
 *                William Danforth, U.S.G.S.
 *
 */

#include <unistd.h> /* read, close, lseek, write */

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include "xsonar.h"

#include "sonar_struct.h"


float find_alt(int imagesize, unsigned char *indata, int pixel_alt_threshold, int firstportpix, int firststbdpix, int port_flag, float resin);
void smooth(float *alt, int n);


void slant(UNUSED Widget w,
	   XtPointer client_data,
	   UNUSED  XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    Widget fileLabel;

    unsigned char *outdata, *indata;
    unsigned char *portout_ptr, *stbdout_ptr;
    unsigned char portinpix, stbdinpix;
    char outputfile[100];
    char pingmessage[255];
    char warningmessage[255];
    char tempText[100];

    int *table2_ptr;
    int groundrng2, portSlantrng, stbdSlantrng;
    int i, z;
    int firstportpix, firststbdpix;
    int scansize, imagesize;
    int halfscan;
    int outbytes;
    int scannum;
    int port_flag;
    int threshold_flag;
    int pixel_alt_threshold;
    int headsize;
    int updateScanSize;
    int cancelCheck;
    int eventCheck;
    int infd;
    int outfd;

    off_t position;

    float portAlt, stbdAlt;
    float portPixAlt, stbdPixAlt, portSum, stbdSum,resin;
    float portPixAlt2, stbdPixAlt2;
/*
    float oldAlt;
*/
    float newPortAlt[15000];
    float newStbdAlt[15000];

    double swath_width;
/*
    double distanceTravelled;
    double deltaAlt;
    double angle;
    double oldNorth, oldEast;
*/

    Widget slantDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget separator2;
    Widget cancelButton;
    Widget displayPings;
    Widget buttonHolder;

    Arg args[10];
    Cardinal n;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;


    scannum = 0;
    port_flag = 0;
    threshold_flag = 1;
    headsize = HEADSIZE;          /*   defined in sonar_struct.h    */
    updateScanSize = 0;
 
    /*
     *   If slant flag was not set in setup, return.
     */

    if(!sonar->slant_flag)
          return;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    slantDialog = XtCreatePopupShell("SlantDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                     slantDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Performing correction ...",
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

    XtRealizeWidget(slantDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(slantDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(slantDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(slantDialog, args, n);

    XtPopup(slantDialog, XtGrabNone);

    /*
     *    Check input arguments
     */

    if(!sonar->swath)
        {
        sprintf(warningmessage, "Chose setup and select a swath width");
        messages(sonar, warningmessage);
        closeDialog(sonar, slantDialog, True);
        return;
        }

    if(sonar->findAltFlag && !threshold_flag)
        {

        sprintf(warningmessage,
                "Need to input a threshold for finding the altitude");
        messages(sonar, warningmessage);
        closeDialog(sonar, slantDialog, True);
        return;
        }

     if((infd = open(sonar->infile, O_RDWR)) == -1)
        {
        sprintf(warningmessage, "Error opening input file.\nPlease reselect a file.");
        messages(sonar, warningmessage);
        closeDialog(sonar, slantDialog, True);
        return;
        }

    sprintf (outputfile, "%ss", sonar->infile);
/*
    strcpy(outputfile, sonar->infile);
    strcat(outputfile, "s");
*/

    if((outfd = open(outputfile, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningmessage, "Error opening output file");
        messages(sonar, warningmessage);
        close(infd);
        closeDialog(sonar, slantDialog, True);
        return;
        }


    /*
     *    allocate memory for input and output ping
     */

     if(read (infd, &sonar->ping_header, headsize) != headsize)
         {
         sprintf(warningmessage, "cannot read first header");
         messages(sonar, warningmessage);
         close(infd);
         close(outfd);
         closeDialog(sonar, slantDialog, True);
         return;
         }
    else
         {
/*
         oldAlt = sonar->ping_header.alt;
         oldNorth = sonar->ping_header.utm_n;
         oldEast = sonar->ping_header.utm_e;
*/
         }

    if(sonar->ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, slantDialog, True);
        return;
        }


    position = lseek(infd, 0L, SEEK_SET);

    scansize = sonar->ping_header.sdatasize;

/*
    if((scansize - headsize) % 512)
        {
        getScanSize(sonar, &scansize, &headsize);
        updateScanSize++;
        }
*/


    imagesize = scansize - headsize;
    halfscan = imagesize / 2;

    if(sonar->altManuallyCorrected && sonar->findAltFlag)
        {
        sprintf(warningmessage, "Altitude has been traced with mouse and\n");
	strcat(warningmessage, "find altitude flag is set (will delete\n");
	strcat(warningmessage, "altitudes traced with the mouse) !!\n");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, slantDialog, True);
        return;
        }

    outdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(outdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for output sonar array");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, slantDialog, True);
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
        closeDialog(sonar, slantDialog, True);
        return;
        }

    table2_ptr = (int *) XtCalloc(halfscan, sizeof(int));
    if(table2_ptr == NULL)
        {
        sprintf(warningmessage, "Not enough memory for table pointer");
        messages(sonar, warningmessage);
        XtFree((char *)indata);
        XtFree((char *)outdata);
        close(infd);
        close(outfd);
        closeDialog(sonar, slantDialog, True);
        return;
        }

/*
    table2_ptr = table2;
     sonar->infile = "l8f2.";
*/

    for(i = 0; i < halfscan; i++)
        table2_ptr[i] = i * i;

    firstportpix = halfscan;
    firststbdpix = halfscan - 1;

    swath_width = sonar->swath;

    resin = (float) swath_width / imagesize;

    pixel_alt_threshold = sonar->findAltThreshold;

    if(sonar->findAltFlag)
        {
        sprintf(pingmessage, "Finding first return for port and stbd ...");
        n = 0;
        XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(pingmessage,
                            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(displayPings, args, n);
        XmUpdateDisplay(slantDialog);

        z = 0;
        while((read (infd, &sonar->ping_header, 256) == 256) && 
                    (read (infd, indata, imagesize) == imagesize))
            {
            newPortAlt[z] = find_alt(imagesize, indata, pixel_alt_threshold, 
                                  firstportpix, firststbdpix, port_flag, resin);
            port_flag = 0;
            newStbdAlt[z] = find_alt(imagesize, indata, pixel_alt_threshold,
                                  firstportpix, firststbdpix, port_flag, resin);
            port_flag = 1;

            z++;
            }
        smooth(newPortAlt, z);
        smooth(newStbdAlt, z);
        }

    position = lseek(infd, 0L, SEEK_SET);
    port_flag = 0;

/*    Write file header.
        if ((write (outfd, &demuxed_file_header, FILE_HEADER_SIZE)) != FILE_HEADER_SIZE)
                {
                fprintf (stdout, "%s: error writing file header.\n", progname);
                }
 */


    /*
     *    Main loop
     */

    eventCheck = 1;


    while((read (infd, &sonar->ping_header, 256) == 256) && 
             (read (infd, indata, imagesize) == imagesize))
        {

/*
        if(!(scannum % 10))
            {
            distanceTravelled = sqrt(((sonar->ping_header.utm_n - oldNorth) 
                        *  (sonar->ping_header.utm_n - oldNorth))
                        + ((sonar->ping_header.utm_e - oldEast) *
                           (sonar->ping_header.utm_e - oldEast)));

            
            deltaAlt = fabs(sonar->ping_header.alt - oldAlt);
            if(deltaAlt != 0)
                angle = RAD_TO_DEG * atan(deltaAlt / distanceTravelled); 
            else
                angle = 0;

fprintf(stdout, "deltaAlt = %f\tdistance = %f\tangle = %f\n", deltaAlt, distanceTravelled, angle);
            oldAlt = sonar->ping_header.alt;
            oldNorth = sonar->ping_header.utm_n;
            oldEast = sonar->ping_header.utm_e;

            }

        sonar->ping_header.altSlope = angle;
*/

        if(!(scannum % 100))
            {
            sprintf(pingmessage, "Processing Ping # %d", scannum);
            n = 0;
            XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(pingmessage,
                                XmSTRING_DEFAULT_CHARSET)); n++;
            XtSetValues(displayPings, args, n);
            XmUpdateDisplay(slantDialog);
            }

        /*
         *    Update the display, check for events.
         */

        while(eventCheck == 1)
            {
            if(XtAppPending(sonar->toplevel_app))
                {
                XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
                XmUpdateDisplay(sonar->toplevel);
                }
            else
                eventCheck = 0;
            }

        eventCheck = 1;

        /*
         *    Check to see if the user has hit the cancel button.
         */

        XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
        if(cancelCheck == True)
            {
            XtFree((char *)indata);
            XtFree((char *)outdata);
            XtFree((char *)(char *)table2_ptr);
            close(infd);
            close(outfd);
            closeDialog(sonar, slantDialog, True);
            return;
            }


        /*
         *    Begin processing the data.
         */

        portout_ptr = &outdata[halfscan];
        stbdout_ptr = &outdata[halfscan - 1];

        if(sonar->findAltFlag)
            {
            if(port_flag)
                portAlt = sonar->ping_header.alt = newPortAlt[scannum];
            else
                portAlt = sonar->ping_header.alt = newStbdAlt[scannum];
            stbdAlt = newStbdAlt[scannum];
            portPixAlt = (portAlt / resin);
            portPixAlt2 = portPixAlt * portPixAlt;
            stbdPixAlt = (stbdAlt / resin);
            stbdPixAlt2 = stbdPixAlt * stbdPixAlt;
            }
        else
            {
            portAlt = stbdAlt = sonar->ping_header.alt;
            portPixAlt = stbdPixAlt = (portAlt / resin);
            portPixAlt2 = stbdPixAlt2 = portPixAlt * portPixAlt;
            }

        for(i = 0; i < halfscan; i++)
            {
            groundrng2 = table2_ptr[i];

            portSum = groundrng2 + portPixAlt2;
            stbdSum = groundrng2 + stbdPixAlt2;

            portSlantrng = (int) (sqrt(portSum) + 0.5);
            stbdSlantrng = (int) (sqrt(stbdSum) + 0.5);

            if(portSlantrng > halfscan - 1)
                portinpix = stbdinpix = 0;
            else
                {
                portinpix = indata[firstportpix + portSlantrng];
                stbdinpix = indata[firststbdpix - stbdSlantrng];
                }

            *portout_ptr++ = portinpix;
            *stbdout_ptr-- = stbdinpix;
            }



        if(updateScanSize)
            sonar->ping_header.sdatasize = scansize;

        position = -(256 + imagesize);
        position = lseek(infd, position, SEEK_CUR);
        if((outbytes = write(infd, &sonar->ping_header, 256)) != 256 ||
            (outbytes = write(infd, indata, imagesize)) != imagesize)
            {
            sprintf(warningmessage,
                     "Error writing back to input file, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            XtFree((char *)table2_ptr);
            close(infd);
            close(outfd);
            closeDialog(sonar, slantDialog, True);
            return;
            }

        if((outbytes = write(outfd, &sonar->ping_header, 256)) != 256 ||
            (outbytes = write(outfd, outdata, imagesize)) != imagesize)
            {
            sprintf(warningmessage,
                     "Error on output, record length: %d", outbytes);
            messages(sonar, warningmessage);
            XtFree((char *)indata);
            XtFree((char *)outdata);
            XtFree((char *)table2_ptr);
            close(infd);
            close(outfd);
            closeDialog(sonar, slantDialog, True);
            return;
            }


        ++scannum;
        }

    strcat(sonar->infile, "s");

    fileLabel = XtNameToWidget(sonar->toplevel, "*FileNameLabel");
    if(fileLabel)
        {
        sprintf(tempText, "File is: %s", sonar->infile);

        n = 0;
        XtSetArg(args[n], XmNlabelString,
             XmStringCreateLtoR(tempText, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(fileLabel, args, n);
        }

    XtFree((char *)indata);
    XtFree((char *)outdata);
    XtFree((char *)table2_ptr);

    close(infd);
    close(outfd);

    closeDialog(sonar, slantDialog, False);


    return;

}

/*    smooth()
 *
 *    smooths the alt array by a moving average. 
 *
 */
void smooth(float *alt, int n)
{
    int i;
    float oldalt[15000];

    for (i = 0; i < n; i++)
        oldalt[i] = alt[i];

    for (i = 2; i < n - 2; i++)
        {
        alt[i] = (oldalt[i+2] + oldalt[i+1] + 
        oldalt[i] + oldalt[i-1] + oldalt[i-2]) / 5.0;
        }

    return;
}

float find_alt(int imagesize, unsigned char *indata, int pixel_alt_threshold, int firstportpix, int firststbdpix, int port_flag, float resin)
{ 
    int i, j;
    int pixalt;
    int startPoint;
    float alt; /* FIX: alt can be used uninitialized */

    startPoint = 2 / resin;  /*  start 2 meters out from nadir */

    if(port_flag)
        {
        for(i = (firstportpix + startPoint); i < imagesize; i++)
            {
            if(((indata[i - 3] + indata[i - 2] +
                 indata[i - 1] + indata[i] +
                 indata[i + 1] + indata[i + 2] +
                 indata[i + 3]) / 7) > pixel_alt_threshold) 
                {
                for(j = (i - 2); j < imagesize; j++)
                    {
                    if(indata[j] > pixel_alt_threshold) 
                        {
                        pixalt = (float) ((j  + .25 / resin) - firstportpix);
                        alt = (float)pixalt * resin;
                        break;
                        }
                    }
                break;
                }
            }
        }
    else    
        {
        for(i = (firststbdpix - startPoint); i > 0; i--)
            {
            if(((indata[i - 2] + indata[i - 1] +
                 indata[i] + indata[i + 1] +
                 indata[i + 2]) / 5) > pixel_alt_threshold) 
                {
                for(j = (i + 5); j > 0; j--)
                    {
                    if(indata[j] > pixel_alt_threshold) 
                        {
                        pixalt = (float) (firststbdpix - (j - .25 / resin));
                        alt = (float)pixalt * resin;
                        break;
                        }
                    }
                break;
                }
            }
        }
    return(alt);
}

