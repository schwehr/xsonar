/* 
*    beam.c 
*
*    program to accomplish beam pattern correction of a sonar 
*    data file. Output file name will have a "b" appended to the end for
*    "beam-corrected". Sends to stdout also the table of pixel intensity
*    vs. angle for port and starboard.
*
*    Modified by ME from smbeam.c by A. Malinverno and Tom Reed
*    Dec 1988
*
*    Modified by William Danforth, U.S. Geological Survey, AMG.
*    July 1990.  Performs a spline interpolation for pixel values
*    between each 1 degree bin.
*
*    Modified by William Danforth, U.S. Geological Survey, AMG.
*    July 1992.  Ported to Motif as a callback for xsonar.
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include <map.h>
#include "xsonar.h"
#include "sonar_struct.h"


#define        MAXDN        254
#define        MINDN        1
#define        GAP            0.0

void beam(UNUSED Widget w, XtPointer client_data,
	  UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget beamDialog;
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


    unsigned char *indata;
    char pingmessage[400];
    char statusmessage[400];
    char warningmessage[400];
    char outputfile[255];    /* output file name */
    char tempText[100];

    int x, y, z, totalRecords;
    /*int counter;*/

    int imagesize;
    int scansize;
    int halfscan;
    int outbytes;
    int cancelCheck;
    int checkFileSize;
    int infd;
    int outfd;

    int i, i0, j, j0, k;
    int recnum;            /* current ping number */
    int iangle;            /* integer incidence angle */
    int corrected;         /* pixel value after beam pattern correction */
    int port_num_angles;        /* number of angles for spline interp */

    /* FIX: can be used uninitialized */
    int prev_port_num_angles;        /* number of angles for spline interp */
    int stbd_num_angles;        /* number of angles for spline interp */

    /* FIX: can be used uninitialized */
    int prev_stbd_num_angles;        /* number of angles for spline interp */
    int stbd_max_angle;     /* maximum grazing angle */
    int port_max_angle;     /* maximum grazing angle */
    int endOfFile;         /* flag to check for end of processing loop */
    int numProcessed;      /* gets set when EOF is reached.  */
                           /* both of the above are used to ensure the 
                            * correct number gets displayed in the status
                            * window indicating num pings processed.
                            */

    int passedFirstGroup;      /* counter for applying rolling response */

    double correction_port;    /* correction factor for port pixels */
    double correction_stbd;    /* correction factor for stbd pixels */

    off_t currentPosition;      /* position holders for lseek        */
    off_t writePosition;
    off_t position;

    long portBeamSum[900];    /* sum of pixels for angles 0 to 90 (port) */
    long stbdBeamSum[900];    /* sum of pixels for angles 0 to 90 (stbd) */
    long portBeamCount[900];    /* count of angles 0 to 90 (port) */
    long stbdBeamCount[900];    /* count of angles 0 to 90 (stbd) */
    long stbd_total;       /* sum of all pixel values (stbd) */
    long port_total;       /* sum of all pixel values (port) */
    long stbd_total_knt;   /* total count of stbd pixel values */
    long port_total_knt;   /* total count of port pixel values */
    long file_size;

    double beamPar;        /* beam correction when applying rolling response */
    double previousPortBeamPar[900];
    double previousStbdBeamPar[900];
    double portBeamPar[900];  /* port parameters for 0 to 90 */
    double stbdBeamPar[900];  /* stbd parameters for 0 to 90 */
    double portAngleResponse;
    double oldPortAngleResponse;
    double stbdAngleResponse;
    double oldStbdAngleResponse;
    /*double currentResponse;*/
    /*double responseAngle;*/   /* Holder for sonar->beamResponseAngle * 10  */
    double angle;          /* incidence angle */
    /*double splineAngle;*/    /* incidence angle */
    double port_angles[900];    /* array of angle values */
    double previous_port_angles[900];    /* array of angle values */
    double stbd_angles[900];    /* array of angle values */
    double previous_stbd_angles[900];    /* array of angle values */
    double stbd_coef[900]; /* spline coefficient for starboard */
    double previous_stbd_coef[900]; /* spline coefficient for starboard */
    double port_coef[900]; /* spline coefficient for port */
    double previous_port_coef[900]; /* spline coefficient for port */

    double previousBeamSpline;
    double thisBeamSpline;

    int port_angle_count[900];
    int stbd_angle_count[900];

    double stbd_aver;      /* average pixel value (stbd) */

    /* FIX: can be used uninitialized */
    double old_stbd_aver;      /* average pixel value (stbd) */

    double current_stbd_aver;      /* average pixel value (stbd) */
    double port_aver;      /* average pixel value (port) */

    /* FIX: can be used uninitialized */
    double old_port_aver;      /* average pixel value (stbd) */

    double current_port_aver;      /* average pixel value (stbd) */
    double image_aver;    /* average pixel value for image */

    /* FIX: can be used uninitialized */
    double old_image_aver;    /* average pixel value for image */

    /* FIX: can be used uninitialized */
    double current_aver;    /* average pixel value for image */

    double port_adjust;   /* factor to scale port side based on image average */
    double stbd_adjust;   /* factor to scale stbd side based on image average */
    double hor_range;    /* horizontal range */
    double gap;        /* # of meters to ignore around nadir */
    double half_gap;        /* gap / 2 */
    double dec_pixsize;    /* across track pixel size in meters */

    /*double fishDepth;*/

    /*double startValue, stepValue;*/

    /*double slantAdjust, percentAdjust;*/
/*
    int DNsum[900], DNcount[900];
*/

    int swath;        /* swath width in meters */

    struct stat file_params;

    /*
     *   If beam flag was not set in setup, return.
     */

    if(!sonar->beam_flag)
          return;

    /*
    fprintf(stderr, "beam flag is %d\n", sonar->beam_flag);
    */

    /*
     *   If the number of pings to use in the correction is 0, return.
     */

    if(!sonar->beamNumLines)
        {
        sprintf(warningmessage, "Chose the number of pings to use for beam\ncorrection under the Options button in Setup.");
        messages(sonar, warningmessage);
        return;
        }


    /*
     *    Start creating widgets for this app.
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    beamDialog = XtCreatePopupShell("BeamDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                     beamDialog, args, n);
    XtManageChild(statusWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Applying Beam Pattern Correction",
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

    XtRealizeWidget(beamDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(beamDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(beamDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(beamDialog, args, n);

    XtPopup(beamDialog, XtGrabNone);

    /*
     *    Set swath width to sonar->swath.
     */

    if(!sonar->swath)
        {
        sprintf(warningmessage, "Chose setup and select a swath width");
        messages(sonar, warningmessage);
        closeDialog(sonar, beamDialog, True);
        return;
        }

    swath = sonar->swath;


    /*
     *    Open input and output files.
     */

     if((infd = open(sonar->infile, O_RDONLY)) == -1)
          {
          sprintf(warningmessage, "Error opening input file");
          messages(sonar, warningmessage);
          closeDialog(sonar, beamDialog, True);
          return;
          }

     /* sprintf (outputfile, "%sb\0", sonar->infile); FIX: was the \0 needed? */
     sprintf (outputfile, "%sb", sonar->infile);

     if((outfd = open(outputfile, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
          {
          sprintf(warningmessage, "Error opening output file");
          messages(sonar, warningmessage);
          close(infd);
          closeDialog(sonar, beamDialog, True);
          return;
          }

     /*
      *   Allocate memory for input and output ping.
      *   Make sure the file can be read.
      */

    if (read (infd, &sonar->ping_header, 256) != 256)
        {
        sprintf(warningmessage, "cannot read first header");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, beamDialog, True);
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
        closeDialog(sonar, beamDialog, True);
        return;
        }


    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - HEADSIZE;
    halfscan = imagesize / 2;

    indata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
    if(indata == NULL)
        {
        sprintf(warningmessage, "Not enough memory for input sonar array");
        messages(sonar, warningmessage);
        close(infd);
        close(outfd);
        closeDialog(sonar, beamDialog, True);
        return;
        }

    gap = GAP;
    half_gap = gap / 2.0;

    /*    
     *    Read input file and compute sums
     */


    j0 = halfscan;
    i0 = j0-1;

    if((checkFileSize = stat(sonar->infile, &file_params)) == -1)
        {
        sprintf(warningmessage, "Cannot read data file size");
        messages(sonar, warningmessage);

        XtFree((char *) indata);
        close(infd);
        close(outfd);
        closeDialog(sonar, beamDialog, False);
        return;
        }

    /*
     *   Initialize variables
     */

    file_size = file_params.st_size;

    totalRecords = file_size / scansize;

    dec_pixsize =  (double)swath / (double)imagesize;

    currentPosition = lseek(infd, 0L, SEEK_SET);

    recnum = 0;
    endOfFile = False;
    numProcessed = 0;
    passedFirstGroup = 0;


    /*
     *  Now start looping through the file performing beam
     *  corrections.
     */

/*
    sonar->maxBeamAngle *= 10;
*/

    for(x = 0; x < totalRecords; 
        x+=sonar->beamNumLines - sonar->beamAverageLines)
        {

        /*
         *    Initialize sums, counts, and parameters
         */

        for (i = 0; i < 90; i++)
            {
            portBeamSum[i] = stbdBeamSum[i] = 0;
            portBeamCount[i] = stbdBeamCount[i] = 0;
            portBeamPar[i] = stbdBeamPar[i] = 0.0;
            port_angles[i] = i;
            stbd_angles[i] = i;
            port_angle_count[i] = 0;
            stbd_angle_count[i] = 0;
            }

        port_num_angles = stbd_num_angles = 0;
        port_max_angle = stbd_max_angle = 0;

        port_total = stbd_total = port_total_knt = stbd_total_knt = 0;

        currentPosition = lseek(infd, 0L, SEEK_CUR);

        sprintf(statusmessage, "Computing sums ...");
        n = 0;
        XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR(statusmessage, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(statusMessage, args, n);
        XmUpdateDisplay(beamDialog);

        for(y = 0; y < sonar->beamNumLines; y++)
            {

            
            if((read (infd, &sonar->ping_header, 256) == 256) &&
            (read (infd, indata, imagesize) == imagesize))
                {
                recnum++;

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
                    close(infd);
                    close(outfd);
                    closeDialog(sonar, beamDialog, True);
                    return;
                    }

                if(!(recnum % 100))
                    {
                    sprintf(pingmessage, "Processing Ping # %d", recnum);
        
                    n = 0;
                    XtSetArg(args[n], XmNlabelString, 
                                       XmStringCreateLtoR(pingmessage,
                                       XmSTRING_DEFAULT_CHARSET)); n++;
                    XtSetValues(displayPings, args, n);
                    XmUpdateDisplay(beamDialog);
                    }

/*
                surfaceReflectionAngle = acos((double) sonar->ping_header.alt
                                          / (double) sonar->ping_header.depth);

                stbdSurfaceReflection = halfscan - (int)
                                ((sin(surfaceReflectionAngle) * (double)
                                sonar->ping_header.depth) / dec_pixsize + 0.5);
                portSurfaceReflection = halfscan + (int) 
                                ((sin(surfaceReflectionAngle) * (double)
                                sonar->ping_header.depth) / dec_pixsize + 0.5);
*/

/*
fprintf(stdout, "angle = %lf, surface reflection = %d, depth = %f\n", RAD_TO_DEG * surfaceReflectionAngle, portSurfaceReflection, sonar->ping_header.depth);
*/

                for (i=i0, j=j0, k=0; j<imagesize; i--, j++, k++)
                    {    
                        hor_range = (double)(k*dec_pixsize+half_gap);

                        checkForNans(sonar);

                        if(sonar->ping_header.alt > 0)
                            {
                            angle = RAD_TO_DEG * atan(hor_range /
                                       (double) sonar->ping_header.alt);
                            }
                        else
                            angle = RAD_TO_DEG * atan(hor_range / 1.0);

/*
                        iangle = (int) (angle);
fprintf(stdout, "angle is %f\n", angle);
*/
                        iangle = (int) (angle + 0.5);


                /*
                 *    Find the average pixel value for port and starboard
                 */

/*
                        if(hor_range < swath / 2)
                            )
                            {
*/
/*
                                && (j < portSurfaceReflection - 10 
                                || j > portSurfaceReflection + 10))
*/
                            if(indata[j] && iangle < sonar->maxBeamAngle)
                                {
                                if((sonar->useBeamLimits && hor_range < 
                                    sonar->ping_header.portBeamLimit) || !sonar->useBeamLimits)
                                    {

                                    portBeamSum[iangle] += indata[j];
                                    portBeamCount[iangle]++;

                                    port_total += indata[j];
                                    port_total_knt++;

                                    if(port_max_angle < iangle)
                                        port_max_angle = iangle;

                                    port_angle_count[iangle]++;

                                    }
                                }

/*
                                && (i < stbdSurfaceReflection - 10 
                                || i > stbdSurfaceReflection + 10))
*/
                            if(indata[i] && iangle < sonar->maxBeamAngle)
                                {
                                if((sonar->useBeamLimits && hor_range < 
                                    sonar->ping_header.stbdBeamLimit) || !sonar->useBeamLimits)
                                    {
                                    stbdBeamSum[iangle] += indata[i];
                                    stbdBeamCount[iangle]++;
    
                                    stbd_total += indata[i];
                                    stbd_total_knt++;
    
                                    if(stbd_max_angle < iangle)
                                        stbd_max_angle = iangle;
    
                                    stbd_angle_count[iangle]++;
                                    }

                                }

                          /*  }    * end of if testing horizontal range */
                    }

                }       /*  end of if statement reading data */
            else
                {
                endOfFile = True;
                numProcessed = y;
                y = sonar->beamNumLines;
                }

            }  /*  end of 1st for loop reading records  */


        /**************************************
         *  Compute average angle parameters  *
         **************************************/

        stbd_aver = (double)stbd_total / (double)stbd_total_knt;
        port_aver = (double)port_total / (double)port_total_knt;

        image_aver = (port_aver + stbd_aver) / 2.0;

        port_adjust = (port_aver - image_aver);
        stbd_adjust = (stbd_aver - image_aver);

/*
fprintf(stdout, "Stbd average = %.1lf, Port average  = %.1lf\n", stbd_aver, port_aver);
fprintf(stdout, "Port average = %.1lf, Old port average  = %.1lf\n", port_aver, old_port_aver);
fprintf(stdout, "Stbd average = %.1lf, Port average  = %.1lf\n", stbd_aver, port_aver);
fprintf(stdout, "image average = %.1lf, Old image average  = %.1lf\n", image_aver, old_image_aver);
fprintf (stdout, "%2d\tport par = %f\tstbd par = %f\n", i, portBeamPar[i], stbdBeamPar[i]);
fprintf(stdout, "Port response at %d degrees = %.1lf, Stbd response at %d degrees = %.1lf\n", sonar->beamResponseAngle, portAngleResponse, sonar->beamResponseAngle, stbdAngleResponse);
*/

        for (i = 0; i < 90; i++)
            {

        /*
         *    Take reciprocals of parameters, so corrections can
         *    be done by multiplication
         */

            if(stbdBeamCount[i] > 0 && stbdBeamSum[i] > 0)
                stbdBeamPar[i] = (double)stbdBeamSum[i] /
                                                  (double)stbdBeamCount[i];

            if(portBeamCount[i] > 0 && portBeamSum[i] > 0)
                portBeamPar[i] = (double)portBeamSum[i] /
                                              (double)portBeamCount[i];

            }

        portAngleResponse = portBeamPar[sonar->beamResponseAngle];
        stbdAngleResponse = stbdBeamPar[sonar->beamResponseAngle];

        port_num_angles = port_max_angle + 1;
        stbd_num_angles = stbd_max_angle + 1;

        if(port_num_angles > 90)
            port_num_angles = 90;
        if(stbd_num_angles > 90)
            stbd_num_angles = 90;

        zspl3(stbd_angles, stbdBeamPar, stbd_coef, stbd_num_angles);
        zspl3(port_angles, portBeamPar, port_coef, port_num_angles);

        /*    
         *    Read again input file and output corrected data
         */

        writePosition = lseek (infd, currentPosition, SEEK_SET);

        sprintf(statusmessage, "Writing corrected data ...");

        n = 0;
        XtSetArg(args[n], XmNlabelString,
             XmStringCreateLtoR(statusmessage, XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetValues(statusMessage, args, n);
        XmUpdateDisplay(beamDialog);

        if(!endOfFile && sonar->beamNumLines < totalRecords)
            recnum -= sonar->beamNumLines;
        else
            recnum -= numProcessed;

        z = 0;

        for(y = 0; y < sonar->beamNumLines; y++)
            {
            if((read (infd, &sonar->ping_header, 256) == 256) &&
            (read (infd, indata, imagesize) == imagesize))
            {
            recnum++;

            if(XtAppPending(sonar->toplevel_app))
                {
                XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
                XmUpdateDisplay(sonar->toplevel);
                }

            XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
            if(cancelCheck)
                {
                XtFree((char *) indata);
                close(infd);
                close(outfd);
                closeDialog(sonar, beamDialog, True);
                return;
                }

            if(!(recnum % 100))
                {
                sprintf(pingmessage, "Writing Ping # %d", recnum);
       
                n = 0;
                XtSetArg(args[n], XmNlabelString, 
                                   XmStringCreateLtoR(pingmessage,
                                   XmSTRING_DEFAULT_CHARSET)); n++;
                XtSetValues(displayPings, args, n);
                XmUpdateDisplay(beamDialog);
                } 

/*
            surfaceReflectionAngle = acos((double) sonar->ping_header.alt
                                          / (double) sonar->ping_header.depth);

            stbdSurfaceReflection = halfscan - (int)
                                ((sin(surfaceReflectionAngle) * (double)
                                sonar->ping_header.depth) / dec_pixsize + 0.5);

            portSurfaceReflection = halfscan + (int) 
                                ((sin(surfaceReflectionAngle) * (double)
                                sonar->ping_header.depth) / dec_pixsize + 0.5);
*/

            for (i=i0, j=j0, k=0; j<imagesize; i--, j++, k++)
                {    
                hor_range = (double)(k*dec_pixsize+half_gap);

                checkForNans(sonar);

                if (sonar->ping_header.alt > 0)
                    angle = RAD_TO_DEG * atan(hor_range /
                                       (double) sonar->ping_header.alt);
                else
                    angle = RAD_TO_DEG * atan(hor_range / 1.0);

                iangle = (int) (angle + 0.5);
/*
                iangle = (int) (angle);
*/

/******************************** STBD ************************************/

/*
                    && (i < stbdSurfaceReflection - 5 
                    || i > stbdSurfaceReflection + 5))
*/
                if((indata[j] && iangle < sonar->maxBeamAngle) && 
                    ((sonar->useBeamLimits && hor_range < sonar->ping_header.stbdBeamLimit) 
                    || !sonar->useBeamLimits))
                    {
                    if(passedFirstGroup && y < sonar->beamAverageLines)
                        {
                        if(iangle > 40)
                            {
                            previousBeamSpline = spl3(angle, 
                                         previous_stbd_angles, 
                                         previousStbdBeamPar, 
                                         previous_stbd_coef, 
                                         prev_stbd_num_angles);

                            thisBeamSpline = spl3(angle, stbd_angles, 
                                         stbdBeamPar, stbd_coef, 
                                         stbd_num_angles);

                            beamPar = (previousBeamSpline * 
                                        (sonar->beamAverageLines - y) 
                                        / sonar->beamAverageLines) +
                                        (thisBeamSpline * y / 
                                        sonar->beamAverageLines);
                            }
                        else
                            beamPar = (previousStbdBeamPar[iangle] *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (stbdBeamPar[iangle] *
                                        y / sonar->beamAverageLines);

/*
                        currentResponse = (oldStbdAngleResponse *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (stbdAngleResponse *
                                        y / sonar->beamAverageLines);

*/
                        current_aver = (old_image_aver * 
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (image_aver *
                                        y / sonar->beamAverageLines);

                        current_stbd_aver = (old_stbd_aver * 
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (stbd_aver *
                                        y / sonar->beamAverageLines);
/*
                        correction_stbd = currentResponse / beamPar;
*/
                        if(sonar->adjustBeam)
                            correction_stbd = current_aver / beamPar;
                        else
                            correction_stbd = current_stbd_aver / beamPar;

                        corrected = (int) ((indata[i]&0xff) * correction_stbd);

/*
                        if(sonar->normalize)
                            corrected *= sonar->normalize / current_aver;
*/

                        }
                    else
                        {
                        if(iangle > 40)
                            beamPar = spl3(angle, stbd_angles, 
                                             stbdBeamPar, stbd_coef, 
                                             stbd_num_angles);

                        else
                            beamPar = stbdBeamPar[iangle];

/*
                        correction_stbd = stbdAngleResponse / beamPar;
*/
                        if(sonar->adjustBeam)
                            correction_stbd = image_aver / beamPar;
                        else
                            correction_stbd = stbd_aver / beamPar;

                        corrected = (int) ((indata[i]&0xff) * correction_stbd);

/*
                        if(sonar->normalize)
                            corrected *= sonar->normalize / image_aver;
*/
                        }

                    }
                else
                    {
                    correction_stbd = 1.0;
                    corrected = (int) ((indata[i]&0xff) * correction_stbd);

/*
                    if(sonar->normalize)
                        corrected *= sonar->normalize / image_aver;
*/
                    }


                if (corrected > MAXDN) corrected = MAXDN;
                if (corrected < MINDN) corrected = MINDN;

                if(iangle >= sonar->maxBeamAngle)
                    corrected = 255;

                indata[i] = corrected;

/******************************** PORT ************************************/

/*
                    && (j < portSurfaceReflection - 5 
                    || j > portSurfaceReflection + 5))
*/
                if((indata[j] && iangle < sonar->maxBeamAngle) && 
                    ((sonar->useBeamLimits && hor_range < sonar->ping_header.portBeamLimit) 
                    || !sonar->useBeamLimits))
                    {   

                    if(passedFirstGroup && y < sonar->beamAverageLines)
                        {
                        if(iangle > 40)
                            {
                            previousBeamSpline = spl3(angle,
                                         previous_port_angles,
                                         previousPortBeamPar,
                                         previous_port_coef,
                                         prev_port_num_angles);

                            thisBeamSpline = spl3(angle, port_angles,
                                         portBeamPar, port_coef,
                                         port_num_angles);

                            beamPar = (previousBeamSpline *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (thisBeamSpline * y /
                                        sonar->beamAverageLines);
                            }
                        else
                            beamPar = (previousPortBeamPar[iangle] *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (portBeamPar[iangle] *
                                        y / sonar->beamAverageLines);

/*
                        currentResponse = (oldPortAngleResponse *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (portAngleResponse *
                                        y / sonar->beamAverageLines);

*/
                        current_port_aver = (old_port_aver *
                                        (sonar->beamAverageLines - y)
                                        / sonar->beamAverageLines) +
                                        (port_aver *
                                        y / sonar->beamAverageLines);

/*
                        correction_port = currentResponse / beamPar;
*/
                        if(sonar->adjustBeam)
                            correction_port = current_aver / beamPar;
                        else
                            correction_port = current_port_aver / beamPar;

                        corrected = (int) ((indata[j]&0xff) * correction_port);
/*
                        if(sonar->normalize)
                            corrected *= sonar->normalize / current_aver;
*/

                        }
                    else
                        {
                        if(iangle > 40)
                            beamPar = spl3(angle, port_angles,
                                             portBeamPar, port_coef,
                                             port_num_angles);

                        else
                            beamPar = portBeamPar[iangle];

/*
                        correction_port = portAngleResponse / beamPar;
*/
                        if(sonar->adjustBeam)
                            correction_port = image_aver / beamPar;
                        else
                            correction_port = port_aver / beamPar;

                        corrected = (int) ((indata[j]&0xff) * correction_port);

/*
                        if(sonar->normalize)
                            corrected *= sonar->normalize / image_aver;
*/
                       }
                    }
                else
                    {
                    correction_port = 1.0;
                    corrected = (int) ((indata[j]&0xff) * correction_port);
/*
                    if(sonar->normalize)
                        corrected *= sonar->normalize / image_aver;
*/
                    }

                if (corrected > MAXDN) corrected = MAXDN;
                if (corrected < MINDN) corrected = MINDN;

                if(iangle >= sonar->maxBeamAngle)
                    corrected = 255;

                indata[j] = corrected;

                }

            if((outbytes = write(outfd, &sonar->ping_header, 256)) != 256
            || (outbytes = write(outfd, indata, imagesize)) != imagesize)
                {
                sprintf(warningmessage, "Error writing output file");
                messages(sonar, warningmessage);
                close(infd);
                close(outfd);
                closeDialog(sonar, beamDialog, True);
                return;
                }
            }
            else
                y = sonar->beamNumLines;

            }  /* end of for loop writing records */

        for(iangle = 0; iangle < 90; iangle++)
            {
            previousPortBeamPar[iangle] = portBeamPar[iangle];
            previousStbdBeamPar[iangle] = stbdBeamPar[iangle];
            previous_port_coef[iangle] = port_coef[iangle];
            previous_stbd_coef[iangle] = stbd_coef[iangle];
            previous_port_angles[iangle] = port_angles[iangle];
            previous_stbd_angles[iangle] = stbd_angles[iangle];
            prev_port_num_angles = port_num_angles;
            prev_stbd_num_angles = stbd_num_angles;
            }
      
        old_image_aver = image_aver;
        old_port_aver = port_aver;
        old_stbd_aver = stbd_aver;
        oldPortAngleResponse = portAngleResponse;
        oldStbdAngleResponse = stbdAngleResponse;

        position = (off_t) (sonar->beamAverageLines * scansize * -1);
        currentPosition = lseek(infd, position, SEEK_CUR);
        currentPosition = lseek(outfd, position, SEEK_CUR);

        passedFirstGroup++;

        }      /*end of for loop incrementing x by sonar->beamNumLines */


    XtFree((char *) indata);

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

    closeDialog(sonar, beamDialog, False);

    return;
}
/*
fprintf(stdout, "record number: %d\n", recnum);
fprintf (stdout, "%2d\tPort: %.5f\tStbd: %.5f\n", i, portSlantPar[i], stbdSlantP
ar[i]);
fprintf (stdout, "%2d\tPort:\t%d\t%d\tStbd:\t%d\t%d\n", i, portSlantCount[i], po
rtSlantSum[i], stbdSlantCount[i], stbdSlantSum[i]);
fprintf (stdout, "angle\tstbd beam\tport beam\tstbd slant\tport slant\n\n");

Sum[i], stbdSlantSum[i], stbdBeamSum[i]);
fprintf (stdout, "stbd_aver=%.1f port_aver=%.1f\n", stbd_aver, port_aver);
fprintf(stdout, "\nport adjust = %f\nstbd adjust = %f\n\n", port_adjust, stbd_ad
just);
fprintf(stdout, "rec # %d, pixel # %d\tport correct = %f\tstbd correct = %f\n",
x, i, correction_port, correction_stbd);
*/

/*
long recheckBeamPars(sonar, indata, portBeamPar, newPortBeamPar, newStbdBeamPar, imagesize, dec_pixsize, port_aver, stbd_aver, currentPosition, infd)
MainSonar *sonar;
char *indata;
double *portBeamPar;
double *newPortBeamPar;
double *newStbdBeamPar;
int imagesize;
double dec_pixsize;
double port_aver, stbd_aver;
long currentPosition;
int infd;
{

    int i, j, k, j0, i0, z;
    int iangle;
    int stbd_max_angle;     * maximum grazing angle *
    int port_max_angle;     * maximum grazing angle *
    int inbytes;
    int counter;
    int pingCounter;

    double startValue, stepValue;
 
    double hor_range;
    double angle;

    long portBeamSum[900], stbdBeamSum[900];

    long portBeamCount[900], stbdBeamCount[900];
    long position;


    j0 = imagesize / 2;
    i0 = j0 -1;
    port_max_angle = 0;
    counter = 1;
     
    for (i = 0; i < 900; i++)
        {
        portBeamSum[i] = stbdBeamSum[i] = 0;
        portBeamCount[i] = stbdBeamCount[i] = 0;
        newPortBeamPar[i] = newStbdBeamPar[i] = 0.0;
        }

    position = lseek(infd, currentPosition, SEEK_SET);
    pingCounter = 0;

    while(pingCounter < 300)
    {    
    inbytes = read (infd, &sonar->ping_header, 256);
    inbytes = read (infd, indata, imagesize);

    position = lseek(infd, 0L, SEEK_CUR);

    for (i=i0, j=j0, k=0; j<imagesize; i--, j++, k++)
        {
        hor_range = (double)(k*dec_pixsize);

        if(sonar->ping_header.alt > 0)
            {
            angle = RAD_TO_DEG * atan(hor_range /
                           (double) sonar->ping_header.alt);
            }
        else
            angle = RAD_TO_DEG * atan(hor_range / 1.0);

        iangle = (int) (angle);

        if(indata[j])
            {

            portBeamSum[iangle] += indata[j];
            portBeamCount[iangle]++;

            if(port_max_angle < iangle)
                port_max_angle = iangle;

            }

        if(indata[i])
            {
            stbdBeamSum[iangle] += indata[i];
            stbdBeamCount[iangle]++;
            }

        }
pingCounter++;
    }

    for (i = 0; i < 900; i++)
        {

        *
         *    Take reciprocals of parameters, so corrections can
         *    be done by multiplication
         *


        if(portBeamCount[i] > 0 && portBeamSum[i] > 0)
            {
            newPortBeamPar[i] = (double)portBeamSum[i] /
                                          (double)portBeamCount[i];
            newPortBeamPar[i] /= port_aver;
            newPortBeamPar[i] = 1.0/newPortBeamPar[i];
            }

fprintf (stdout, "\tPort:sum = %d  count = %d\n", portBeamSum[i], portBeamCount[i]);
fprintf (stdout, "%d\tPort:\told par = %f\tnew par = %f\n", i, portBeamPar[i], newPortBeamPar[i]);
        }

        for (i = port_max_angle; i > 0; i -= counter)
            {

            if(i <= 0)
                break;

            counter = 1;


            if(portBeamPar[i - 1] == 0)
                {

                while(portBeamPar[i - counter] == 0)
                    {
                    if((i - counter) == 0)
                        break;
                    counter++;
                    }

                stepValue = (portBeamPar[i] - portBeamPar[i - counter]) 
                                                                 / counter;
                startValue = portBeamPar[i - counter];

                for(z = counter - 1; z >= 0; z--)
                    portBeamPar[i - z] = startValue + stepValue * z;

                }

            }


    return(position);

}


*/
