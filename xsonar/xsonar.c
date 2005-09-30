/* 
 *    xsonar.c
 *
 *    Main program for the xsonar software.  This sets up the main display
 *    buttons and attaches various processing callbacks to each.  See the
 *    source code associated with each callback for information on the
 *    particular processing function.
 *
 *    Written for X11 Release 4 and Motif 1.1.3 on a DECstation 3100.
 *
 *    William Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA  02543
 *
 *    7/25/92
 */


#include "sonar_struct.h"
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/Separator.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>

#include "bitmaps.h"

#include <map.h>
#include "xsonar.h"


void all_done(Widget w, XtPointer client_data, XtPointer call_data);
void createColorMap(MainSonar *sonar);


int
main (int argc, char *argv[])
{

    Widget shell;
    Widget mainForm;
    Widget titleWindow;
    Widget buttonFrame;
    Widget mainButtonHolder;
    Widget histButton;
    Widget combineButton;
    Widget demuxButton;
    Widget processButton;
    Widget navButton;
    Widget orientButton;
    Widget setupButton;
    Widget exitButton;
    Widget programButton;
    Widget convertButton;
    Widget filterButton;
    Widget mapButton;

    Pixmap xsonar_pixmap;
    Pixmap icon_pixmap;
    GC xsonar_gc;

    Arg args[10];
    Cardinal n;

    XFontStruct *label_font;

    MainSonar sonar;

    char warningMessage[100];

    int i;
    /* 
     *   Set a few variables to keep various compilers happy.
     */

    if(INTEL)
        fprintf(stderr, "machine byte order is INTEL\n");
    else if (MOTOROLA)
        fprintf(stderr, "machine byte order is MOTOROLA\n");
    else
        {
        fprintf(stderr, "machine byte order is UNDEFINED\n");
        fprintf(stderr, "Exiting...check sonar_struct.h for machine type.");
        }


    for(i = 0; i < 3; i++)
        sonar.stretch_params[i] = 0;

    for(i = 0; i < 20; i++)
        {
        sonar.utm_west[i] = 0;
        sonar.utm_south[i] = 0;
        }

    sonar.south = 0;
    sonar.north = 0;
    sonar.east = 0;
    sonar.west = 0;

    sonar.mapSouth = 0;
    sonar.mapNorth = 0;
    sonar.mapEast = 0;
    sonar.mapWest = 0;

    sonar.rasterSouth = 0;
    sonar.rasterNorth = 0;
    sonar.rasterEast = 0;
    sonar.rasterWest = 0;

    sonar.setReturnWhite = True;
    sonar.setReturnBlack = False;

    sonar.portNormalizeValue = 6144;
    sonar.stbdNormalizeValue = 6144;
    sonar.portMaxNormalizeValue = 6144;
    sonar.stbdMaxNormalizeValue = 6144;

    sonar.number_of_images = 0;
    sonar.navInterval = 0;
    sonar.magnetic_declination = 0;
    sonar.c_lon = 0;
    sonar.mer_grid_int = 0;
    sonar.along_track = 0;
    sonar.across_track = 0;
    sonar.meterRange = 0;
    sonar.stbdOffset = 0;
    sonar.portOffset = 0;
    sonar.skipScans = 0;
    sonar.stbdBitShift = 0;
    sonar.portBitShift = 0;
    sonar.scale = 0;
    sonar.swath = 0;
    sonar.beamNumLines = 0;
    sonar.sixteenBit = 0;
    sonar.eightBit = 0;

    sonar.rasterFlag = FALSE;
    sonar.nadirTextOut = FALSE;
    sonar.tduFlag = TRUE;
    sonar.aldenFlag = FALSE;
    sonar.fish_azi_flag = FALSE;
    sonar.utm_flag = FALSE;
    sonar.mercator_flag = FALSE;
    sonar.latlon_flag = FALSE;
    sonar.magnetometer_flag = FALSE;
    sonar.eastnorth_flag = FALSE;
    sonar.demuxFlag = FALSE;
    sonar.navmerge_flag = FALSE;
    sonar.slant_flag = FALSE;
    sonar.destripe_flag = FALSE;
    sonar.beam_flag = FALSE;
    sonar.linear_flag = FALSE;
    sonar.equalize_flag = FALSE;
    sonar.adjustBeam = TRUE;
    sonar.tduPixelsNS = 201.5;
    sonar.beamAverageLines = 0;
    sonar.beamResponseAngle = 55;
    sonar.maxBeamAngle = 90;
    sonar.normalize = 0;
    sonar.bitShift = 1;
    sonar.removeRampPortFlag = 0;
    sonar.removeRampStbdFlag = 0;
    sonar.removeRampPortPivot = 0;
    sonar.removeRampStbdPivot = 0;
    sonar.startScanForRemoveRampStbd = 0;
    sonar.endScanForRemoveRampStbd = 0;
    sonar.startScanForRemoveRampPort = 0;
    sonar.endScanForRemoveRampPort = 0;
    sonar.removeRampRangeStartStbd = 0;
    sonar.removeRampRangeStartPort = 0;
    sonar.getCurveModelText = 0;

    sonar.mosaicRange = 0;

    sonar.infd = -1;
    sonar.outfd = -1;
    sonar.navfp = NULL;
 
    strcpy(sonar.currentDirectory, "./*");

    sonar.fileType = 0;

    sonar.ellipsoid = 1;

    sonar.findAltFlag = 0;
    sonar.findAltThreshold = 10;
    sonar.altManuallyCorrected = 0;
    sonar.useBeamLimits = 0;

    strcpy(sonar.infile, "No file selected");

    /*    
     *    open input file
     */

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNmaxWidth, 398); n++;
    XtSetArg(args[n], XmNmaxHeight, 248); n++;
    XtSetArg(args[n], XmNminWidth, 398); n++;
    XtSetArg(args[n], XmNminHeight, 248); n++;
    shell = XtAppInitialize(&sonar.toplevel_app, "XSonar", NULL, 0,
                                    &argc, argv, NULL, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    mainForm = XtCreateWidget("MainForm", xmFormWidgetClass,
                   shell, args, n);
    XtManageChild(mainForm);
/*
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    titleFrame = XtCreateWidget("TitleFrame",
                    xmFrameWidgetClass, mainForm, args, n);
    XtManageChild(titleFrame);
*/
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    titleWindow = XtCreateWidget("TitleWindow",
                    xmLabelWidgetClass, mainForm, args, n);
    XtManageChild(titleWindow);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, titleWindow); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    buttonFrame = XtCreateWidget("ButtonFrame",
                    xmFrameWidgetClass, mainForm, args, n);
    XtManageChild(buttonFrame);

    n = 0;
/*
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, titleWindow); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
*/
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    mainButtonHolder = XtCreateWidget("MainButtonHolder",
                xmRowColumnWidgetClass, buttonFrame, args, n);
    XtManageChild(mainButtonHolder);
/*
    n = 0;
    button1 = XtCreateWidget("Button1",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(button1);

    n = 0;
    button2 = XtCreateWidget("Button2",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(button2);
*/

    n = 0;
    setupButton = XtCreateWidget("SetupButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(setupButton);

    n = 0;
    demuxButton = XtCreateWidget("DemuxButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(demuxButton);

    n = 0;
    processButton = XtCreateWidget("Process",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(processButton);

     n = 0;
    histButton = XtCreateWidget("HistButton",
                xmPushButtonWidgetClass, mainButtonHolder, NULL, 0);
    XtManageChild(histButton);

    n = 0;
    navButton = XtCreateWidget("NavButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(navButton);

    orientButton = XtCreateWidget("OrientButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(orientButton);

    n = 0;
    mapButton = XtCreateWidget("MapButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(mapButton);

    n = 0;
    filterButton = XtCreateWidget("FilterButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(filterButton);

    n = 0;
    combineButton = XtCreateWidget("CombineButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(combineButton);

    n = 0;
    convertButton = XtCreateWidget("ConvertButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(convertButton);

    n = 0;
    exitButton = XtCreateWidget("ExitButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(exitButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString, 
              XmStringCreateLtoR("XSONAR V1.0", XmSTRING_DEFAULT_CHARSET)); n++;
    programButton = XtCreateWidget("ProgramButton",
                xmPushButtonWidgetClass, mainButtonHolder, args, n);
    XtManageChild(programButton);

    sonar.toplevel = shell;

    XtAddCallback(exitButton, XmNactivateCallback, all_done, NULL);
    XtAddCallback(navButton, XmNactivateCallback, show_nav, &sonar);
    XtAddCallback(setupButton, XmNactivateCallback, setup, &sonar);
    XtAddCallback(orientButton, XmNactivateCallback, orient_sonar, &sonar);
    XtAddCallback(demuxButton, XmNactivateCallback, demux, &sonar);
    XtAddCallback(histButton, XmNactivateCallback, histogram, &sonar);
    XtAddCallback(combineButton, XmNactivateCallback, combineImages, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, mergenav, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, slant, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, destripe, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, beam, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, stretch, &sonar);
    XtAddCallback(processButton, XmNactivateCallback, equalize, &sonar);
    XtAddCallback(convertButton, XmNactivateCallback, fileConvert, &sonar);
    XtAddCallback(filterButton, XmNactivateCallback, filter, &sonar);
    XtAddCallback(mapButton, XmNactivateCallback, mapCoast, &sonar);

    XtRealizeWidget(shell);

    xsonar_gc = XCreateGC(XtDisplay(shell), XtWindow(shell), None, NULL);

    label_font = XLoadQueryFont(XtDisplay(shell),
               "-adobe-helvetica-bold-r-normal-*-*-120-*-*-*-*-*-*");

    if(label_font == 0)
          {
          sprintf(warningMessage, "Could not find font, using default");
          messages(&sonar, warningMessage); /* FIX: added the &sonar... is this okay? */
          }
    else
          XSetFont(XtDisplay(shell), xsonar_gc, label_font->fid);


    xsonar_pixmap = XCreatePixmapFromBitmapData(XtDisplay(shell),
            XtWindow(shell), (char *) xsonar_bits, 100, 100, 
            WhitePixelOfScreen(XtScreen(shell)),
            BlackPixelOfScreen(XtScreen(shell)), 
			DefaultDepth(XtDisplay(shell), 
			DefaultScreen(XtDisplay(shell))));    

    icon_pixmap = XCreatePixmapFromBitmapData(XtDisplay(shell),
            XtWindow(shell), (char *) icon_bits, 40, 40, 
            WhitePixelOfScreen(XtScreen(shell)),
            BlackPixelOfScreen(XtScreen(shell)),    
			DefaultDepth(XtDisplay(shell),
			DefaultScreen(XtDisplay(shell))));    

    sonar.main_gc = XCreateGC(XtDisplay(sonar.toplevel),
                                       XtWindow(sonar.toplevel), None, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
    XtSetArg(args[n], XmNlabelPixmap, xsonar_pixmap); n++;
    XtSetValues(titleWindow, args, n);

    n = 0;
    XtSetArg(args[n], XmNiconPixmap, icon_pixmap); n++;
    XtSetValues(shell, args, n);

	/*
	 *   Now create a colormap for the application 
	 */

	createColorMap(&sonar);

    XtAppMainLoop(sonar.toplevel_app);

    /* control probably never gets here */
    return (EXIT_SUCCESS);
}

    
void all_done(UNUSED Widget w,
	      UNUSED XtPointer client_data, 
	      UNUSED XtPointer call_data)
{
    exit(EXIT_SUCCESS);
}

void createColorMap(MainSonar *sonar)
{

    Status result;
    XSetWindowAttributes main_window_attr;  /* window attribute struct */

    unsigned long main_window_valuemask;
    unsigned long cell;                      /* pixel number */
    unsigned long pixels[256];               /* pixel values in colormap */
    unsigned long plane_masks[8];            /* number of planes in system */

    XColor colors[256];              /*  For the PseudoColor visual   */
    Colormap default_colormap;              /* colormap from server */

/*
    XVisualInfo info;             * visual structure of current visual *
    XVisualInfo *visualList, visualTemplate;
*/
    Visual *defaultVisual;         /* visual structure of default visual */
/*
    int numberOfVisuals;
*/

    /*
     *   Get default colormap so it can be reset after the app finishes
     */

    default_colormap = DefaultColormap(XtDisplay(sonar->toplevel),
                    DefaultScreen(XtDisplay(sonar->toplevel)));

    sonar->colormap = XCreateColormap(XtDisplay(sonar->toplevel),
                XtWindow(sonar->toplevel),
                DefaultVisualOfScreen(XtScreen(sonar->toplevel)),
                AllocNone);

    fprintf(stderr, "Getting visual info...\n");

/*
    visualTemplate.screen = DefaultScreen(XtDisplay(sonar->toplevel));
    visualList = XGetVisualInfo(XtDisplay(sonar->toplevel), VisualScreenMask,
            &visualTemplate, &numberOfVisuals);
*/

    defaultVisual = DefaultVisual(XtDisplay(sonar->toplevel),
                            DefaultScreen(XtDisplay(sonar->toplevel)));

    if(defaultVisual->class == TrueColor)
        {
        fprintf(stderr, "Default visual is TrueColor\n");
        if(!XParseColor(XtDisplay(sonar->toplevel), sonar->colormap,
              "red", &sonar->redPixel))
            {
            fprintf(stderr, "Unable to alloc color Red\n");
            perror("Reason");
            return;
            }
        else
            {
            sonar->redPixel.pixel = 255;
            XAllocColor(XtDisplay(sonar->toplevel), sonar->colormap, 
                                                   &sonar->redPixel);
            }

        if(!XParseColor(XtDisplay(sonar->toplevel), sonar->colormap,
              "yellow", &sonar->yellowPixel))
            {
            fprintf(stderr, "Unable to alloc color Yellow\n");
            perror("Reason");
            return;
            }
        else
           XAllocColor(XtDisplay(sonar->toplevel), sonar->colormap,
                                                   &sonar->yellowPixel);

        if(!XParseColor(XtDisplay(sonar->toplevel), sonar->colormap,
              "green", &sonar->greenPixel))
            {
            fprintf(stderr, "Unable to alloc color Green\n");
            perror("Reason");
            return;
            }
        else
           XAllocColor(XtDisplay(sonar->toplevel), sonar->colormap,
                                                   &sonar->greenPixel);
        }

    if(defaultVisual->class == PseudoColor)
        {
        result = XAllocColorCells(XtDisplay(sonar->toplevel), sonar->colormap,
                           True, plane_masks, 8, pixels, 1);

        if(result == 0)
              {
                fprintf(stderr, "color cell allocation failed.\n");
              perror("Reason");
                exit(-1);
              }

        for(cell = 0; cell < 256; cell++)
            colors[cell].pixel = cell;

        XQueryColors(XtDisplay(sonar->toplevel), default_colormap, colors, 256);

        sonar->redPixel.red = colors[90].red = 65535;
        sonar->redPixel.blue = colors[90].blue = 0;
        sonar->redPixel.green = colors[90].green = 0;
        sonar->redPixel.pixel = colors[90].pixel = 90;
        sonar->redPixel.flags = colors[90].flags = DoRed | DoGreen | DoBlue;

        sonar->greenPixel.red = colors[91].red = 0;
        sonar->greenPixel.blue = colors[91].blue = 0;
        sonar->greenPixel.green = colors[91].green = 65535;
        sonar->greenPixel.pixel = colors[91].pixel = 91;
        sonar->greenPixel.flags = colors[91].flags = DoRed | DoGreen | DoBlue;

        sonar->yellowPixel.red = colors[92].red = 65535;
        sonar->yellowPixel.blue = colors[92].blue = 0;
        sonar->yellowPixel.green = colors[92].green = 65535;
        sonar->yellowPixel.pixel = colors[92].pixel = 92;
        sonar->yellowPixel.flags = colors[92].flags = DoRed | DoGreen | DoBlue;

        XStoreColors(XtDisplay(sonar->toplevel), sonar->colormap, colors, 256);

        main_window_attr.colormap = sonar->colormap;
        main_window_valuemask = CWColormap;

        XChangeWindowAttributes(XtDisplay(sonar->toplevel),
                    XtWindow(sonar->toplevel), main_window_valuemask,
                    &main_window_attr);

        }

    return;

}

