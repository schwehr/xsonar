/* 
 *    destripe.c
 *
 *    Program to detect and correct low-amplitude records in a Sea MARC
 *    data file. The data must be compacted (e.g., by smdemux).
 *    A stripe is detected if the average ratio between the sum of all
 *    pixels in the record and the sum of all pixels in the two adjacent
 *    records is less than delta. If a stripe is detected, the data in the
 *    record are replaced by the average between the two adjacent records.
 *    The process is repeated until there is no change.
 *    Output is written to a file input_file'd', for "destriped".
 *
 *    Options:
 *        -d max_delta    Value of delta; defaults to MAX_DELTA_DEF.
 *        -v        Verbose option. Prints to stdout statistics
 *                for each record that is a stripe
 *
 *    A. Malinverno
 *    October 25 1987
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>

#include "xsonar.h"

#include "sonar_struct.h"

#define     MAX_DELTA_DEF 0.80


void s_interpo(unsigned char *this, unsigned char *last, unsigned char *next, int imagesize);
void p_interpo(unsigned char *this, unsigned char *last, unsigned char *next, int imagesize);
int s_integ(unsigned char *c, int imagesize);
int p_integ(unsigned char *c, int imagesize);
int get(int fd, long int *pos, unsigned char *c, PingHeader *head, int n);
int put(int fd, long int *pos, unsigned char *c, PingHeader *head, int n);


void destripe(UNUSED Widget w,
	      XtPointer client_data,
	      UNUSED  XtPointer call_data)
{


    MainSonar *sonar = (MainSonar *) client_data;

    PingHeader last_ping_header, next_ping_header;

    Widget destripeDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget separator2;
    Widget buttonHolder;
    Widget cancelButton;
    Widget displayPings;
    Widget fileLabel;

    Arg args[10];
    Cardinal n;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    unsigned char *currentdata, *lastdata, *nextdata;
    char statusmessage[255];
    char pingmessage[400];
    char warningmessage[400];
    char outputfile[255];    /* output file name */
    char tempText[100];

    int imagesize;
    int scansize;
    int halfscan;
    int outbytes; /* FIX: can be used uninitialized */
    int i;
    int /*in_fd, out_fd, c, */ recnum;    
    int pass = 0;
    int change = 1;
    int this_s_int, last_s_int, next_s_int;
    int this_p_int, last_p_int, next_p_int;
    int cancelCheck;
    int eventCheck;
    int infd;
    int outfd;

    long read_offset = 0;
    long write_offset = 0;
    long position;

    float max_delta = MAX_DELTA_DEF;
    float last_delta, next_delta, delta;

    /*
     *   If destripe flag was not set in setup, return.
     */

    if(!sonar->destripe_flag)
          return;

    /*
     *   Start creating widgets for this app.
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    destripeDialog = XtCreatePopupShell("DestripeDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                      destripeDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Beginning procedure ...",
          XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNwidth, 500); n++;
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

    XtRealizeWidget(destripeDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(destripeDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(destripeDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(destripeDialog, args, n);

    XtPopup(destripeDialog, XtGrabNone);

    XmUpdateDisplay(destripeDialog);

    /*
     *   Open input and output files.
     */

    if((infd = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningmessage, "Error opening input file");
        messages(sonar, warningmessage);
        closeDialog(sonar, destripeDialog, True);
        return;
        }

    /*sprintf (outputfile, "%sd\0", sonar->infile); FIX: need the \0 for anything */
    sprintf (outputfile, "%sd", sonar->infile);

    if((outfd = open(outputfile, O_RDWR | O_CREAT |O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningmessage, "Error opening output file");
        messages(sonar, warningmessage);
        close(infd);
        closeDialog(sonar, destripeDialog, True);
        return;
        }

    /*
     *   Allocate memory for input and output ping.
     *   Make sure the file can be read.
     */

    if (read (infd, &sonar->ping_header, 256) != 256)
        {
        sprintf(warningmessage, "Cannot read first header");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, destripeDialog, True);
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
        closeDialog(sonar, destripeDialog, True);
        return;
        }


    position = lseek(infd, 0L, SEEK_SET);

    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;
    halfscan = imagesize / 2;

    currentdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(currentdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for current sonar array");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, destripeDialog, True);
        return;
        }

    lastdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(lastdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for last sonar array");
        messages(sonar, warningmessage);
        XtFree((char *)currentdata);
        close(infd);
        close(outfd);
        closeDialog(sonar, destripeDialog, True);
        return;
        }

    nextdata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(nextdata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for next sonar array");
        messages(sonar, warningmessage);
        XtFree((char *)currentdata);
        XtFree((char *)lastdata);
        close(infd);
        close(outfd);
        closeDialog(sonar, destripeDialog, True);
        return;
        }

    /*
     *    Copy input file into output file, and rewind it
     */

    sprintf(statusmessage, "Copying %s\nto %s ...", sonar->infile, outputfile);
    n = 0;
    XtSetArg(args[n], XmNlabelString,
           XmStringCreateLtoR(statusmessage, XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetValues(statusMessage, args, n);
    XmUpdateDisplay(destripeDialog);
    XmUpdateDisplay(destripeDialog);

    recnum = 0;
    eventCheck = 1;

    while(read (infd, &sonar->ping_header, 256) == 256
        && (read (infd, currentdata, imagesize) == imagesize))
        {
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


        if(!(recnum % 100))
            {
            sprintf(pingmessage, "Copying ping # %d", recnum);
            n = 0;
            XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(pingmessage,
                         XmSTRING_DEFAULT_CHARSET)); n++;
            XtSetValues(displayPings, args, n);
            XmUpdateDisplay(destripeDialog);
            }

        write(outfd, &sonar->ping_header, 256);
        write(outfd, currentdata, imagesize);

        recnum++;

        /*
         *    Check to see if the user has hit the cancel button.
         */

        XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
        if(cancelCheck == True)
            {
            XtFree((char *)currentdata);
            XtFree((char *)nextdata);
            XtFree((char *)lastdata);
            close(infd);
            close(outfd);
            closeDialog(sonar, destripeDialog, True);
            return;
            }

        }


    position = lseek (outfd, 0L, SEEK_SET);    

    sprintf(statusmessage, "Starting search ...");
    n = 0;
    XtSetArg(args[n], XmNlabelString,
           XmStringCreateLtoR(statusmessage, XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetValues(statusMessage, args, n);
    XmUpdateDisplay(destripeDialog);

    /********************************************
     *    Loop until there are no more changes   *
     ********************************************/

    while (change)
        {
        change = 0;
        recnum = 1;

        if (get (outfd, &read_offset, lastdata, &last_ping_header,
                    imagesize) != (imagesize + HEADSIZE))
            {
            sprintf(warningmessage, "Can't read first record");
            messages(sonar, warningmessage);
            XtFree((char *)currentdata);
            XtFree((char *)nextdata);
            XtFree((char *)lastdata);
            close(infd);
            close(outfd);
            closeDialog(sonar, destripeDialog, True);
            return;
            }

        last_s_int = s_integ (lastdata, imagesize);
        last_p_int = p_integ (lastdata, imagesize);

        if (put (outfd, &write_offset, lastdata, &last_ping_header,
                         imagesize) != (imagesize + HEADSIZE))
            {
            sprintf(warningmessage, "Error in output, bytes written = %d",
                                            outbytes);
            XtFree((char *)currentdata);
            XtFree((char *)nextdata);
            XtFree((char *)lastdata);
            messages(sonar, warningmessage);
            close(infd);
            close(outfd);
            closeDialog(sonar, destripeDialog, True);
            return;
            }

        if (get (outfd, &read_offset, currentdata, &sonar->ping_header,
                    imagesize) != (imagesize + HEADSIZE))
            {
            sprintf(warningmessage, "Can't read second record");
            messages(sonar, warningmessage);
            XtFree((char *)currentdata);
            XtFree((char *)nextdata);
            XtFree((char *)lastdata);
            close(infd);
            close(outfd);
            closeDialog(sonar, destripeDialog, True);
            return;
            }

        /*
         *    Start looping in the file looking for dropouts
         */

        eventCheck = 1;

        while(get(outfd, &read_offset, nextdata, &next_ping_header,
            imagesize) == (imagesize + HEADSIZE))
            {

            if(!(recnum % 100))
                {
                sprintf(pingmessage, "Processing Ping # %d", recnum);
                n = 0;
                XtSetArg(args[n], XmNlabelString,
                        XmStringCreateLtoR(pingmessage,
                        XmSTRING_DEFAULT_CHARSET)); n++;
                XtSetValues(displayPings, args, n);
                XmUpdateDisplay(destripeDialog);
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
             *   Check the status of the cancel button.
             */

            XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
            if(cancelCheck == True)
                {
                XtFree((char *)currentdata);
                XtFree((char *)nextdata);
                XtFree((char *)lastdata);
                close(infd);
                close(outfd);
                closeDialog(sonar, destripeDialog, True);
                return;
                }


            /*
             *    Begin processing the data ...
             *
             *    Starboard
             */

            /*    Check the sums to make sure they are not
             *    0 (make them 1).  Do this to make sure
             *    the delta calculations do not divide by
             *    0.
             */

            this_s_int = s_integ(currentdata, imagesize);
            if(this_s_int == 0)
                this_s_int = 1;
            next_s_int = s_integ(nextdata, imagesize);
            if(next_s_int == 0)
                next_s_int = 1;
            last_delta = (float)this_s_int / (float)last_s_int;
            next_delta = (float)this_s_int / (float)next_s_int;
            delta = (last_delta + next_delta) / 2.0;

            if (delta < max_delta || (1 / delta) < max_delta)
                {
                s_interpo(currentdata, lastdata, nextdata, imagesize);
                change++;
                }
/*
                fprintf (stderr, "recnum=%5d(s) ", recnum);
                fprintf (stderr, "delta=%.3f ", delta);
                fprintf (stderr, "this_s_int=%d ", this_s_int);
                fprintf (stderr, "last_s_int=%d ", last_s_int);
                fprintf (stderr, "next_s_int=%d\n", next_s_int);
*/
            /*
             *    Port
             */

            /*    Check the sums to make sure they are not
             *    0 (make them 1).  Do this to make sure
             *    the delta calculations do not divide by
             *    0.
             */
            this_p_int = p_integ (currentdata, imagesize);
            if(this_p_int == 0)
                this_p_int = 1;
            next_p_int = p_integ (nextdata, imagesize);
            if(next_p_int == 0)
                next_p_int = 1;
            last_delta = (float)this_p_int / (float)last_p_int;
            next_delta = (float)this_p_int / (float)next_p_int;
            delta = (last_delta + next_delta) / 2.0;

            if (delta < max_delta || (1 / delta) < max_delta)
                {
/*
                fprintf (stderr, "recnum=%-5d(p) ", recnum);
                fprintf (stderr, "delta=%.3f ", delta);
                fprintf (stderr, "this_p_int=%d ", this_p_int);
                fprintf (stderr, "last_p_int=%d ", last_p_int);
                fprintf (stderr, "next_p_int=%d\n", next_p_int);
*/
                p_interpo(currentdata, lastdata, nextdata, imagesize);
                change++;
                }

             if (put (outfd, &write_offset, currentdata,
                    &sonar->ping_header, imagesize) !=
                    (imagesize + HEADSIZE))
                {
                sprintf(warningmessage,
                             "Error in output, bytes written = %d", outbytes);
                XtFree((char *)currentdata);
                XtFree((char *)nextdata);
                XtFree((char *)lastdata);
                messages(sonar, warningmessage);
                close(infd);
                close(outfd);
                closeDialog(sonar, destripeDialog, True);
                return;
                }

            for(i = 0; i < imagesize; i++)
                lastdata[i] =  currentdata[i];
            last_ping_header = sonar->ping_header;

            for(i = 0; i < imagesize; i++)
                currentdata[i] = nextdata[i];
            sonar->ping_header = next_ping_header;

            last_s_int = this_s_int;
            last_p_int = this_p_int;

            recnum++;

            /*
             *    Check to see if the user has hit the cancel button.
             */


            }

    
        position = lseek(outfd, 0L, SEEK_CUR);

        if (put (outfd, &write_offset, currentdata,&sonar->ping_header,
                imagesize) != (imagesize + HEADSIZE))
            {
            sprintf(warningmessage, "Error in output, bytes written = %d", outbytes);
            XtFree((char *)currentdata);
            XtFree((char *)nextdata);
            XtFree((char *)lastdata);
            messages(sonar, warningmessage);
            close(infd);
            close(outfd);
            closeDialog(sonar, destripeDialog, True);
            return;
            }

        pass++;

        sprintf(statusmessage, "End of pass %d -> %d changes", pass, change);

        n = 0;
        XtSetArg(args[n], XmNlabelString,
            XmStringCreateLtoR(statusmessage, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(statusMessage, args, n);
        XmUpdateDisplay(destripeDialog);

        write_offset = read_offset = 0;

        }

    XtFree((char *)currentdata);
    XtFree((char *)nextdata);
    XtFree((char *)lastdata);

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

    closeDialog(sonar, destripeDialog, False);
    
    return;
}

/*    s_integ()
 *
 *    Returns the integral of the starboard side for the sonar data in the 
 *    structure c.
 */
int s_integ (unsigned char *c, int imagesize)
{
    int i, tot;

    tot = 0;
    for (i = 0; i < imagesize/2; i++) 
        tot += c[i];
    return (tot);
}

/*    p_integ()
 *
 *    Returns the integral of the port side for the sonar data in the 
 *    structure c.
 */
int p_integ (unsigned char *c, int imagesize)
{
    int i, tot;

    tot = 0;
    for (i = imagesize / 2; i < imagesize; i++) 
        tot += c[i];
    return (tot);
}

/*    s_interpo()
 *
 *    Interpolates between last and next for starboard side
 */
void s_interpo (unsigned char *this, unsigned char *last, unsigned char *next, int imagesize)
{
    int i;

    for (i = 0; i < imagesize/2; i++) 
        this[i] = (last[i] + next[i]) / 2;

    return;

}
    
/*    p_interpo()
 *
 *    Interpolates between last and next for port side
 */
void p_interpo (unsigned char *this, unsigned char *last, unsigned char *next, int imagesize)
{
    int i;

    for (i = imagesize / 2; i < imagesize; i++) 
        this[i] = (last[i] + next[i]) / 2;

    return;

}

/*    get()
 *
 */
int get (int fd, long int *pos, unsigned char *c, PingHeader *head, int n)
{

    int bytes = 0;
    long filePosition;

    filePosition = lseek (fd, *pos, 0);
    *pos += (long)(n + HEADSIZE);
    bytes += read (fd, head, HEADSIZE);
    bytes += read (fd, c, n);
    return(bytes);
}

/*    put()
 *
 *    Write a structure c of n bytes at position pos and update pos
 */
int put (int fd, long int *pos, unsigned char *c, PingHeader *head, int n)
{

    int bytes = 0;
    long filePosition;

    filePosition = lseek (fd, *pos, 0);
    *pos += (long)(n + HEADSIZE);

    bytes += write (fd, head, HEADSIZE);
    bytes += write (fd, c, n);

    return(bytes);
}
