/* show_nav
 *
 *   Plots on the terminal screen the location of a sonar data file.
 *   
 */

#include <unistd.h>
#include <math.h>
#include <assert.h>

#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>

#include "sonar_struct.h"

#include <map.h>
#include "xsonar.h"

void process_buttonPress(Widget w, MainSonar *sonar, XEvent *ev, int *anchor_x, int *anchor_y, int *lastx, int *lasty);
void process_buttonMotion(Widget w, MainSonar *sonar, XEvent *ev, int *currentx, int *currenty, int *lastx, int *lasty);
void process_buttonRelease(Widget w, MainSonar *sonar, XEvent *ev, int *lastx, int *lasty);
/*void raster_xy();*/
/*void xy_raster(MainSonar *, double, double, int *, int *, int);*/

void process(Widget w, XtPointer client_data, XtPointer call_data);
void close_app(Widget w, XtPointer client_data, XtPointer call_data);
void redraw(Widget w, XtPointer client_data, XtPointer call_data);
void window_input(Widget w, XtPointer client_data, XtPointer call_data);
void ascii_nav(Widget w, XtPointer client_data, XtPointer call_data);

int bounds(double a_min, double a_max, double *interval);

void showaswath(MainSonar *sonar, int *check_time);
void plot_utm_grid(MainSonar *sonar, double grid_int, int label_flag, ImageHeader *image_params);
void plot_mercator_grid(MainSonar *sonar, int dgrid_sec, int label_flag, ImageHeader *image_params);


#define VERYBIG 1e100
#define BIG 1e10
#define SMALL -1e10
#define BACKGROUND 255    
#define TRUE 1
#define FALSE 0

void show_nav(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    Cardinal n;
    Arg args[20];

    Widget navDialog;
    Widget mainNavForm;
    Widget fileMenuBar;
    Widget filePullDownMenu;
    Widget optionsPullDownMenu;
    Widget printerPullDownMenu;
    Widget typePullDownMenu;
    Widget fileButton;
    Widget printerButton;
    Widget typeButton;
    Widget optionsButton;
    Widget exitButton;
    Widget asciiNavButton;
    Widget showNav;
    Widget tduButton;
    Widget aldenButton;
    Widget rasterButton;
    Widget setBoundsButton;
    /*Widget hardReturnButton; */
    /* Widget hardReturnPullDownMenu; */
    Widget frame;
    Widget placeBox;
    Widget separator;
    /* Widget testShell; */
    Widget nadirTextButton;
    Widget nadirPullDownMenu;
    Widget nadirYesButton;
    Widget nadirNoButton;
    Widget tduLineSpacing;
    Widget laybackButton;
    Widget ellipsoidType;

    XFontStruct *label_font;

    /* Colormap default_colormap; */              /* colormap from server */

    /* Status result; */
    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;
    /* Atom ATOM_WM_COLMAP_WIN; */

    /* Window drawing_window; */

    /* unsigned short intensity; */


    char warningmessage[100];
    /* int c, i; */

    /*
     *    Set the calling widget's sensitivity to FALSE
     */

    XtSetSensitive(w, FALSE);

    /*
     *    Start creating widgets for this app
     */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     XtSetArg(args[n], XmNmaxWidth, 704); n++;
     XtSetArg(args[n], XmNmaxHeight, 740); n++;
     XtSetArg(args[n], XmNminWidth, 704); n++;
     XtSetArg(args[n], XmNminHeight, 740); n++;
     navDialog = XtCreatePopupShell("NavDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     mainNavForm = XtCreateWidget("MainNavForm", xmFormWidgetClass,
                       navDialog, args, n);
     XtManageChild(mainNavForm);

    /*
     *   Create a menu bar for the top of the form that will contain
     *   the push button widgets for showing nav, exiting, and mapping.
     */

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     fileMenuBar = XmCreateMenuBar(mainNavForm, "FileMenuBar", args, n);
     XtManageChild(fileMenuBar);

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'N'); n++;
     fileButton = XtCreateWidget("FileButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
     XtManageChild(fileButton);

     filePullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                        "FilePullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, filePullDownMenu); n++;
     XtSetValues(fileButton, args, n);

     showNav = XtCreateWidget("ShowNav", xmPushButtonWidgetClass,
                filePullDownMenu, NULL, 0);
     XtManageChild(showNav);

     asciiNavButton = XtCreateWidget("AsciiNavButton", xmPushButtonWidgetClass,
                                    filePullDownMenu, NULL, 0);
     XtManageChild(asciiNavButton);

     exitButton = XtCreateWidget("ExitButton", xmPushButtonWidgetClass,
                                    filePullDownMenu, NULL, 0);
     XtManageChild(exitButton);

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'O'); n++;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Output", XmSTRING_DEFAULT_CHARSET)); n++;
     printerButton = XtCreateWidget("PrinterButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
     XtManageChild(printerButton);

     printerPullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                        "PrinterPullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, printerPullDownMenu); n++;
     XtSetValues(printerButton, args, n);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Type", XmSTRING_DEFAULT_CHARSET)); n++;
     typeButton = XtCreateWidget("TypeButton",
                    xmCascadeButtonWidgetClass, printerPullDownMenu, args, n);
     XtManageChild(typeButton);

     typePullDownMenu = XmCreatePulldownMenu(printerPullDownMenu,
                                        "TypePullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, typePullDownMenu); n++;
     XtSetValues(typeButton, args, n);

     n = 0;
     XtSetArg(args[n], XmNlabelString, 
               XmStringCreateLtoR("TDU 850", XmSTRING_DEFAULT_CHARSET)); n++;
     tduButton = XtCreateWidget("TduButton", xmPushButtonWidgetClass,
                              typePullDownMenu, args, n);
     XtManageChild(tduButton);

     n = 0;
     XtSetArg(args[n], XmNlabelString, 
               XmStringCreateLtoR("Alden", XmSTRING_DEFAULT_CHARSET)); n++;
     aldenButton = XtCreateWidget("AldenButton", xmPushButtonWidgetClass,
                              typePullDownMenu, args, n);
     XtManageChild(aldenButton);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Raster", XmSTRING_DEFAULT_CHARSET)); n++;
     rasterButton = XtCreateWidget("RasterButton", xmPushButtonWidgetClass,
                              typePullDownMenu, args, n);
     XtManageChild(rasterButton);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Boundaries", XmSTRING_DEFAULT_CHARSET)); n++;
     setBoundsButton = XtCreateWidget("SetBoundsButton", 
               xmPushButtonWidgetClass, printerPullDownMenu, args, n);
     XtManageChild(setBoundsButton);

     /*
      *    Create a pull down menu for various options associated with
      *    the navigation.
      */

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'O'); n++;
     optionsButton = XtCreateWidget("OptionsButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
     XtManageChild(optionsButton);

     optionsPullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                        "OptionsPullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, optionsPullDownMenu); n++;
     XtSetValues(optionsButton, args, n);

     /*
      *    Create a button to allow the user to place a box of
      *    a specified scale around the nav track.  This outline is
      *    what will be mapped in the sonar map routine.
      */

     placeBox = XtCreateWidget("PlaceBox", xmPushButtonWidgetClass,
                              optionsPullDownMenu, NULL, 0);
     XtManageChild(placeBox);

     /*
      *    Create a button to allow the user to adjust the aspect
      *    ratio of the printed image using the Alden or TDU850.
      *    These printers plot 203 pixels / inch across the print head,
      *    but vary along the paper output axis.
      */

     tduLineSpacing = XtCreateWidget("TduLineSpacing", xmPushButtonWidgetClass,
                              optionsPullDownMenu, NULL, 0);
     XtManageChild(tduLineSpacing);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ellipsoids",
         XmSTRING_DEFAULT_CHARSET)); n++;
     ellipsoidType = XtCreateWidget("EllipsoidType", xmPushButtonWidgetClass,
                              optionsPullDownMenu, args, n);
     XtManageChild(ellipsoidType);

     /*
      *   Setup the button for turning on or off the ascii nadir text.
      */

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("ASCII Nadir",
               XmSTRING_DEFAULT_CHARSET)); n++;
     nadirTextButton = XtCreateWidget("NadirTextButton",
                    xmCascadeButtonWidgetClass, optionsPullDownMenu, args, n);
     XtManageChild(nadirTextButton);

     nadirPullDownMenu = XmCreatePulldownMenu(printerPullDownMenu,
                                        "NadirPullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, nadirPullDownMenu); n++;
     XtSetValues(nadirTextButton, args, n);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("On", XmSTRING_DEFAULT_CHARSET)); n++;
     nadirYesButton = XtCreateWidget("NadirYesButton",
               xmPushButtonWidgetClass, nadirPullDownMenu, args, n);
     XtManageChild(nadirYesButton);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Off", XmSTRING_DEFAULT_CHARSET)); n++;
     nadirNoButton = XtCreateWidget("NadirNoButton",
               xmPushButtonWidgetClass, nadirPullDownMenu, args, n);
     XtManageChild(nadirNoButton);

     /*
      *   Add a button to call up the layback routines.
      */

     n = 0;
     XtSetArg(args[n], XmNlabelString,
               XmStringCreateLtoR("Layback", XmSTRING_DEFAULT_CHARSET)); n++;
     laybackButton = XtCreateWidget("LaybackButton", xmPushButtonWidgetClass,
                              optionsPullDownMenu, args, n);
     XtManageChild(laybackButton);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, fileMenuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator = XtCreateWidget("separator",
                    xmSeparatorWidgetClass, mainNavForm, args, n);
     XtManageChild(separator);

    n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     frame = XtCreateWidget("WindowFrame", xmFrameWidgetClass,
                        mainNavForm, args, n);
     XtManageChild(frame);

    n = 0;
    XtSetArg(args[n], XmNheight, 700); n++;
    XtSetArg(args[n], XmNwidth, 700); n++;
     sonar->nav_window = XtCreateWidget("NavWindow", xmDrawingAreaWidgetClass,
                        frame, args, n);
     XtManageChild(sonar->nav_window);

    XtAddCallback(placeBox, XmNactivateCallback, 
                                       window_input, sonar);
    XtAddCallback(showNav, XmNactivateCallback, 
                                       process, sonar);
    XtAddCallback(exitButton, XmNactivateCallback, 
                                       close_app, sonar);
    XtAddCallback(sonar->nav_window, XmNexposeCallback, 
                                       redraw, sonar);
    XtAddCallback(asciiNavButton, XmNactivateCallback, 
                                       ascii_nav, sonar);
    XtAddCallback(tduButton, XmNactivateCallback, 
                                       setTduFlag, sonar);
    XtAddCallback(aldenButton, XmNactivateCallback, 
                                       setAldenFlag, sonar);
    XtAddCallback(rasterButton, XmNactivateCallback, 
                                       setRasterFlag, sonar);
    XtAddCallback(setBoundsButton, XmNactivateCallback, 
                                       rasterBounds, sonar);
    XtAddCallback(nadirYesButton, XmNactivateCallback, 
                                       asciiNadirText, sonar);
    XtAddCallback(nadirNoButton, XmNactivateCallback, 
                                       asciiNadirText, sonar);
    XtAddCallback(laybackButton, XmNactivateCallback, 
                                       setupFishNav, sonar);
    XtAddCallback(tduLineSpacing, XmNactivateCallback, 
                                       tduSetup, sonar);
    XtAddCallback(ellipsoidType, XmNactivateCallback, 
                                       ellipsoidSetup, sonar);

    /*
     *   Realize the nav dialog
     */

    XtRealizeWidget(navDialog);

    sonar->nav_gc = XCreateGC(XtDisplay(navDialog),
                                  XtWindow(navDialog), None, NULL);

    label_font = XLoadQueryFont(XtDisplay(sonar->nav_window),
            "-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*");
/*
            "-adobe-times-bold-r-normal--12-120-75-75-m-67-iso8859-1");
            "-adobe-courier-bold-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-courier-medium-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-helvetica-medium-r-normal--8-80-75-75-p-46-iso8859-1");
*/

    if(label_font == 0)
        {
        sprintf(warningmessage, "Could not find font, using default");
        messages(sonar, warningmessage);
        }
    else
        XSetFont(XtDisplay(sonar->nav_window), sonar->nav_gc, label_font->fid);

	/*
	 *    Set the window's colormap to that of the toplevel
	 *
	 */

    main_window_attr.colormap = sonar->colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(navDialog),
                    XtWindow(navDialog), main_window_valuemask,
                    &main_window_attr);

    /*
     *    Get screen dimensions.
     */

    sonar->scrheight =  sonar->scrwidth = 700;

    XSetBackground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             BlackPixelOfScreen(XtScreen(sonar->nav_window)));

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             BlackPixelOfScreen(XtScreen(sonar->nav_window)));

    sonar->nav_pixmap = XCreatePixmap(XtDisplay(sonar->nav_window),
                        XtWindow(sonar->nav_window), 700, 700, 
                        DefaultDepth(XtDisplay(sonar->nav_window),
                        DefaultScreen(XtDisplay(sonar->nav_window))));

    XFillRectangle(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, 0, 0, 700, 700);

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             WhitePixelOfScreen(XtScreen(sonar->nav_window)));


    XtPopup(navDialog, XtGrabNone);

    return;

}

void process(UNUSED Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{


    MainSonar *sonar = (MainSonar *) client_data;
    ImageHeader image_params;

    Cardinal n;
    Arg args[20];

    int /*i, j, k,*/ count;
    int check_time = 0;
    /*int xl, xr, yb, yt;*/
    int backcolor = BACKGROUND;    /* background color */
    int gridcolor;                /* color of grid lines */
    int mergridint;
    int infd;
    int status;

    long skipsize;
    long position;

    double utmgridint;
    double xpix_s, ypix_s;        /* pixel dimension in meters */
    /*double pix_size=0.;*/            /* screen pixel size in meters / pixel */

    double centx, centy;        /* center easting and northing */
    double maxx = 0.0;            /* maximum easting */
    double maxy = 0.0;            /* maximum northing */
    double minx = 20000000.0;    /* minimum easting */
    double miny = 20000000.0;    /* minimum northing */
    /*double sum_azi=0.;*/

    char warningmessage[255];

    /*float getcentlon();*/
    /*void messages();*/

    XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*NavDialog"));


/*
    if(sonar->rasterFlag && (sonar->rasterWest == 0 || sonar->rasterEast == 0))
        {
        sprintf(warningmessage, 
             "Please type in the map area boundaries\n(Boundaries button -- Option pull-down");
        messages(sonar, warningmessage);
        return;
        }
*/

    infd = open (sonar->infile, O_RDONLY);  /* read only */
    if (infd == -1)
        {
        sprintf(warningmessage, 
             "Cannot open input file\nPlease select a data file in Setup");
        messages(sonar, warningmessage);
        return;
        }

    if (read (infd, &sonar->ping_header, 256) != 256)
        {
        sprintf(warningmessage, "Cannot read data file header\n");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }

    if(sonar->ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }


    if(sonar->ping_header.utm_e > 0.0 && sonar->ping_header.utm_e < 500000.0)
        sonar->ping_header.merged = 1;

    if(!sonar->ping_header.merged)
        {
        sprintf(warningmessage, "Navigation has not been merged.\n");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }

    position = lseek (infd, 0L, SEEK_SET);

    if(!sonar->swath)
        sonar->swath = sonar->ping_header.swath_width;

    XClearWindow(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window));
    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             BlackPixelOfScreen(XtScreen(sonar->nav_window)));

    XFillRectangle(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, 0, 0, 700, 700);

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             WhitePixelOfScreen(XtScreen(sonar->nav_window)));

   
/*
*    Read data and get min and max northing and easting
*/
    count = 0;

    position = lseek(infd, 0L, SEEK_SET);
    skipsize = sonar->ping_header.sdatasize - 256;

    if(sonar->tduFlag || sonar->aldenFlag)
        {
        while (read(infd, &sonar->ping_header, 256) == 256)
            {

            if(!sonar->c_lon && position > 40000)
                {
                
                sonar->c_lon = (double) sonar->ping_header.clon;

                n = 0;
                if(XtNameToWidget(sonar->toplevel, "*SetupDialog") != NULL)
                   {
                   XtSetArg(args[n], XmNvalue, sonar->c_lon); n++;
                   XtSetValues(XtNameToWidget(sonar->toplevel,
                                               "*CentralLonScale"), args, n);
                   }
                }


            if ((sonar->ping_header.utm_e > SMALL)
             && (sonar->ping_header.utm_n > SMALL)
             && (sonar->ping_header.utm_e < BIG)
             && (sonar->ping_header.utm_n < BIG))
                {
                if (sonar->ping_header.utm_e > maxx)
                    maxx = sonar->ping_header.utm_e;
                if (sonar->ping_header.utm_e < minx)
                    minx = sonar->ping_header.utm_e;
                if (sonar->ping_header.utm_n > maxy)
                    maxy = sonar->ping_header.utm_n;
                if (sonar->ping_header.utm_n < miny)
                    miny = sonar->ping_header.utm_n;
                }

            position = lseek(infd, skipsize, SEEK_CUR);
            }
        }
    else
        {
        maxx = sonar->rasterEast;
        minx = sonar->rasterWest;
        maxy = sonar->rasterNorth;
        miny = sonar->rasterSouth;
        }

    position = lseek(infd, 0L, SEEK_SET);


/*
*    Compute screen boundaries for data just read, draw a box of the background
*    color, and select a color for the grid lines
*/

    xpix_s = (int)((maxx - minx + sonar->swath) / sonar->scrwidth) + 1.0;
    ypix_s = (int)((maxy - miny + sonar->swath) / sonar->scrheight) + 1.0;
    sonar->pix_s = MAX (xpix_s, ypix_s);
    centx = (maxx + minx) / 2.0;
    centy = (maxy + miny) / 2.0;

    sonar->west = (int)(centx - (sonar->scrwidth * sonar->pix_s / 2.0));
    sonar->south = (int)(centy - (sonar->scrheight * sonar->pix_s / 2.0));
    sonar->east = sonar->scrwidth * sonar->pix_s + sonar->west;
    sonar->north = sonar->scrheight * sonar->pix_s + sonar->south;

    if(sonar->tduFlag || sonar->aldenFlag)
        {
        sonar->rasterNorth = sonar->north;
        sonar->rasterSouth = sonar->south;
        sonar->rasterWest = sonar->west;
        sonar->rasterEast = sonar->east;
        sonar->rasterPixSize = sonar->pix_s;
        }

/*
fprintf(stdout, "north = %f\tsouth = %f\n", sonar->north, sonar->south);
fprintf(stdout, "east = %f\twest = %f\n", sonar->east, sonar->west);
*/

    if (backcolor > 128)
        gridcolor = 0;
    else
        gridcolor = 255;

    /*    
     *    Compute grid intervals
     */

    if ((sonar->north - sonar->south) < (sonar->east - sonar->west))
        status = bounds(sonar->west, sonar->east, &utmgridint);
    else
        status = bounds(sonar->south, sonar->north, &utmgridint);

    if(!status)
        {
        sprintf(warningmessage, "Zero range in bounds().\n");
        strcat(warningmessage, "Check ASCII nav file and remerge\n");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }

    mergridint = (int) ((utmgridint * 2 / 1852.0) * 60);

    while(mergridint % 15)
        mergridint++;

    plot_utm_grid (sonar, utmgridint, 1, &image_params);
    plot_mercator_grid (sonar, mergridint, 1, &image_params);
    while (read(infd, &sonar->ping_header, 256) == 256)
        {
        showaswath(sonar, &check_time);
        position = lseek(infd, skipsize, SEEK_CUR);
        }

    close(infd);

    return;
}

/* ++++++++++++++   beginning of functions and subroutines   ++++++++++++++++ */

void redraw(UNUSED Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    
    XCopyArea(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
            XtWindow(sonar->nav_window), sonar->nav_gc,
            0, 0, 700, 700, 0, 0);

    return;
}

void ascii_nav(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget parent_widget;
    Widget asciiNavButton;

    int i, j;
    int x1, y1, x0, y0;
    int julianDay[5000], hour[5000], minute[5000], second[5000];
    int *jday_ptr, *hour_ptr, *minute_ptr, *second_ptr;
    int number_of_nodes;
    int grid_ticks;
    int doNavText;
    int latitude_degrees, longitude_degrees;
    int badFix[5000];
    int answer;
    int buffer_len;

    double latitude_minutes, longitude_minutes;
    double latitude[5000], longitude[5000];
    double latitude_max, longitude_max;
    double latitude_min, longitude_min;
    double latitude_range, longitude_range;
    double latitude_tick_interval, longitude_tick_interval;
    double range_difference;
    double current_decimal_time, next_decimal_time;

    char *navString[5000];
    char *navPtr;
    /*char *tempPtr;*/
    char time_string[20];
    char lon_label[20];
    char lat_label[20];
    char warningmessage[255];

    char *stringPtr;
    char *buffer;
    char *tempString;

    long position;

    unsigned int numberOfChars;

    /*void messages();*/
    /*int createNavText(); */

    buffer = (char *) XtCalloc(255, sizeof(char));

    doNavText = 0;
    answer = 0;

    strcpy(buffer, sonar->infile);

    if(sonar->infile != NULL && (stringPtr = strrchr(buffer, '.')) != NULL)
        {

        /*
         *    Added by Steve Dzurenko (URI Ocean Mapping Group) April 1995.
         *    Searches the file name buffer backwards looking for the first
         *    "." just in case some component of the filename directory
         *    hierarchy has a period in it.
         */

        buffer_len = (int) strlen(buffer);
        while(*(buffer + --buffer_len) != '.')      /* just find the "." */
            strncpy((buffer + buffer_len),"\0",1);

        strncat(buffer, "nav", 3);

        if((sonar->navfp = fopen(buffer, "r")) == NULL)
            {
            sprintf(warningmessage, 
                     "Could not open ascii navigation file: %s\n", buffer);
            strcat(warningmessage, "Check to ensure that nav file exists");

            messages(sonar, warningmessage);
            parent_widget = w;
            while(!XtIsTransientShell(parent_widget))
                parent_widget = XtParent(parent_widget);

            asciiNavButton = XtNameToWidget(parent_widget, "*AsciiNavButton");

            XtSetSensitive(asciiNavButton, FALSE);
            XtFree(buffer);
            return;
            }
        }
    else
        {
        sprintf(warningmessage, 
                 "No sonar file selected.  Please \nselect a file in Setup.");
        messages(sonar, warningmessage);
        XtFree(buffer);
        return;
        }

    /*
     *    Set the calling button sensitivity to FALSE
     */

    XtSetSensitive(w, FALSE);


    jday_ptr = julianDay;
    hour_ptr = hour;
    minute_ptr = minute;
    second_ptr = second;

    XClearWindow(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window));

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             BlackPixelOfScreen(XtScreen(sonar->nav_window)));


    XFillRectangle(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, 0, 0, 700, 700);

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             WhitePixelOfScreen(XtScreen(sonar->nav_window)));

    number_of_nodes = 0;

    position = fseek(sonar->navfp, 0L, SEEK_SET);

    /*
     *    Read in the data from the ASCII file.
     *
     *    Set up the nav string pointer to handle each line of ASCII
     *    nav in order to display duplicate fixes if any are flagged.
     */

    while (fscanf(sonar->navfp,"%d %d %d %d %d %lf %d %lf", jday_ptr++,
        hour_ptr++, minute_ptr++, second_ptr++, &latitude_degrees,
        &latitude_minutes, &longitude_degrees, &longitude_minutes) != EOF)
        {

        longitude[number_of_nodes] = (double) (longitude_degrees)
                                - (longitude_minutes / 60.0);
        latitude[number_of_nodes] = (double) (latitude_degrees)
                                + (latitude_minutes / 60.0);

        navString[number_of_nodes] = (char *) XtCalloc(80, sizeof(char));

        sprintf(navString[number_of_nodes], 
		/* FIX: was the \0 needed for padding? */
		/*                 "%03d %02d %02d %02d %d %.5lf %d %.5lf\0", */
                 "%03d %02d %02d %02d %d %.5lf %d %.5lf", 
                 julianDay[number_of_nodes], hour[number_of_nodes], 
                 minute[number_of_nodes], second[number_of_nodes], 
                 latitude_degrees, latitude_minutes, longitude_degrees, 
                 longitude_minutes);

        badFix[number_of_nodes] = 0;

/*
fprintf(stderr, "read node %d\n", number_of_nodes);
fprintf(stderr, "longitude[%d] = %f\n", number_of_nodes, longitude[number_of_nodes]);
fprintf(stderr, "latitude[%d] = %f\n", number_of_nodes, latitude[number_of_nodes]);
*/

        number_of_nodes++;


        }


    longitude_max = -180.0;
    longitude_min = 180.0;

    latitude_min = 90.0;
    latitude_max = 0.0;
    

    for(i = 0; i < number_of_nodes; i++)
        {

        if(longitude[i] != 0)
            {
            if(longitude[i] > longitude_max)
                longitude_max = longitude[i];
            if(longitude[i] < longitude_min)
                longitude_min = longitude[i];
            }
#if 0
        else
            ;  /* ignore  the point */
#endif

        if(latitude[i] != 0)
            {
            if(latitude[i] > latitude_max)
                latitude_max = latitude[i];
            if(latitude[i] < latitude_min)
                latitude_min = latitude[i];
            }
#if 0
        else
            ;  /* ignore  the point */
#endif

        }

    latitude_range = fabs(latitude_max - latitude_min);
    longitude_range = fabs(longitude_max - longitude_min);

    latitude_tick_interval = (latitude_range * (double) 20 / (double) 600);
    longitude_tick_interval = (longitude_range * (double) 20 / (double) 600);

    range_difference = fabs(latitude_range - longitude_range);

    if(latitude_range < longitude_range)
        {
        latitude_range = longitude_range;
        latitude_min -= range_difference / 2;
        }

    if(longitude_range < latitude_range)
        {
        longitude_range = latitude_range;
        longitude_min -= range_difference / 2;
        }

    latitude_tick_interval = (latitude_range * (double) 20 / (double) 600);
    longitude_tick_interval = (longitude_range * (double) 20 / (double) 600);

    grid_ticks = 20;

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 

    XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

    XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                            sonar->nav_gc, 50, 650, 650, 650);
    XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                            sonar->nav_gc, 50, 50, 50, 650);
    XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                            sonar->nav_gc, 50, 650, 650, 650);
    XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                            sonar->nav_gc, 50, 50, 50, 650);

    for(i = 0, j = 0; i < 620; i+=grid_ticks, j++)
        {
        if(!(i % 2))
            {
            XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 

            XDrawLine(XtDisplay(sonar->nav_window),
                XtWindow(sonar->nav_window), sonar->nav_gc,
                (int)i + 50, 650, (int)i + 50, 655);
            XDrawLine(XtDisplay(sonar->nav_window),
                XtWindow(sonar->nav_window), sonar->nav_gc,
                50, 650 - i, 45, 650 - i);

            XDrawLine(XtDisplay(sonar->nav_window),
                sonar->nav_pixmap, sonar->nav_gc,
                (int)i + 50, 650, (int)i + 50, 655);
            XDrawLine(XtDisplay(sonar->nav_window),
                sonar->nav_pixmap, sonar->nav_gc,
                50, 650 - i, 45, 650 - i);

            if(!(i % 6))
                {
                XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                          WhitePixelOfScreen(XtScreen(sonar->nav_window)));

                XDrawLine(XtDisplay(sonar->nav_window),
                    XtWindow(sonar->nav_window), sonar->nav_gc,
                    (int)i + 50, 650, (int)i + 50, 655);

                XDrawLine(XtDisplay(sonar->nav_window),
                    XtWindow(sonar->nav_window), sonar->nav_gc,
                    50, 650 - i, 45, 650 - i);

                XDrawLine(XtDisplay(sonar->nav_window),
                    sonar->nav_pixmap, sonar->nav_gc,
                    (int)i + 50, 650, (int)i + 50, 655);

                XDrawLine(XtDisplay(sonar->nav_window),
                    sonar->nav_pixmap, sonar->nav_gc,
                    50, 650 - i, 45, 650 - i);

                sprintf(lon_label, "%.3lf",
                    longitude_min + (longitude_tick_interval * j));

                XDrawString(XtDisplay(sonar->nav_window),
                     XtWindow(sonar->nav_window), sonar->nav_gc, 25 + i, 670,
                     lon_label, strlen(lon_label));

                XDrawString(XtDisplay(sonar->nav_window),
                     sonar->nav_pixmap, sonar->nav_gc, 25 + i, 670,
                     lon_label, strlen(lon_label));

                sprintf(lat_label, "%.3lf",
                    latitude_min + (latitude_tick_interval * j));

                XDrawString(XtDisplay(sonar->nav_window),
                     XtWindow(sonar->nav_window), sonar->nav_gc, 5, 655 - i,
                     lat_label, strlen(lat_label));

                XDrawString(XtDisplay(sonar->nav_window),
                     sonar->nav_pixmap, sonar->nav_gc, 5, 655 - i,
                     lat_label, strlen(lat_label));

                }
            }
        }
        
    /*
     *    Set up the nav string pointer to handle each line of ASCII
     *    nav in order to display duplicate fixes if any are flagged.
     */

    for(i = 0; i < number_of_nodes - 1; i++)
        {

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 

        x1 = 50 + (int) ((fabs(longitude[i + 1] - longitude_min)
                                    / longitude_range) * 600.0);
        y1 = (int) (650.0 - ((latitude[i + 1] - latitude_min)
                                    / latitude_range) * 600.0);

        x0 = 50 + (int) ((fabs(longitude[i] - longitude_min)
                                    / longitude_range) * 600.0);
        y0 = (int) (650.0 - ((latitude[i] - latitude_min)
                                    / latitude_range) * 600.0);

        XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

        if(longitude[i] != 0 && latitude[i] != 0)
            {
            XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                sonar->nav_gc, x0, y0, x1, y1);

            XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                sonar->nav_gc, x0, y0, x1, y1);
            }

        next_decimal_time = (double) julianDay[i + 1] + 
            (double) hour[i + 1] / 24.0 +
            (double) minute[i + 1] / 1440.0 + 
            (double) second[i + 1] / 86400;
        current_decimal_time = (double) julianDay[i] +
            (double) hour[i] / 24.0 +
            (double) minute[i] / 1440.0 + 
            (double) second[i] / 86400;

        if(next_decimal_time - current_decimal_time == 0
            || longitude[i + 1] == longitude[i] 
            || latitude[i + 1] == latitude[i])
            {
            sprintf(time_string, "Duplicate fix @ %02d:%02d", 
                                                     hour[i], minute[i]);

            XDrawString(XtDisplay(sonar->nav_window),
                 XtWindow(sonar->nav_window), sonar->nav_gc, x0 + 3, y0 + 3,
                 time_string, strlen(time_string));

            XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                 sonar->nav_gc, x0 + 3, y0 + 3,
                 time_string, strlen(time_string));


            if(i == number_of_nodes - 2)
                {
                if(!badFix[i])
                    {
                    badFix[i]++;
                    strcat(navString[i], " ***** "); 
                    }
                }
            else
                {
                badFix[i + 1]++;
                strcat(navString[i + 1], " ***** "); 
                }


            doNavText++;
            }
        else
            {
            XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, 
                                                 sonar->yellowPixel.pixel);

            XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 3, 
                                        LineSolid, CapButt, JoinRound);

            if(longitude[i] != 0 && latitude[i] != 0)
                {
                XDrawRectangle(XtDisplay(sonar->nav_window), 
                    XtWindow(sonar->nav_window), sonar->nav_gc, 
                    x0-1, y0-1, 3, 3);

                XDrawRectangle(XtDisplay(sonar->nav_window), 
                    sonar->nav_pixmap,
                    sonar->nav_gc, x0-1, y0-1, 3, 3);
                }

            }

        if(latitude[i] == 0 || longitude[i] == 0)
            {
            sprintf(time_string, "Bad fix @ %02d:%02d", hour[i], minute[i]);

            XDrawString(XtDisplay(sonar->nav_window),
                 XtWindow(sonar->nav_window), sonar->nav_gc, 100, 600,
                 time_string, strlen(time_string));

            XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                 sonar->nav_gc, 100, 600,
                 time_string, strlen(time_string));

            doNavText++;
            }

        if(next_decimal_time - current_decimal_time > 0.25 || !(i % 6))
            {
            sprintf(time_string, "%02d:%02d", hour[i], minute[i]);

            XDrawString(XtDisplay(sonar->nav_window),
                 XtWindow(sonar->nav_window), sonar->nav_gc, x0 + 5, y0 + 5,
                 time_string, strlen(time_string));

            XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                 sonar->nav_gc, x0 + 5, y0 + 5,
                 time_string, strlen(time_string));

            }



        }
    
    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
             WhitePixelOfScreen(XtScreen(sonar->nav_window)));

    fclose(sonar->navfp);

    numberOfChars = 80 * number_of_nodes;
    navPtr = (char *) XtCalloc(numberOfChars, sizeof(char));

    for(i = 0; i < number_of_nodes; i++)
        {
        strncat(navPtr, navString[i], strlen(navString[i]));
        strcat(navPtr, "\n");
        }

    if(doNavText)
        {

        /*
         *    Set the file button sensitivity to FALSE
         *    so that the nav dialog cannot be closed out if
         *    editing the nav.
         */

        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                   "*NavDialog*ExitButton"), FALSE);

        answer = createNavText(w, navPtr, sonar, number_of_nodes); 

        j = 0;

        for(i = 0; i < number_of_nodes; i++)
            {
            tempString = (char *) XtCalloc(80, sizeof(char));
            while(strrchr(tempString, '\n') == NULL)
                {
                strncat(tempString, navPtr, 1);
                navPtr++;
                j++;
                }
            strncpy(navString[i], tempString, strlen(tempString));
            XtFree(tempString);
            }

        navPtr -= j;

        if(answer == 1)
            {
            if((sonar->navfp = fopen(buffer, "w+")) == NULL)
                {
                sprintf(warningmessage, 
                     "Could not open ascii navigation file: %s\n", buffer);
                strcat(warningmessage, "Check permissions on the directory");

                messages(sonar, warningmessage);
                }
            else
                {
                for(i = 0; i < number_of_nodes; i++)
                    if(!badFix[i])
                        fwrite(navString[i], strlen(navString[i]), 1, sonar->navfp);
                }
            fclose(sonar->navfp);
            }

        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                   "*NavDialog*ExitButton"), TRUE);


        }
           


    for(i = 0; i < number_of_nodes; i++)
        XtFree(navString[i]);

    XtFree(navPtr);
    XtFree(buffer);

    /*
     *    Set the file button sensitivity to FALSE
     */

    XtSetSensitive(w, True);



    return;
}

void window_input(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Arg args[20];
    Cardinal n;

    XGCValues rubber_GC_values;
    GC the_rubber_GC;

    Status status;


    char warningmessage[100];

    /*void messages();*/

    /*int x, y, z;*/
    int correcting;
    int firstx, lastx, firsty, lasty, currentx, currenty;
    int endx, endy;
    int graphics_box_width, graphics_box_height;
    int box_count;
    int fore;

    double /*box_west,*/ box_east, box_north, box_south;
    double box_width, box_height;

    /* FIX: imageHeight & imageWidth can be used uninitialized */
    double pix_size, imageHeight, imageWidth;

    XEvent next_event;

    /*
     *    Make sure an output format has been selected before proceeding.
     */

    if(!sonar->tduFlag && !sonar->aldenFlag)
        {
        sprintf(warningmessage,
             "Please select an output\noption from the Output pull-down");
        messages(sonar, warningmessage);
        return;
        }

    if(sonar->scale == 0 || sonar->scale == (int) NULL)
        {
        sprintf(warningmessage,
                "Select a scale in setup when outlining for Sonar Map\n");
        messages(sonar, warningmessage);
        return;
        }

    XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*NavDialog"));

    XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                          WhitePixelOfScreen(XtScreen(sonar->nav_window)));
    XSetBackground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                          BlackPixelOfScreen(XtScreen(sonar->nav_window)));

    XSelectInput(XtDisplay(w),XtWindow(sonar->nav_window),
                ExposureMask|ButtonPressMask|
                ButtonReleaseMask|Button1MotionMask|
                Button2MotionMask|Button3MotionMask);

    status = XGrabPointer(XtDisplay(w), XtWindow(sonar->nav_window), FALSE,
        ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
        GrabModeAsync, GrabModeAsync, XtWindow(sonar->nav_window),
        XCreateFontCursor(XtDisplay(sonar->nav_window), XC_diamond_cross),
        CurrentTime);

    n = 0;
    XtSetArg(args[n], XmNforeground, &rubber_GC_values.foreground); n++;
    XtSetArg(args[n], XmNbackground, &rubber_GC_values.background); n++;
    XtGetValues(w, args, n);
    fore = rubber_GC_values.foreground;

    if (! fore)
        rubber_GC_values.foreground = rubber_GC_values.background;

    rubber_GC_values.function = GXxor;

    the_rubber_GC = XCreateGC(XtDisplay(w),XtWindow(sonar->nav_window),
              GCFunction |GCForeground| GCBackground, &rubber_GC_values);

    XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

    XSetLineAttributes(XtDisplay(w),the_rubber_GC, 2,
                    LineSolid, CapButt, JoinRound);

    correcting = TRUE;

    /*
     *   Compute pixel size in meters per pixel for Raytheon printer.
     */

    pix_size = ((double)sonar->scale / 203.0) * 0.0254;

    if(sonar->tduFlag)
        imageHeight = imageWidth = 1728;
    if(sonar->aldenFlag)
        imageHeight = imageWidth  = 2048;

    box_height = imageHeight * pix_size;
    box_width = imageWidth * pix_size;

    graphics_box_width = (int) (box_width / sonar->pix_s);
    graphics_box_height = (int) (box_height / sonar->pix_s);

    box_count = sonar->number_of_images = 0;

    while(correcting)
        {
        XNextEvent(XtDisplay(sonar->nav_window),&next_event);

        switch(next_event.type)
            {
            case ButtonPress:

                process_buttonPress(w, sonar, &next_event,
                        &firstx, &firsty, &lastx, &lasty);

                if(next_event.xbutton.button == Button1)
                    {
                    raster_xy (sonar, firstx, firsty, &box_east,
                                &box_north);

                    box_south = box_north - box_width;
                    
                    sonar->utm_west[box_count] = box_east;
                    sonar->utm_south[box_count] = box_south;

                    XDrawRectangle(XtDisplay(sonar->nav_window),
                            XtWindow(sonar->nav_window), the_rubber_GC,
                            firstx, firsty,    
                            graphics_box_width, graphics_box_height);

                    }

                break;

            case ButtonRelease:
	      /* FIX: bad call! extra correcting variable removed*/
                process_buttonRelease(w, sonar, &next_event,
                                     &endx, &endy/*, &correcting*/);

                if(next_event.xbutton.button == Button1)
                    {
                    raster_xy (sonar, endx, endy, &box_east,
                                &box_north);


                    box_south = box_north - box_width;
                    
                    sonar->utm_west[box_count] = box_east;
                    sonar->utm_south[box_count] = box_south;

                    sonar->rasterNorth = box_south + box_height;
                    sonar->rasterSouth = box_south;
                    sonar->rasterWest = box_east;
                    sonar->rasterEast = box_east + box_width;
                    sonar->rasterPixSize = pix_size;

                    box_count++;

                    sonar->number_of_images = box_count;

                    XDrawRectangle(XtDisplay(sonar->nav_window),
                            XtWindow(sonar->nav_window), sonar->nav_gc,
                            endx, endy,
                            graphics_box_width, graphics_box_height);

                    }

                if(next_event.xbutton.button == Button3)
                    {
                    XUngrabPointer(XtDisplay(w), CurrentTime);
                    correcting = FALSE;

                    }
                        
                break;
            case MotionNotify:

                process_buttonMotion(w, sonar, &next_event,
                        &currentx, &currenty, &lastx, &lasty);

                if(next_event.xmotion.state == Button1MotionMask)
                    {

                    XDrawRectangle(XtDisplay(sonar->nav_window),
                            XtWindow(sonar->nav_window), the_rubber_GC,
                            lastx, lasty,    
                            graphics_box_width, graphics_box_height);

                    XDrawRectangle(XtDisplay(sonar->nav_window),
                            XtWindow(sonar->nav_window), the_rubber_GC,
                            currentx, currenty,    
                            graphics_box_width, graphics_box_height);

                    lastx = currentx;
                    lasty = currenty;

                    }
                break;
            case KeyPress:
                break;
            case KeyRelease:
                break;
            default:
                break;

            }

        }


    XFreeGC(XtDisplay(w), the_rubber_GC);



    return;
}
    
void process_buttonPress(UNUSED Widget w,
			 UNUSED MainSonar *sonar,
			 XEvent *ev, int *anchor_x, int *anchor_y, int *lastx, int *lasty)
{

    switch(ev->xbutton.button)
        {
        case Button1:
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

void process_buttonMotion(UNUSED Widget w,
			  UNUSED MainSonar *sonar,
			  XEvent *ev, int *currentx, int *currenty, int *lastx, int *lasty)
{

    switch(ev->xmotion.state)
        {
        case Button1MotionMask:
            if(*lasty != ev->xmotion.y || *lastx != ev->xmotion.x)
                {
                *currentx = ev->xmotion.x;
                *currenty = ev->xmotion.y;
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

void process_buttonRelease(UNUSED Widget w,
			   UNUSED MainSonar *sonar,
			   XEvent *ev, int *lastx, int *lasty)
{

    switch(ev->xbutton.button)
        {
        case Button1:

    
            *lastx = ev->xbutton.x;
            *lasty = ev->xbutton.y;

            break;
        case Button2:
            break;
        case Button3:
            break;
        default:
            break;
        }
    
    return;

}

/* Subroutine that shows on the screen the location of one swath of data */

void showaswath (MainSonar *sonar, int *check_time)
{

    char temp[5],hourstr[5];

    int hour, min;
    int hourmin;
    int centx, centy, x, y;
    int sx, sy, px, py;

    double starpixe, starpixn, portpixe, portpixn;
    double fardist;
    double de, dn;

    double sec/*, lastsec*/;
    double theta;
    /*double sin(), cos();*/
    double degrees;

    fardist = sonar->swath/2.0;    /* distance vehicle-end of swath */
    theta = sonar->ping_header.utm_azi + M_PI/2.0;        /* this ping */
    de = (double)sin(theta);
    dn = (double)cos(theta);

    /* location of starboardmost pixel */
    starpixn = (sonar->ping_header.utm_n) + fardist*dn;
    starpixe = (sonar->ping_header.utm_e) + fardist*de;

    /* location of portmost pixel */
    portpixn = (sonar->ping_header.utm_n) - fardist*dn;
    portpixe = (sonar->ping_header.utm_e) - fardist*de;


    djdtohm (sonar->ping_header.djday, &hour, &min, &sec);

    XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

    if ((min % 2) != 0 || *check_time)
        {

        if((min % 2) != 0)
            *check_time = 0;

        /* draws a starboard line in green */

        XSetForeground(XtDisplay(sonar->nav_window), 
						sonar->nav_gc, sonar->greenPixel.pixel); 
        xy_raster (sonar, starpixe, starpixn, &x, &y, SCREEN);
        xy_raster (sonar, sonar->ping_header.utm_e,
                             sonar->ping_header.utm_n, &centx, &centy, SCREEN);

        XDrawPoint(XtDisplay(sonar->nav_window), 
                    XtWindow(sonar->nav_window), sonar->nav_gc, x, y);  
        XDrawPoint(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, x, y);  

        /* draws a port line in red */

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 
        xy_raster (sonar, portpixe, portpixn, &x, &y, SCREEN);

        XDrawPoint(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                        sonar->nav_gc, x, y);  
        XDrawPoint(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                        sonar->nav_gc, x, y);  

        /* draws a center pixel */

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
            WhitePixelOfScreen(XtScreen(sonar->nav_window)));
        xy_raster (sonar, sonar->ping_header.utm_e, 
                             sonar->ping_header.utm_n, &x, &y, SCREEN);
        XDrawPoint(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                        sonar->nav_gc, x, y);  
        XDrawPoint(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                        sonar->nav_gc, x, y);  

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
            WhitePixelOfScreen(XtScreen(sonar->nav_window)));
        }
    else     /* put a 15 min mark  */
        {
        *check_time = 1;
        hourmin = hour  * 100 + min;
        itoa (hourmin, temp);
        switch ( strlen(temp) )
            {
            case 4:
                strcpy(hourstr,temp);
                break;
            case 3:
                strcpy(hourstr,"0");
                strcat(hourstr,temp);
                break;
            case 2:
                strcpy(hourstr,"00");
                strcat(hourstr,temp);
                break;
            case 1:
                strcpy(hourstr,"000");
                strcat(hourstr,temp);
                break;
            default:
                strcpy(hourstr,"????");
                break;
            }


        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, 
						sonar->yellowPixel.pixel);

        xy_raster (sonar, portpixe, portpixn, &px, &py, SCREEN);
        xy_raster (sonar, starpixe, starpixn, &sx, &sy, SCREEN);

        degrees = sonar->ping_header.utm_azi * 180 / M_PI;
        if(degrees > 90 && degrees < 270)
            {
            XDrawImageString(XtDisplay(sonar->nav_window), 
                XtWindow(sonar->nav_window), sonar->nav_gc, px, py, 
                hourstr, strlen(hourstr));
            XDrawImageString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                sonar->nav_gc, px, py, hourstr, strlen(hourstr));
            }
        else
            {
            XDrawImageString(XtDisplay(sonar->nav_window), 
                XtWindow(sonar->nav_window), sonar->nav_gc, sx, sy, 
                hourstr, strlen(hourstr));
            XDrawImageString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                sonar->nav_gc, sx, sy, hourstr, strlen(hourstr));
            }

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
            WhitePixelOfScreen(XtScreen(sonar->nav_window)));

        XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                            sonar->nav_gc, px, py, sx, sy);
        XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                            sonar->nav_gc, px, py, sx, sy);

        }


    return;
}

/*
*    bounds
*
*    computes the grid interval given the boundaries min and max
*/
int bounds(double a_min, double a_max, double *interval)
{
    double range, log_range, rough_da;
    double min_dda, dda, order_mag, grids[4];
    int i, max_grids=3, ngrids=4;


/*    
*    Initialize grids array
*/
    grids[0]=1.0;
    grids[1]=2.0;
    grids[2]=5.0;
    grids[3]=10.0;
/*
*    Compute grid interval using the boundaries
fprintf(stderr, "max = %.1f\tmin = %.1f\n", a_max, a_min);
*/

    range= a_max - a_min;
    if (range<=0.0)
        {
        fprintf (stderr, "zero or negative range in bounds()\n");
        return(0);
        }

    log_range=(int)log10(range);
    rough_da=range/max_grids;
    order_mag=pow(10.0,log_range-1.0);
    min_dda=VERYBIG;
    for (i=0; i<ngrids; i++) {
        dda=fabs(rough_da-grids[i]*order_mag);
        if (dda<min_dda) {
            min_dda=dda;
            *interval=(double)(grids[i]*order_mag);
        }
    }
    return(1);
}

/*
**  next_box()   returns TRUE or FALSE depending if another qmdisp box is 
**               needed.
int next_box(azi_case,startx,starty,finalx,finaly)
   int azi_case,startx,starty,finalx,finaly;
   {
      int ret=1;
      switch (azi_case)
      {
         case  1:
            if ( (startx>finalx) && (starty>finaly) )
               ret = 0;
            break;
         case  2:
            if ( (startx>finalx) && (starty<finaly) )
               ret = 0;
            break;
         case  3:
            if ( (startx<finalx) && (starty<finaly) )
               ret = 0;
            break;
         case  4:
            if ( (startx<finalx) && (starty>finaly) )
               ret = 0;
            break;
         default:
            printf("There is a problem with azimuth (case = %d).\n",azi_case);
            exit(1);
      }
      return(ret);
   }
*/

void close_app(UNUSED Widget w,
	       XtPointer client_data,
	       UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget parentWidget;

    parentWidget = XtNameToWidget(sonar->toplevel, "*NavDialog");

    if(sonar->nav_gc != NULL)
        XFreeGC(XtDisplay(sonar->nav_window), sonar->nav_gc);
    if(sonar->nav_pixmap != (int) NULL)
        XFreePixmap(XtDisplay(sonar->nav_window), sonar->nav_pixmap);

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*NavButton"), TRUE);

    XtPopdown(parentWidget);
    XtDestroyWidget(parentWidget);

    return;

}

/*****************************************************************************
 *
 *    plot_utm_grid()
 *
 *    Plots and labels (if label_flag != 0) a UTM grid on the screen
 */
void plot_utm_grid(MainSonar *sonar, double grid_int,
		   UNUSED int label_flag,
		   UNUSED ImageHeader *image_params)
{
    char grid_label [20];
    int lab_off = 8;    /* offset of the grid labels */
    int e, w, s, n;
    int rastx, rasty;
    double x, y;
    /*void xy_raster(MainSonar *, double, double, int *, int *, int);*/

    x = sonar->west;

    XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

    while (x < sonar->east)
        {    
        xy_raster (sonar, x, sonar->south, &rastx, &s, SCREEN);
        xy_raster (sonar, x, sonar->north, &rastx, &n, SCREEN);

        sprintf (grid_label, "%d", (int)x);

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                    WhitePixelOfScreen(XtScreen(sonar->nav_window)));

        XDrawString(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                    sonar->nav_gc, rastx+lab_off, s - lab_off,
                    grid_label, strlen(grid_label));
        XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, rastx+lab_off, s - lab_off,
                    grid_label, strlen(grid_label));

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 

        XDrawLine(XtDisplay(sonar->nav_window), 
            XtWindow(sonar->nav_window), sonar->nav_gc, rastx, n, rastx, s);
        XDrawLine(XtDisplay(sonar->nav_window), 
            sonar->nav_pixmap, sonar->nav_gc, rastx, n, rastx, s);

        x += grid_int;

        }

    /*    Draw horizontal grid lines    */

    y = sonar->south;

    while (y < sonar->north)
        {    
        xy_raster (sonar, sonar->west, y, &w, &rasty, SCREEN);
        xy_raster (sonar, sonar->east, y, &e, &rasty, SCREEN);


        sprintf (grid_label, "%d", (int)y);

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc, sonar->redPixel.pixel); 

        XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                        sonar->nav_gc, w, rasty, e, rasty);
        XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                        sonar->nav_gc, w, rasty, e, rasty);

        XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                    WhitePixelOfScreen(XtScreen(sonar->nav_window)));

        XDrawString(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                    sonar->nav_gc, w + lab_off, rasty - lab_off,
                    grid_label, strlen(grid_label));
        XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, w + lab_off, rasty - lab_off,
                    grid_label, strlen(grid_label));


        y += grid_int;

        }



    return;

}

/*****************************************************************************
 *
 *    plot_mercator_grid()
 *
 *    Plots and labels (if label_flag != 0) a Mercator grid on the screen
 */
void plot_mercator_grid(MainSonar *sonar, int dgrid_sec,
			UNUSED int label_flag,
			UNUSED ImageHeader *image_params)
{
    char grid_label[10];

    int crossw = 5;
    int lon_startdeg, lat_startdeg, lat_startmin, lon_startmin;
    int x, y;
    int lat_deg, lat_minutes, lat_seconds, lon_deg, lon_minutes, lon_seconds;

    double lat_min, lon_min, lat_max, lon_max;    
    double lat_startsec, lon_startsec;
    double lat_start, lon_start;
    double lat, lon, lat_sec, lon_sec;
    double north, east;    
    double delta_grid;

    /*    Compute lat. and long. to start and end
     */

    delta_grid = (double)dgrid_sec / 3600.0;

    utmgeo (&lat_min, &lon_min, sonar->c_lon, sonar->west, 
                                       sonar->south, 'n', sonar->ellipsoid);
    utmgeo (&lat_max, &lon_max, sonar->c_lon, sonar->east,
                                       sonar->north, 'n', sonar->ellipsoid);
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

    XSetLineAttributes(XtDisplay(sonar->nav_window), sonar->nav_gc, 1, 
                                        LineSolid, CapButt, JoinRound);

    /*    Loop on lat and lon drawing crosses and labels 
     */
    for (lat = lat_start; lat < lat_max; lat += delta_grid)
        {
        for (lon = lon_start; lon < lon_max; lon += delta_grid)
            {
            geoutm (lat, lon, sonar->c_lon, &east, &north, sonar->ellipsoid);

            xy_raster (sonar, east, north, &x, &y, SCREEN);
/*
fprintf(stdout, "lon = %f\tlat = %f\neast = %f\twest = %f\nscreen west = %f\tscreen north = %f\nx = %d\ty = %d\n\n", lon, lat, east, north, sonar->west, sonar->north, x, y);

*/
            XSetForeground(XtDisplay(sonar->nav_window), sonar->nav_gc,
                             WhitePixelOfScreen(XtScreen(sonar->nav_window)));
                    
            XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                    sonar->nav_gc, x - crossw, y, x + crossw, y);
            XDrawLine(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                    sonar->nav_gc, x, y - crossw, x, y + crossw);

            XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, x - crossw, y, x + crossw, y);
            XDrawLine(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, x, y - crossw, x, y + crossw);

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

            XDrawString(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                    sonar->nav_gc, x + crossw + 5, y + 5,
                    grid_label, strlen(grid_label));
            XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                    sonar->nav_gc, x + crossw + 5, y + 5,
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

            XDrawString(XtDisplay(sonar->nav_window), XtWindow(sonar->nav_window),
                sonar->nav_gc, x - 3 * crossw, y - (crossw + 5),
                grid_label, strlen(grid_label));
            XDrawString(XtDisplay(sonar->nav_window), sonar->nav_pixmap,
                sonar->nav_gc, x - 3 * crossw, y - (crossw + 5),
                grid_label, strlen(grid_label));

            }

        }

    return;

}
