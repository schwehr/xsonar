/* 
    orient.c

    Plots the sonar data along a sinuous track for some given pixel size. 
    It takes as input a file merged with navigation. 

    NEW: fills a polygon in an image array, and then displays the whole
    image array. Supposed to produce a smoother image
    at small pix_size and to go faster at large pix_size.

    Original code written for the MASSCOMP by:
       Alberto Malinverno
       Last modification: Oct 20 1987 

    Rewritten and converted to Motif by:
    William W. Danforth
    U.S. Geological Survey

    Jul 11, 1992
 */

#include <unistd.h>

#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>

#include <map.h>

#include "xsonar.h"

#include "sonar_struct.h"

void mapDisplay(Widget w, XtPointer client_data, XtPointer call_data);
void mapClose(Widget w, XtPointer client_data, XtPointer call_data);
void GetUtmText(Widget w, XtPointer client_data, XtPointer call_data);
/*void doMap();*/
void utmGrid(MainSonar *sonar, int grid_int_ew, int grid_int_ns, ImageHeader *image_params);
void merGrid(MainSonar *sonar, int dgrid_sec, ImageHeader *image_params, ImageBounds *image_bounds);
Boolean checkTheQueue(MainSonar *sonar);


typedef struct {                /* polygon vertex structure */
    int x;
    int y;
    unsigned char z;
    int x_max;
    int x_min;
    double a;
    double b;
} PixelVertex;


void mapCoast(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget mapWindow;
    Widget mapDialog;
    Widget showProgress;
    /*Widget infoframe;*/
    /*Widget topmessage;*/
    Widget /*separator1,*/ separator2, separator3;
    Widget displaytime;
    /*Widget infomessage;*/
    Widget statusmessage;
    Widget drawframe;
    Widget runButton;
    Widget cancelButton;
    Widget closeButton;
    Widget utmLabel[2];
    Widget utmText[2];

    Arg args[10];
    /*Status status;*/
    Cardinal n;
    /*Window root;*/

    XFontStruct *label_font;

    char warningmessage[50];
    char widgetName[50];

    /*unsigned int width, height, depth, border_width;*/
    /*int i, j, c, base_x, base_y;*/
    int i;

    /*void messages();*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog.
     */

    XtSetSensitive(w, FALSE);

    /*   
     *     Check to make sure the selected file can be opened (exists).
     */

    if ((sonar->infd = open (sonar->infile, 0)) == -1)
          {
          sprintf(warningmessage, 
                    "Cannot open input file\nPlease select a file in Setup");
          messages(sonar, warningmessage);
          XtSetSensitive(w, True);
          return;
          }
    close(sonar->infd);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    mapDialog = XtCreatePopupShell("MapDialog",
              transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    mapWindow = XtCreateWidget("Window", xmFormWidgetClass,
                  mapDialog, args, n);
    XtManageChild(mapWindow);

    for(i = 0; i < 2; i++)
        {
        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        sprintf(widgetName, "UtmLabel%d", i);
        utmLabel[i] = XtCreateWidget(widgetName, xmLabelWidgetClass,
                                     mapWindow, args, n);
        XtManageChild(utmLabel[i]);

        n = 0;
        XtSetArg(args[n], XmNwidth, 20); n++;
        sprintf(widgetName, "UtmText%d", i);
        utmText[i] = XtCreateWidget(widgetName, xmTextFieldWidgetClass, 
                                          mapWindow, args, n);

        XtManageChild(utmText[i]);
        XtAddCallback(utmText[i], XmNactivateCallback, 
                                      GetUtmText, sonar);
        }

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetValues(utmLabel[0], args, n);
    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, utmLabel[0]); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetValues(utmText[0], args, n);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, utmLabel[0]); n++;
    XtSetValues(utmLabel[1], args, n);
    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, utmLabel[1]); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, utmText[0]); n++;
    XtSetValues(utmText[1], args, n);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Initializing display",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, utmLabel[1]); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    statusmessage = XtCreateWidget("Status",
                    xmLabelWidgetClass, mapWindow, args, n);
    XtManageChild(statusmessage);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, statusmessage); n++;
    separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                            mapWindow, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Julian Day 000 / 00:00:00",
        XmSTRING_DEFAULT_CHARSET)); n++;
    displaytime = XtCreateWidget("DisplayTime",
                xmLabelWidgetClass, mapWindow, args, n);
    XtManageChild(displaytime);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, displaytime); n++;
    separator3 = XtCreateWidget("Separator3", xmSeparatorWidgetClass,
                            mapWindow, args, n);
    XtManageChild(separator3);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    runButton = XtCreateWidget("RunButton",
                xmPushButtonWidgetClass, mapWindow, args, n);
    XtManageChild(runButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, runButton); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNwidth, 200); n++;
    drawframe = XtCreateWidget("drawFrame",
                xmFrameWidgetClass, mapWindow, args, n);
    XtManageChild(drawframe);

    showProgress = XtCreateWidget("ShowProgress", xmDrawingAreaWidgetClass,
                    drawframe, args, n);
    XtManageChild(showProgress);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, drawframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Stop",
                    XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, mapWindow, args, n);
    XtManageChild(cancelButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, cancelButton); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Close",
                    XmSTRING_DEFAULT_CHARSET)); n++;
    closeButton = XtCreateWidget("CloseButton",
                xmPushButtonWidgetClass, mapWindow, args, n);
    XtManageChild(closeButton);

    XtAddCallback(runButton, XmNactivateCallback,
                                        mapDisplay, sonar);
    XtAddCallback(closeButton, XmNactivateCallback, 
                                        mapClose, sonar);

    XtRealizeWidget(mapDialog);

    XSetForeground(XtDisplay(mapWindow), sonar->main_gc,
             WhitePixelOfScreen(XtScreen(mapWindow)));

    XSetBackground(XtDisplay(mapWindow), sonar->main_gc,
             BlackPixelOfScreen(XtScreen(mapWindow)));

    label_font = XLoadQueryFont(XtDisplay(mapWindow),
            "-adobe-courier-bold-r-normal--12-120-75-75-m-70-iso8859-1");
    if(label_font == 0)
        {
        sprintf(warningmessage, "No font for map, closing window.\n");
        messages(sonar, warningmessage);
        XtSetSensitive(w, True);
        return;
        }

    XSetFont(XtDisplay(mapWindow), sonar->main_gc, label_font->fid);

    XSetLineAttributes(XtDisplay(mapDialog),sonar->main_gc, 5,
                    LineSolid, CapButt, JoinRound);

    XSetForeground(XtDisplay(mapWindow), sonar->main_gc,
             WhitePixelOfScreen(XtScreen(mapWindow)));

    XSetBackground(XtDisplay(mapWindow), sonar->main_gc,
             BlackPixelOfScreen(XtScreen(mapWindow)));

/*
    XFillRectangle(XtDisplay(mapWindow), sonar->orient_pixmap,
                    sonar->main_gc, 0, 0, 1728, 1728);
*/


    XtPopup(mapDialog, XtGrabNone);

/*
fprintf(stderr, "pixmap resources:\nroot window = %d\n", root);    
fprintf(stderr, "base x = %d, base y = %d\n", base_x, base_y);
fprintf(stderr, "width = %d, height = %d\n", width, height);
fprintf(stderr, "border width = %d, depth = %d\n", border_width, depth);
fprintf(stderr, "creating fonts ... ");
fprintf(stderr, "done.\n");
fprintf(stderr, "cancel button window = %d\n", XtWindow(cancelButton));    
fprintf(stderr, "orient dialog window = %d\n", XtWindow(orientDialog));    
fprintf(stderr, "form window = %d\n", XtWindow(window));    
*/


}

void mapDisplay(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    XImage *x_image;
    XmString time_string;
    XmString info_string;
    Arg args[20];
    Cardinal n;

    FILE *coastFile;

    char time[40];
    char information[40];
    char warningmessage[50];
    char lonChar[20];
    char latChar[20];
    char jday[20];
    char hr[20];
    char mn[20];
    char sc[20];
    char depth[20];

    /*unsigned char *current_image;*/
    /*unsigned char *next_image;*/
    /*unsigned char *pad_image;*/

    ImageBounds image_bounds;
    ImageHeader image_params;

    double nextEast, nextNorth;
    double nextLon, nextLat;
    double currentEast, currentNorth;
    double currentLon, currentLat;
    /*double delta_sec;*/
    /*double sec;*/

    int x1, y1, x2, y2;
    /*int position;*/
    int  mer_grid_int;
    /*int hour, min;*/
    int outbytes;
    /*int blocky_flag = 0;*/
    int i;/*, j;*/
    /*int image_num;*/
    int image_h, image_w;            /* height and width (pix.) of image */
    int xl, yb, xr, yt;                /* printer boundaries */
    int check; /* FIX: can be used uninitialized */
    int record;
    int items;
    int result;
    int eofCheck;

    char time_label[100];
    int julian, plotHour, plotMin, plotSecond;
    double totalDepth;

    long filePosition;

    int utm_grid_int_north;
    int utm_grid_int_west;

    /*void messages();*/
    

    /*Boolean status;*/


    xl = 0;
    xr = 1727;
    yb = 0;
    yt = 1727;

    if((coastFile = fopen(sonar->infile, "r")) == NULL)
        {
        sprintf(warningmessage, "Could not open ascii navigation file.");
        messages(sonar, warningmessage);
        return;
        }

    /*
     *   Check that a map scale has been selected, and that the map
     *   area has been outlined.

    if(!sonar->number_of_images)
        {
        sprintf(information, "Need to outline the map area in Navigation.");
        messages(sonar, information);
        return;
        }

     */

    if(!sonar->scale)
        {
        sprintf(information, "No map scale, select a map scale in Setup.");
        messages(sonar, information);
        return;
        }

    /*
     *   De-sensitize the Run button and Close button.
     */

    XtSetSensitive(w, False);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                     "*MapDialog*CloseButton"), False);
   
    image_w = xr - xl + 1;
    image_h = yt - yb + 1;

    image_params.width = image_w;
    image_params.height = image_h;

    sonar->orient_pixmap = XCreatePixmap(XtDisplay(sonar->toplevel),
                          RootWindow(XtDisplay(sonar->toplevel),
                          DefaultScreen(XtDisplay(sonar->toplevel))),
                          (unsigned) image_w, (unsigned) image_h, 8);

fprintf(stdout, "Initializing image space\n");
        sonar->image = (unsigned char *)
                        XtCalloc(image_w*image_h, sizeof(unsigned char));

        for (i = 0; i < (image_w*image_h); i++)
            sonar->image[i] = 255;

        x_image = XCreateImage(XtDisplay(sonar->toplevel),
                DefaultVisualOfScreen(XtScreen(sonar->toplevel)),
                8, ZPixmap, 0, (char *) sonar->image, 1728, 1728, 8, 1728);

        XPutImage(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                           sonar->main_gc, x_image, 0, 0, 0, 0, 1728, 1728);

fprintf(stdout, "done\n");



    /*
     *    Loop until the number of images requested is processed.
     */ 

        if((sonar->outfile = (char *) XtCalloc(1,
                                   strlen(sonar->infile) + 10)) == NULL)
            {
            sprintf(information, "Could not allocate memory output filename.");
            messages(sonar, information);
            XtSetSensitive(w, True);
            XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                     "*MapDialog*CloseButton"), False);
            return;
            }

        strcpy(sonar->outfile, sonar->infile);

        for(i = 0; i <(int)(strlen(sonar->infile)); i++)
            {
            if(sonar->outfile[i] == '.')
                {
                sonar->outfile[i + 1] = '\0';
                i = strlen(sonar->infile);
                }
            }

        strcat(sonar->outfile,"mapimg");

        if((sonar->outfd = creat(sonar->outfile, PMODE)) < 0)
            {
            sprintf(information, "Error opening output file.");
            messages(sonar, information);
            XtSetSensitive(w, True);
            XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                         "*MapDialog*CloseButton"), True);

            return;
            }
        else
            sonar->outfd = open(sonar->outfile, O_RDWR);

        XtFree((char *)sonar->outfile);

        /*    
         *    Allocate storage for input data.
         */

        image_params.pix_size = sonar->pix_s = (sonar->scale / 200) * 0.0254;
        sonar->pix_ew = ((double)sonar->scale / 203.0) * 0.0254;
        sonar->pix_ns = ((double)sonar->scale / 202.0) * 0.0254;

        if (sonar->pix_s == 0.0)
            {
            fprintf(stderr, "Please enter a map scale.\n");
            exit(-1);
            }


        /*    Reads first data record to get screen bounds if necessary
         *    Compute UTM bounds of the screen
         *
         */


        sonar->south = sonar->utm_south[1];
        sonar->west = sonar->utm_west[1];

        if(!sonar->c_lon)
            {
            sprintf(warningmessage, 
                        "Please enter a central longitude in setup.");
            messages(sonar, warningmessage);
            XtSetSensitive(w, True);
            XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                         "*MapDialog*CloseButton"), True);

            return;
            }

        /*
         *   Set up UTM map parameters to be passwd to disp_swath
         *   and the gridding routines.
         */

        image_params.west = sonar->west;
        image_params.south = sonar->south;

        filePosition = lseek (sonar->infd, 0L, 0);        
        sonar->north = sonar->south + (image_h * sonar->pix_s);
        sonar->east = sonar->west + (image_w * sonar->pix_s);

        utm_grid_int_west = (int)((sonar->pix_ew * 203.0) + 0.5);
        utm_grid_int_north = (int)((sonar->pix_ns * 202.0) + 0.5);

        if(sonar->mer_grid_int)
            mer_grid_int = sonar->mer_grid_int;
        else
            mer_grid_int = (int)(((sonar->pix_s * 200) * 2 / 1852.0) * 60);

        /*
         *    Set bounds of box for in_box()
         *    and of image for in_image()
         */

        image_bounds.west = sonar->west;
        image_bounds.south = sonar->south;
        image_bounds.east = sonar->east;
        image_bounds.north = sonar->north;
        image_bounds.x_min = xl;
        image_bounds.y_min = yb;
        image_bounds.x_max = xr;
        image_bounds.y_max = yt;

fprintf(stdout, "west = %f\n", image_bounds.west);
fprintf(stdout, "east = %f\n", image_bounds.east);
fprintf(stdout, "north = %f\n", image_bounds.north);
fprintf(stdout, "south = %f\n", image_bounds.south);

        /*
         *    Read first record to start 
         */

        items = fscanf(coastFile, "%s %s %s %s %s %s %s", 
                           jday, hr, mn, sc, lonChar, latChar, depth );
fprintf(stdout, "lonchar = %s\tlatchar = %s\n", lonChar, latChar);
        if((result = strcmp(jday,"#")) == 0)
            {
            items = fscanf(coastFile, "%s %s %s %s %s %s %s", 
                           jday, hr, mn, sc, lonChar, latChar, depth );
            fprintf(stdout, "lonchar = %s\tlatchar = %s\n", lonChar, latChar);
            }

        julian = atoi(jday);
        plotHour = atoi(hr);
        plotMin = atoi(mn);
        plotSecond = atoi(sc);
        currentLon = atof(lonChar);
        currentLat = atof(latChar);
        totalDepth = atof(depth);

fprintf(stdout, "result = %d\n", result);
fprintf(stdout, "items = %d\n", items);
fprintf(stdout, "lon = %f\n", currentLon);
fprintf(stdout, "lat = %f\n", currentLat);

        geoutm (currentLat, currentLon, sonar->c_lon, &currentEast, &currentNorth, sonar->ellipsoid);

fprintf(stdout, "utm lon = %f\n", currentEast);
fprintf(stdout, "utm lat = %f\n", currentNorth);


        xy_raster(sonar, currentEast, currentNorth, &x1, &y1, TDU);
fprintf(stdout, "x1 = %d, y1 = %d\n", x1, y1);

        sprintf(time_label, "%03d %02d:%02d%02d", 
                             julian, plotHour, plotMin, plotSecond);

        XDrawString(XtDisplay(sonar->toplevel),
                     sonar->orient_pixmap, sonar->main_gc, x1, y1,
                     time_label, strlen(time_label));

        record = 1;

        /*
         *    Loop until end of input data file
         */

        sprintf(information, "Mapping in progress");
        info_string = XmStringCreateLtoR(information,XmSTRING_DEFAULT_CHARSET);

        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel, 
                                          "*MapDialog*Status"), args, n);
        XmStringFree(info_string);

        eofCheck = feof(coastFile);
        while(!eofCheck)
            {

            items = fscanf(coastFile, "%s %s %s %s %s %s %s", 
                           jday, hr, mn, sc, lonChar, latChar, depth );

            julian = atoi(jday);
            plotHour = atoi(hr);
            plotMin = atoi(mn);
            plotSecond = atoi(sc);
            totalDepth = atof(depth);

            if((result = strcmp(lonChar, "#")) == 0)
                {
                items = fscanf(coastFile, "%s %s %s %s %s %s %s", 
                           jday, hr, mn, sc, lonChar, latChar, depth );
                nextLon = atof(lonChar);
                nextLat = atof(latChar);
                }
            else
                {
                nextLon = atof(lonChar);
                nextLat = atof(latChar);
                }

/*
fprintf(stdout, "\n");
fprintf(stdout, "items = %d\n", items);
fprintf(stdout, "lon = %f\n", nextLon);
fprintf(stdout, "lat = %f\n", nextLat);
fprintf(stdout, "east = %f\n", nextEast);
fprintf(stdout, "north = %f\n", nextNorth);
*/


            record++;

            eofCheck = feof(coastFile);

            if(!eofCheck)
                {

                check = 1;

                geoutm(currentLat, currentLon, -75.0, 
                                        &currentEast, &currentNorth, sonar->ellipsoid);
                geoutm(nextLat, nextLon, -75.0, &nextEast, &nextNorth, sonar->ellipsoid);
/*
fprintf(stdout, "current east = %f\n", currentEast);
fprintf(stdout, "current north = %f\n", currentNorth);
fprintf(stdout, "next east = %f\n", nextEast);
fprintf(stdout, "next north = %f\n", nextNorth);
fprintf(stdout, "x1 = %d\ty1 = %d\n", x1, y1);
fprintf(stdout, "x2 = %d\ty2 = %d\n", x2, y2);
*/
fprintf(stdout, "x1 = %d\ty1 = %d\n", x1, y1);
fprintf(stdout, "x2 = %d\ty2 = %d\n", x2, y2);


                if(in_box(currentEast, currentNorth, &image_bounds))
                    {

                    if(!(record % 500))
                        {
                        sprintf(time, "Mapping record # %d", record);
                        time_string = XmStringCreateLtoR(time,
                                    XmSTRING_DEFAULT_CHARSET);
    
                        n = 0;
                        XtSetArg(args[n], XmNlabelString, time_string); n++;
                        XtSetValues(XtNameToWidget(sonar->toplevel, 
                                           "*MapDialog*DisplayTime"), args, n);
                        XmUpdateDisplay(sonar->toplevel);
                        XmStringFree(time_string);
                        }
    
                    xy_raster(sonar, currentEast, currentNorth, &x1, &y1, TDU);
                    xy_raster(sonar, nextEast, nextNorth, &x2, &y2, TDU);
    
                    XDrawLine(XtDisplay(sonar->toplevel), sonar->orient_pixmap, 
                                        sonar->main_gc, x1, y1, x2, y2);

        sprintf(time_label, "%03d %02d:%02d%02d", 
                             julian, plotHour, plotMin, plotSecond);

        XDrawString(XtDisplay(sonar->toplevel),
                     sonar->orient_pixmap, sonar->main_gc, x1, y1,
                     time_label, strlen(time_label));

    
                    }
                }

            if(checkTheQueue(sonar))
                {
                XtSetSensitive(w, True);
                XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                              "*MapDialog*CloseButton"), True);
                return;
                }

            while(check == 1)
                {
                if(XtAppPending(sonar->toplevel_app))
                    {
                    XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
                    XmUpdateDisplay(sonar->toplevel);
                    }
                else
                    check = 0;
                }

            currentLat = nextLat;
            currentLon = nextLon;
            }

        /*
         *     Display image, grids and exit
         */

        XSetLineAttributes(XtDisplay(w),sonar->main_gc, 2,
                    LineSolid, CapButt, JoinRound);

        if(sonar->utm_flag)
            utmGrid(sonar, utm_grid_int_west,utm_grid_int_north, &image_params);

        if(sonar->mercator_flag)
            merGrid(sonar, mer_grid_int, &image_params, &image_bounds);

        sprintf(information, "copying pixmap to ximage ...");
        info_string = XmStringCreateLtoR(information, XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel,"*MapDialog*Status"),args,n);
        XmStringFree(info_string);
        XmUpdateDisplay(sonar->toplevel);

        XFree((char *)x_image);
        XtFree((char *)sonar->image);

        x_image = XGetImage(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                                   0, 0, 1728, 1728, AllPlanes, ZPixmap);


        sprintf(information, "Processing completed ... Writing image");
        info_string = XmStringCreateLtoR(information, XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel,"*MapDialog*Status"),args,n);
        XmStringFree(info_string);
        XmUpdateDisplay(sonar->toplevel);

        for(i = 0; i < image_w * image_h; i++)
            sonar->image[i] = ~sonar->image[i];

        if((outbytes = write(sonar->outfd, x_image->data,
                            image_w * image_h)) != image_w * image_h)
            {
            fprintf(stderr, "Error on output, record length: %d\n", outbytes);
            fprintf(stderr, "Exiting ...\n");
            exit(-1);
            }

        sprintf(information, "Exiting");
        info_string = XmStringCreateLtoR(information, XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel,"*MapDialog*Status"),args,n);
        XmStringFree(info_string);
        XmUpdateDisplay(sonar->toplevel);

        XFree((char *)x_image);
        XtFree((char *)sonar->image);


        close(sonar->outfd);
        fclose(coastFile);


    XtSetSensitive(w, True);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                      "*MapDialog*CloseButton"), True);

    return;

}

Boolean checkTheQueue(MainSonar *sonar)
{

    XEvent event;
    Boolean status;
    Widget cancelButton;
    Widget mapDialog;
    char information[40];
    Arg args[5];

    int n;

    XmString info_string;
    XmString string1;
    XmString string2;

    String widget_string;
    String compare_string = "CancelButton";

    status = False;

    mapDialog = XtNameToWidget(sonar->toplevel, "*MapDialog");

    cancelButton = XtNameToWidget(sonar->toplevel, "MapDialog*CancelButton");
    widget_string = XtName(cancelButton);

    XCheckWindowEvent(XtDisplay(mapDialog),XtWindow(cancelButton),
                         ButtonReleaseMask, &event);

    string1 = XmStringCreateLtoR(widget_string,
                                XmSTRING_DEFAULT_CHARSET);
    string2 = XmStringCreateLtoR(compare_string,
                                XmSTRING_DEFAULT_CHARSET);

    if(XmStringByteCompare(string1, string2)
        && event.xbutton.type == ButtonRelease
        && event.xbutton.button == Button1)
        {
        status = True;

        sprintf(information, "Mapping Halted");
        info_string = XmStringCreateLtoR(information,
                                XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel, 
                                     "*MapDialog*Status"), args, n);
        XmStringFree(info_string);

        XSendEvent(XtDisplay(mapDialog), XtWindow(cancelButton),
                      True, ButtonReleaseMask, &event);

        XmUpdateDisplay(mapDialog);
        }

    XmStringFree(string1);
    XmStringFree(string2);


    return(status);

}    

/*
void set_calling_widget(w, client_data, call_data)
Widget     w;
XtPointer    client_data;
XtPointer    call_data;
{

    Widget calling_widget = (Widget) client_data;

    XtSetSensitive(calling_widget, True);
    return;
}
*/

void mapClose(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget parent_widget;

    parent_widget = w;

    while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);

    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*MapButton"), True);

    close(sonar->infd);

    return;


}


/*****************************************************************************
 *
 *    utm_grid()
 *
 *    Plots and labels a UTM grid on the screen
 */
void utmGrid (MainSonar *sonar, int grid_int_ew, int grid_int_ns,
	       UNUSED  ImageHeader *image_params)
{
    char grid_label [20];
    int lab_off = 8;    /* offset of the grid labels */
    int e, w, s, n;
    int rastx, rasty;
    double x, y;

    /*
     *    Draw vertical (north - south) grid lines
     */

    x = (1 + (int)(sonar->west) / grid_int_ew) * grid_int_ew;

    while (x < sonar->east)
        {    
        xy_raster (sonar, x, sonar->south, &rastx, &s, TDU);
        xy_raster (sonar, x, sonar->north, &rastx, &n, TDU);

        XDrawLine(XtDisplay(sonar->toplevel), sonar->orient_pixmap, 
                                    sonar->main_gc, rastx, 0, rastx, 1728);

            sprintf (grid_label, "%d", (int)x);


            XDrawString(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                        sonar->main_gc, rastx+lab_off, 1728 - lab_off,
                        grid_label, strlen(grid_label));

        x += grid_int_ew;

        }

    /*
     *    Draw horizontal (east - west) grid lines
     */

    y = (1 + (int)(sonar->south) / grid_int_ns) * grid_int_ns;


    while (y < sonar->north)
        {    
        xy_raster (sonar, sonar->west, y, &w, &rasty, TDU);
        xy_raster (sonar, sonar->east, y, &e, &rasty, TDU);

            sprintf (grid_label, "%d", (int)y);

            XDrawString(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                        sonar->main_gc, 0 + lab_off, rasty - lab_off,
                        grid_label, strlen(grid_label));


        XDrawLine(XtDisplay(sonar->toplevel), sonar->orient_pixmap, sonar->main_gc, w, rasty, e, rasty);

        y += grid_int_ns;

        }


    return;

}

/*****************************************************************************
 *
 *    mer_grid()
 *
 *    Plots and labels a Mercator grid on the screen
 */
void merGrid (MainSonar *sonar, int dgrid_sec, ImageHeader *image_params, ImageBounds *image_bounds)
{
    char grid_label[10];

    int crossw;
    int lon_startdeg, lat_startdeg, lat_startmin, lon_startmin;
    int x, y;
    int lat_deg, lat_minutes, lat_seconds, lon_deg, lon_minutes, lon_seconds;
    /*int pixel;*/
    int index;

    double lat_min, lon_min, lat_max, lon_max;    
    double lat_startsec, lon_startsec;
    double lat_start, lon_start;
    double lat, lon, lat_sec, lon_sec;
    double north, east;    
    double delta_grid;
    /*    Compute lat. and long. to start and end
     */

    delta_grid = (double)dgrid_sec / 3600.0;

    utmgeo (&lat_min, &lon_min, sonar->c_lon, sonar->west, sonar->south, 'n', sonar->ellipsoid);
    utmgeo (&lat_max, &lon_max, sonar->c_lon, sonar->east, sonar->north, 'n', sonar->ellipsoid);
    ddtodegminsec (lat_min, &lat_startdeg, &lat_startmin, &lat_startsec);
    ddtodegminsec (lon_min, &lon_startdeg, &lon_startmin, &lon_startsec);
    lat_startsec = (int)lat_startsec + 1.0;

    while ((int)lat_startsec % dgrid_sec)
        lat_startsec++;

    if (lat_startsec >= 60.0)
        {
        lat_startsec -= 60.0;
        lat_startmin++;

        if(lat_startmin > 60)
            {
            lat_startmin -= 60;

            if (lat_startdeg >= 0)
                lat_startdeg++;
            else 
                lat_startdeg--;
            }
        }

    lon_startsec = (int)lon_startsec + 1.0;
    while ((int)lon_startsec % dgrid_sec)
        lon_startsec++;

    if (lon_startsec >= 60.0)
        {
        lon_startsec -= 60.0;
        lon_startmin++;

        if(lon_startmin > 60)
            {
            lon_startmin -= 60;

            if (lon_startdeg >= 0)
                lon_startdeg++;
            else 
                lon_startdeg--;
            }
        }

    degmintodd (&lat_start, lat_startdeg,
                    (double)lat_startmin + (lat_startsec / 60));
    degmintodd (&lon_start, lon_startdeg,
                    (double)lon_startmin + (lon_startsec / 60));
    lat = lat_start;
    lon = lon_start;

    /*   
     *    Loop on lat and lon drawing crosses and labels.
     *    Each line of cross is 10 pixels.
     */

    crossw = 10;

    for (lat = lat_start; lat < lat_max; lat += delta_grid)
        {
        for (lon = lon_start; lon < lon_max; lon += delta_grid)
            {
            geoutm (lat, lon, sonar->c_lon, &east, &north, sonar->ellipsoid);
            if (in_box(east, north, image_bounds))
                {
                xy_raster (sonar, east, north, &x, &y, TDU);

                index = xy_to_i(x,y,image_params);
                if(index > 0 && 
                    index < image_params->width * image_params->height)
                    {

                        XDrawLine(XtDisplay(sonar->toplevel),
                                  sonar->orient_pixmap, sonar->main_gc, 
                                  x - crossw, y, x + crossw, y);
                        XDrawLine(XtDisplay(sonar->toplevel),
                                  sonar->orient_pixmap, sonar->main_gc,  
                                  x, y - crossw, x, y + crossw);

                        /* lat */
                        ddtodegminsec (lat, &lat_deg, &lat_minutes, &lat_sec);
                        lat_seconds = (int)(lat_sec + 0.5);
                        if (lat_seconds >= 60)
                            {
                            lat_seconds -= 60;
                            lat_minutes++;

                            if(lat_minutes > 60)
                                {
                                lat_minutes -= 60;

                                if (lat_startdeg >= 0)
                                    lat_deg++;
                                else 
                                    lat_deg--;
                                }
                            }

                        sprintf (grid_label, "%2d %.2d %.2d",
                                lat_deg, lat_minutes, lat_seconds);

                        XDrawString(XtDisplay(sonar->toplevel),
                                sonar->orient_pixmap,
                                sonar->main_gc, x + crossw + 5, y + 5,
                                grid_label, strlen(grid_label));

                        /* lon */

                        ddtodegminsec (lon, &lon_deg, &lon_minutes, &lon_sec);
                        lon_seconds = (int)(lon_sec + 0.5);

                        if (lon_seconds >= 60)
                            {
                            lon_seconds -= 60;
                            lon_minutes++;

                            if(lon_minutes > 60)
                                {
                                lon_minutes -= 60;

                                if (lon_startdeg >= 0)
                                    lon_deg++;
                                else 
                                lon_deg--;
                                }
                            }

                           sprintf (grid_label, "%2d %.2d %.2d",
                                    lon_deg, lon_minutes, lon_seconds);

                        XDrawString(XtDisplay(sonar->toplevel),
                            sonar->orient_pixmap,
                            sonar->main_gc, x - 3 * crossw, y - (crossw + 5),
                            grid_label, strlen(grid_label));


                    } /* end of image index check */

                } /* end of inbox check */

            } /* end of inner for loop */

        } /* end of outer for loop */

}

void GetUtmText(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    XmString calling_name;
    XmString compare_name;

    char *text;

    calling_name = XmStringCreateLtoR(XtName(w),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("UtmText0",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextFieldGetString(w);
        sonar->utm_west[1] = atof(text);
        XtFree((char *)text);
        }

    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("UtmText1",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextFieldGetString(w);
        sonar->utm_south[1] = atof(text);
        XtFree((char *)text);
        }

fprintf(stdout, "utm west = %f\n", sonar->utm_west[1]);
fprintf(stdout, "utm south = %f\n", sonar->utm_south[1]);

    XmStringFree(calling_name);

    return;
}

