/*
 * showimage.c 
 *
 * This is a waterfall display for sonar data demultiplexed by "qmdemux".
 * The program will display altitude data (optional) for each 512 scans.
 * Display size is currently limited to a height of 512 pixels and a
 * width of the current scansize.  Will be adding resizing capabilities
 * in the future.  In addition, error checking is still minimal, needs
 * to be updated for display checking and server configurations.
 * 
 * William W. Danforth
 * U.S. Geological Survey
 * 7/30/91
 *
 */

#include "showimage2.h"
#include "sonar.h"
#include  "iconBitmap.h"

struct long_pos_buf *scans[8192];
struct qmips_sonar *qmipsScans[4096];

/*******************************
 *      GLOBAL VARIABLES       *
 *******************************/

int fp1;                  /* file handle                          */
int inbytes;              /* bytes read from sonar file           */

XImage *image;            /* X image structure to sonar data      */
XImage *subimage;         /* X image structure for zoomed image.  */
XImage *magnified_image;  /* X image for magnified zoom image     */

int swath_width;          /* variables made external for          */
int scansize;             /* several widgets                      */
int datasize;
int headsize;
int fileHeaderSize;
int data_reducer;
int doingEvent;

     /* place for the raw data and telemetry info */

unsigned char *sonar_data;
unsigned char *subbottom_data;
unsigned char *trailer;
unsigned char *zoom_data;
unsigned char *sub_sample_sonar;
unsigned short *sixteenBitData;
unsigned short *zoomSixteenBitData;
unsigned char *preMagnifySonar;     /* This is for the data structure when */
                                    /* using TrueColor (temp date storage  */
unsigned char *display_data;

int main(argc,argv)
int argc;
char *argv[];
{

    Widget
     traceButton,

     portAltButton,
     portAltOnButton,
     portAltOffButton,
     stbdAltButton,
     stbdAltOnButton,
     stbdAltOffButton,
     portAltSubMenu,
     stbdAltSubMenu,

     portShowBeamLimitButton,
     portShowBeamLimitOnButton,
     portShowBeamLimitOffButton,
     stbdShowBeamLimitButton,
     stbdShowBeamLimitOnButton,
     stbdShowBeamLimitOffButton,
     portShowBeamLimitSubMenu,
     stbdShowBeamLimitSubMenu,

     optionsButton,
     gotoStartButton,
     gotoPingButton,
     optionsPullDownMenu,
     mainWindow,               /* top-level container */
     clearButton,              /* clear the workspace */
     exitButton,               /* exit the program    */
     drawFrame,                /* frame for drawArea  */
     imageButton,
     imagePullDownMenu,
     displayButton,
     displaySubMenu,
     readSonarButton,
     brightenButton,
     brightenSubMenu,
     mainStretch,
     mainEqualize,
     fileButton,
     filePullDownMenu,
     fileRetrieveButton,
     fileName,
     helpButton,
     flipcolormap,
     redisplayButton,
     zoomAltMenu,
     zoomAltPullDown,
     zoomBeamMenu,
     zoomBeamPullDown,
     zoomArea,
     zoomButton,
     zoomButtonFrame,
     zoomBrighten,
     zoomBrightenSubMenu,
     zoomEqualize,
     zoomStretch,

     zoomBeamButton,
     zoomBeamSubMenu,
     zoomBeamPortButton,
     zoomBeamStbdButton,

     zoomCorrectAltButton,
     zoomCorrectAltSubMenu,
     zoomCorrectPortButton,
     zoomCorrectStbdButton,

     zoomDialog,
     zoomDone,
     zoomFrame,
     zoomHelpButton,
     zoomMagnify,
     zoomMenuBar,
     zoomMessage,
     zoomFilePullDown,
     zoomFileMenu,
     zoomOptionsPullDown,
     zoomOptionsMenu,
     zoomReset,

     zoomSeparator1,
     zoomSeparator2,

     zoomPortAltButton,
     zoomPortAltOnButton,
     zoomPortAltOffButton,
     zoomPortAltSubMenu,

     zoomStbdAltButton,
     zoomStbdAltOnButton,
     zoomStbdAltOffButton,
     zoomStbdAltSubMenu,

     zoomPortShowBeamLimitButton,
     zoomPortShowBeamLimitOnButton,
     zoomPortShowBeamLimitOffButton,
     zoomStbdShowBeamLimitButton,
     zoomStbdShowBeamLimitOnButton,
     zoomStbdShowBeamLimitOffButton,
     zoomPortShowBeamLimitSubMenu,
     zoomStbdShowBeamLimitSubMenu,

     zoomWindow,
     zoomWriteAlts,
     zoomWriteBeamLimits,
     menuBar,
     telemetrybutton,
     getCentralLonButton;
/*
     working;
*/

    Widget featureRemoveButton;
    Widget fileTypeButton;
    Widget fileTypePullDownMenu;
    Widget qmipsButton;
    Widget usgsButton;
    Widget undefinedButton;
    Widget rasterButton;
    Widget separator1;
    Widget separator2;
    Widget zoomSaveImageButton;
    Widget zoomFlipColorButton;
    Widget mainSaveImageButton;
    Widget stbdBitShiftScale;
    Widget stbdBitShiftFrame;
    Widget portBitShiftScale;
    Widget portBitShiftFrame;
    Widget bitShiftButton;
    Widget bitShiftSubMenu;
    Widget bitShiftShow;
    Widget bitShiftHide;

    Pixmap iconPixmap;


    XSetWindowAttributes main_window_attr;    /* window attribute struct */
    unsigned long main_window_valuemask;
/*
    Atom ATOM_WM_COLMAP_WIN;
    XtAppContext topLevelApp;
    XVisualInfo info;             * visual structure of current visual *
*/

    XFontStruct *label_font;

    XVisualInfo *visualList, visualTemplate;
    Visual *defaultVisual;         /* visual structure of default visual */
    int numberOfVisuals;

    int numCells;                 /* max number of color cells for display */
    int depth;                    /* depth of screen -- will 
                                     need to query the server
                                     for this. */
    int i;                        /* place holders */
    Status result;                /* general X results structure
                                     for return info */
    Arg args[40];                 /* arguments for XtSetValues */
    Cardinal n;                   /* X definition of an int */
    Colormap default_colormap;    /* colormap from server */


	/*
	 *   Setup a gray scale list and XColor arrry
	 *   for the TrueColor display colormap
	 */

    char *grayNames[] = {
         "gray0", "gray2", "gray4", "gray6", "gray8", "gray10", "gray12",
         "gray14", "gray16", "gray18", "gray20", "gray22", "gray24", "gray26",
         "gray28", "gray30", "gray32", "gray34", "gray36", "gray38", "gray40",
         "gray42", "gray44", "gray46", "gray48", "gray50", "gray52", "gray54",
         "gray56", "gray58", "gray60", "gray62", "gray64", "gray66", "gray68",
         "gray70", "gray72", "gray74", "gray76", "gray78", "gray80", "gray82",
         "gray84", "gray86", "gray88", "gray90", "gray92", "gray94", "gray96",
         "gray98", "gray100"
	     };


/*    Declare the callbacks ...
 *
 */
    void exit_callback();
    void repaint_image();
    void equalize_image();
    void stretch_image();
    void drawarea_input();
    void drawarea_clear();
    void file_select();
    void zoom_area();
    void zoom_reset();
    void zoom_done();
    void zoom_magnify();
    void show_telemetry();
    void show_image();
    void flip_color_map();
    void correct_altitude();
    void write_altitude();
    void traceBeamLimits();
    void writeBeamLimits();
    void goto_start();
    void goto_ping();
    void set_file_type();
    void featureRemove();
    void newCentralLon();
    void displayBitShifter();
    void setAltDrawMode();
    void setBeamLimitDrawMode();
    void create_trace();

    unsigned long pixel;        /* pixel value in colormap */
    unsigned long plane_masks[8];    /* number of planes in system */
    int shift;                 /* number of bits to shift */
    unsigned short intensity;    /* 16 bit intensity */
    unsigned long cell;            /* pixel number */
    int gray_level;            /* 8 bit value */
    int number_of_colormaps;

    struct draw drawboard;        /* main structure to pass in callbacks */

    void message_display();

    /* Start 'er up */

    if(INTEL)
        fprintf(stderr, "machine byte order is INTEL\n");
    else if (MOTOROLA)
        fprintf(stderr, "machine byte order is MOTOROLA\n");
    else
        fprintf(stderr, "machine byte order is UNDEFINED\n");

    if(!INTEL && !MOTOROLA)
        {
        fprintf(stderr, "Could not determine machine byte order.\n");
        fprintf(stderr, "Exiting ...\n");
        exit(-1);
        }

    /*
     *   Initialize some variables for when the display is popped up.
     */

    drawboard.top_ping = 0;
    drawboard.magnify = 1;
    drawboard.bitsPerPixel = 0;
    drawboard.clon = 0;
    drawboard.portBitShift = 4;
    drawboard.stbdBitShift = 4;
    drawboard.zoomPixmap = (int) NULL;

    doingEvent = 0;

    fp1 = -1;

    image = NULL;
    subimage = NULL;
    magnified_image = NULL;

    sixteenBitData = (unsigned short *) XtCalloc(512 * 512, sizeof(short));

    /*
     *   Initialize a block of scan structures to hold ping header info.
     *   See sonar.h for the definition of long_pos_buf.
     */

    for(i = 0; i < 8192; i++)
        scans[i] = (struct long_pos_buf *) XtCalloc(256, 1);


     /*
      *   Set the file type to NOTYPE
      *   to be sure the user selects a file type before viewing.
      */

    drawboard.fileType = NOTYPE;

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNmaxWidth, 522); n++;
    XtSetArg(args[n], XmNmaxHeight, 673); n++;
    XtSetArg(args[n], XmNminWidth, 522); n++;
    XtSetArg(args[n], XmNminHeight, 590); n++;
    drawboard.shell = XtAppInitialize(&drawboard.topLevelApp, 
                         "ShowImage", NULL, 0, &argc, argv, NULL, args, n);


    visualTemplate.screen = DefaultScreen(XtDisplay(drawboard.shell));
    visualList = XGetVisualInfo(XtDisplay(drawboard.shell), VisualScreenMask,
                           &visualTemplate, &numberOfVisuals);

    fprintf(stderr, "Number of visuals for this display is: %d\n", 
                                                           numberOfVisuals);

    for(i = 0; i < numberOfVisuals; i++, visualList++)
        {
        switch(visualList->class)
            {
            case PseudoColor:
                fprintf(stderr, "Visual %d class is: PseudoColor, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            case TrueColor:
                fprintf(stderr, "Visual %d class is: TrueColor, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            case DirectColor:
                fprintf(stderr, "Visual %d class is: DirectColor, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            case StaticGray:
                fprintf(stderr, "Visual %d class is: StaticGray, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            case StaticColor:
                fprintf(stderr, "Visual %d class is: StaticColor, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            case GrayScale:
                fprintf(stderr, "Visual %d class is: GrayScaleColor, ", i);
                fprintf(stderr, "depth is: %d\n", visualList->depth);
                break;
            }
        }

    defaultVisual = DefaultVisual(XtDisplay(drawboard.shell),
                                   DefaultScreen(XtDisplay(drawboard.shell)));

    drawboard.visual = defaultVisual;

    switch(defaultVisual->class)
        {
        case PseudoColor:
            fprintf(stderr, "Default visual is: PseudoColor\n");
            break;
        case TrueColor:
            fprintf(stderr, "Default visual is: TrueColor\n");
            break;
        case DirectColor:
            fprintf(stderr, "Default visual is: DirectColor\n");
            break;
        case StaticGray:
            fprintf(stderr, "Default visual is: StaticGray\n");
            break;
        case StaticColor:
            fprintf(stderr, "Default visual is: StaticColor\n");
            break;
        case GrayScale:
            fprintf(stderr, "Default visual is: GrayScaleColor\n");
            break;
        }
 

    depth = DefaultDepth(XtDisplay(drawboard.shell), 
                              DefaultScreen(XtDisplay(drawboard.shell)));

    fprintf(stderr, "Default depth of screen = %ld\n", depth); 

    if(depth < 8)
        {
        fprintf(stderr, "Screen must be at least 8 bits deep.\n");
        fprintf(stderr, "Exiting ...\n");
        exit(-1);
        }

    /*
     * Create Widget hierarchy.  All widgets are created *without*
     * any resources set.  We'll set them in a resource file.
     */
    
    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    mainWindow = XtCreateWidget("mainWindow", xmFormWidgetClass,
                   drawboard.shell, args, n);
    XtManageChild(mainWindow);


    /* 
     *     create the main window menubar, and create the buttons for
     *    the menubar.  The buttons which will be pulldown menus
     *     are created as CascadeButtonWidgetClass widgets.
     *    This setup will be contained in the first pane.
     */

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    menuBar = XmCreateMenuBar(mainWindow, "menuBar", args, n);
    XtManageChild(menuBar);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    fileButton = XtCreateWidget("fileButton",
                xmCascadeButtonWidgetClass, menuBar, args, n);
    XtManageChild(fileButton);

    filePullDownMenu = XmCreatePulldownMenu(menuBar, "fileDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, filePullDownMenu); n++;
    XtSetValues(fileButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'I'); n++;
    imageButton = XtCreateWidget("imageButton", 
                xmCascadeButtonWidgetClass, menuBar, args, n);
    XtManageChild(imageButton);

    imagePullDownMenu = XmCreatePulldownMenu(menuBar,
                            "imagePullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, imagePullDownMenu); n++;
    XtSetValues(imageButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'O'); n++;
    optionsButton = XtCreateWidget("optionsButton",
                xmCascadeButtonWidgetClass, menuBar, args, n);
    XtManageChild(optionsButton);

    optionsPullDownMenu = XmCreatePulldownMenu(menuBar,
                            "optionsPullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, optionsPullDownMenu); n++;
    XtSetValues(optionsButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'H'); n++;
    helpButton = XtCreateWidget("helpButton",
                xmCascadeButtonWidgetClass, menuBar, args, n);

    /*
     *    Tell the menu bar that to place the help button on
     *    the far right (Motif style).  Placement is critical
     *    here, as the help button must be create last.
     */

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, helpButton); n++;
    XtSetValues(menuBar, args, n);

    XtManageChild(helpButton);

    n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
     XtSetArg(args[n], XmNshadowThickness, 5); n++;
     XtSetArg(args[n], XmNheight, 8); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, menuBar); n++;
    separator1 = XtCreateWidget("Separator1",
                    xmSeparatorWidgetClass, mainWindow, args, n);
    XtManageChild(separator1);

    /*
     *    Add a label widget in the second pane which will contain
     *    the filename of the current file being displayed.
      */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("File is: ", XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    fileName = XtCreateWidget("FileName",
                    xmLabelWidgetClass, mainWindow, args, n);
    XtManageChild(fileName);

    n = 0;
     XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
     XtSetArg(args[n], XmNshadowThickness, 5); n++;
     XtSetArg(args[n], XmNheight, 8); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, fileName); n++;
    separator2 = XtCreateWidget("Separator2",
                    xmSeparatorWidgetClass, mainWindow, args, n);
    XtManageChild(separator2);

    /*
     *    Create a frame which will contain the drawarea widget
     *    in the third pane.
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
/*
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
*/
    drawFrame = XtCreateWidget("drawFrame", xmFrameWidgetClass,
                      mainWindow, args, n);
    XtManageChild(drawFrame);

    /*
     *    Create the main drawing area to hold the sonar image.
     */

    drawboard.graphics = XtCreateWidget("drawArea",
                xmDrawingAreaWidgetClass, drawFrame, NULL, 0);
    XtManageChild(drawboard.graphics);

    /*
     *    Bit shift scale widget
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, drawFrame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    stbdBitShiftFrame = XtCreateWidget("StbdBitShiftFrame", xmFrameWidgetClass,
                    mainWindow, args, n);

    n = 0;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Port Shift Right (>>)",
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 16); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue, drawboard.stbdBitShift); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    stbdBitShiftScale = XtCreateWidget("StbdBitShiftScale",
                    xmScaleWidgetClass, stbdBitShiftFrame, args, n);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, stbdBitShiftFrame); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, drawFrame); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    portBitShiftFrame = XtCreateWidget("PortBitShiftFrame", xmFrameWidgetClass,
                    mainWindow, args, n);

    n = 0;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Stbd Shift Right (>>)",
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 16); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue, drawboard.portBitShift); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    portBitShiftScale = XtCreateWidget("PortBitShiftScale",
                    xmScaleWidgetClass, portBitShiftFrame, args, n);


    /*
     *    Now create the various buttons on the pull down menus
     *    contained in the first pane.
     */

    /*
     *    Create the buttons for the file pulldown:
      */

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'R'); n++;
    fileRetrieveButton = XtCreateWidget("FileRetrieveButton",
                xmPushButtonWidgetClass, filePullDownMenu, args, n);
    XtManageChild(fileRetrieveButton);
    
    fileTypeButton = XtCreateWidget("FileTypeButton",
                    xmCascadeButtonWidgetClass, filePullDownMenu, NULL, 0);
    XtManageChild(fileTypeButton);

    fileTypePullDownMenu = XmCreatePulldownMenu(filePullDownMenu,
                                        "FileTypePullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, fileTypePullDownMenu); n++;
    XtSetValues(fileTypeButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'Q'); n++;
    qmipsButton = XtCreateWidget("QmipsButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(qmipsButton);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'U'); n++;
    usgsButton = XtCreateWidget("UsgsButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(usgsButton);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'R'); n++;
    rasterButton = XtCreateWidget("RasterButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(rasterButton);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'N'); n++;
    undefinedButton = XtCreateWidget("UndefinedButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(undefinedButton);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'Q'); n++;
    exitButton = XtCreateWidget("exitButton",
                xmPushButtonWidgetClass, filePullDownMenu, args, n);
    XtManageChild(exitButton);

    
    /*
      *    Buttons for the Image button pulldown:
     */

    clearButton = XtCreateManagedWidget("clearButton",
                    xmPushButtonWidgetClass, imagePullDownMenu, NULL, 0);

    mainSaveImageButton = XtCreateManagedWidget("MainSaveImageButton",
                xmPushButtonWidgetClass, imagePullDownMenu, NULL, 0);

    /*
     *    Make the display button a cascade widget.  Will be a second
     *    layer cascade under the image pulldown.
     */

    displayButton = XtCreateManagedWidget("displayButton",
                xmCascadeButtonWidgetClass, imagePullDownMenu, NULL, 0);
    
    displaySubMenu = XmCreatePulldownMenu(displayButton, 
                                              "displaySubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, displaySubMenu); n++;
    XtSetValues(displayButton, args, n);

    readSonarButton = XtCreateManagedWidget("readSonarButton",
                xmPushButtonWidgetClass, displaySubMenu, NULL, 0);

    telemetrybutton = XtCreateManagedWidget("TelemetryButton",
                xmPushButtonWidgetClass, displaySubMenu, NULL, 0);

    traceButton = XtCreateManagedWidget("TraceButton",
                xmPushButtonWidgetClass, displaySubMenu, NULL, 0);

    brightenButton = XtCreateWidget("brightenButton",
                xmCascadeButtonWidgetClass, imagePullDownMenu, NULL, 0);
    XtManageChild(brightenButton);

    brightenSubMenu = XmCreatePulldownMenu(imagePullDownMenu,
                                    "brightenSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, brightenSubMenu); n++;
    XtSetValues(brightenButton, args, n);

    mainEqualize = XtCreateManagedWidget("MainEqualize",
                xmPushButtonWidgetClass, brightenSubMenu, NULL, 0);

    mainStretch = XtCreateManagedWidget("MainStretch",
                xmPushButtonWidgetClass, brightenSubMenu, NULL, 0);

    flipcolormap = XtCreateManagedWidget("flipColorMap",
                xmPushButtonWidgetClass, imagePullDownMenu, NULL, 0);

    redisplayButton = XtCreateWidget("ReDisplayButton",
                xmPushButtonWidgetClass, imagePullDownMenu, NULL, 0);
    XtManageChild(redisplayButton);

    zoomButton = XtCreateWidget("ZoomButton",
                xmPushButtonWidgetClass, imagePullDownMenu, NULL, 0);
    XtManageChild(zoomButton);

    bitShiftButton = XtCreateWidget("BitShiftButton",
                xmCascadeButtonWidgetClass, imagePullDownMenu, NULL, 0);
    XtManageChild(bitShiftButton);

    bitShiftSubMenu = XmCreatePulldownMenu(bitShiftButton,
                                    "BitShiftSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, bitShiftSubMenu); n++;
    XtSetValues(bitShiftButton, args, n);

    bitShiftShow = XtCreateManagedWidget("BitShiftShow",
                xmPushButtonWidgetClass, bitShiftSubMenu, NULL, 0);

    bitShiftHide = XtCreateManagedWidget("BitShiftHide",
                xmPushButtonWidgetClass, bitShiftSubMenu, NULL, 0);

    /*
     *     Create the buttons for the options pull down button:
     */

    gotoStartButton = XtCreateManagedWidget("gotoStartButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    
    gotoPingButton = XtCreateManagedWidget("gotoPingButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, NULL, 0);

    getCentralLonButton = XtCreateManagedWidget("GetCentralLonButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, NULL, 0);

    /*
     *  Set the buttons and pull downs for showing the altitude trace.
     */

    portAltButton = XtCreateWidget("PortAltButton",
                xmCascadeButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(portAltButton);

    portAltSubMenu = XmCreatePulldownMenu(optionsPullDownMenu,
                                                    "PortAltSubMenu", NULL, 0);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, portAltSubMenu); n++;
    XtSetValues(portAltButton, args, n);

    portAltOnButton = XtCreateManagedWidget("PortAltOnButton",
                xmPushButtonWidgetClass, portAltSubMenu, NULL, 0);

    portAltOffButton = XtCreateManagedWidget("PortAltOffButton",
                xmPushButtonWidgetClass, portAltSubMenu, NULL, 0);

    stbdAltButton = XtCreateWidget("StbdAltButton",
                xmCascadeButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(stbdAltButton);

    stbdAltSubMenu = XmCreatePulldownMenu(optionsPullDownMenu,
                                                    "StbdAltSubMenu", NULL, 0);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, stbdAltSubMenu); n++;
    XtSetValues(stbdAltButton, args, n);

    stbdAltOnButton = XtCreateManagedWidget("StbdAltOnButton",
                xmPushButtonWidgetClass, stbdAltSubMenu, NULL, 0);

    stbdAltOffButton = XtCreateManagedWidget("StbdAltOffButton",
                xmPushButtonWidgetClass, stbdAltSubMenu, NULL, 0);

    /*
     *  Set the buttons and pull downs for showing the trace
     *  of the range limits used in the beam pattern correction.
     */

    portShowBeamLimitButton = XtCreateWidget("PortShowBeamLimitButton",
                xmCascadeButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(portShowBeamLimitButton);

    portShowBeamLimitSubMenu = XmCreatePulldownMenu(optionsPullDownMenu,
                                                    "PortShowBeamLimitSubMenu", NULL, 0);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, portShowBeamLimitSubMenu); n++;
    XtSetValues(portShowBeamLimitButton, args, n);

    portShowBeamLimitOnButton = XtCreateManagedWidget("PortShowBeamLimitOnButton",
                xmPushButtonWidgetClass, portShowBeamLimitSubMenu, NULL, 0);

    portShowBeamLimitOffButton = XtCreateManagedWidget("PortShowBeamLimitOffButton",
                xmPushButtonWidgetClass, portShowBeamLimitSubMenu, NULL, 0);

    stbdShowBeamLimitButton = XtCreateWidget("StbdShowBeamLimitButton",
                xmCascadeButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(stbdShowBeamLimitButton);

    stbdShowBeamLimitSubMenu = XmCreatePulldownMenu(optionsPullDownMenu,
                                                    "StbdShowBeamLimitSubMenu", NULL, 0);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, stbdShowBeamLimitSubMenu); n++;
    XtSetValues(stbdShowBeamLimitButton, args, n);

    stbdShowBeamLimitOnButton = XtCreateManagedWidget("StbdShowBeamLimitOnButton",
                xmPushButtonWidgetClass, stbdShowBeamLimitSubMenu, NULL, 0);

    stbdShowBeamLimitOffButton = XtCreateManagedWidget("StbdShowBeamLimitOffButton",
                xmPushButtonWidgetClass, stbdShowBeamLimitSubMenu, NULL, 0);


    /*
     *    Now create the widgets for the zoom area.  The zoom area 
     *    is a top level shell which will be popped up using
     *    the XtCallbackNone callback.  The children will be
     *     a form widget containing a label widget, a frame
     *    widget, and a drawing area widget.
     */


    /*
     *     Set the resize resource for the popup shell to true.
     *     Must do this to honor geometry requests from children.
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNnoResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    zoomDialog = XtCreatePopupShell("ZoomDialog",
            topLevelShellWidgetClass, drawboard.shell, args, n);

    zoomWindow = XtCreateWidget("ZoomWindow", xmFormWidgetClass,
                   zoomDialog, NULL, 0);
    XtManageChild(zoomWindow);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;

    zoomButtonFrame = XtCreateWidget("ZoomButtonFrame",
                xmFrameWidgetClass, zoomWindow, args, n);
    XtManageChild(zoomButtonFrame);


    zoomMenuBar = XmCreateMenuBar(zoomButtonFrame, "ZoomMenuBar", NULL, 0);
    XtManageChild(zoomMenuBar);

    zoomFilePullDown =
        XmCreatePulldownMenu(zoomMenuBar, "ZoomFilePullDown", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    zoomFileMenu =  XtCreateWidget("ZoomFileMenu",
                    xmCascadeButtonWidgetClass, zoomMenuBar, args, n);
    XtManageChild(zoomFileMenu);

    zoomOptionsPullDown =
        XmCreatePulldownMenu(zoomMenuBar, "ZoomOptionsPullDown", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomFilePullDown); n++;
    XtSetValues(zoomFileMenu, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'O'); n++;
    zoomOptionsMenu =  XtCreateWidget("ZoomOptionsMenu",
                    xmCascadeButtonWidgetClass, zoomMenuBar, args, n);
    XtManageChild(zoomOptionsMenu);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomOptionsPullDown); n++;
    XtSetValues(zoomOptionsMenu, args, n);

    zoomAltPullDown =
        XmCreatePulldownMenu(zoomMenuBar, "ZoomAltPullDown", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'A'); n++;
    zoomAltMenu =  XtCreateWidget("ZoomAltMenu",
                    xmCascadeButtonWidgetClass, zoomMenuBar, args, n);
    XtManageChild(zoomAltMenu);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomAltPullDown); n++;
    XtSetValues(zoomAltMenu, args, n);

    XtSetArg(args[n], XmNmnemonic, 'B'); n++;
    zoomBeamMenu =  XtCreateWidget("ZoomBeamMenu",
                    xmCascadeButtonWidgetClass, zoomMenuBar, args, n);
    XtManageChild(zoomBeamMenu);

    zoomBeamPullDown =
        XmCreatePulldownMenu(zoomMenuBar, "ZoomBeamPullDown", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomBeamPullDown); n++;
    XtSetValues(zoomBeamMenu, args, n);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'H'); n++;
    zoomHelpButton = XtCreateWidget("ZoomHelpButton",
                xmCascadeButtonWidgetClass, zoomMenuBar, args, n);

    /*
     *    Tell the menu bar that to place the help button on
     *    the far right (Motif style).
     */

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, zoomHelpButton); n++;
    XtSetValues(zoomMenuBar, args, n);

    XtManageChild(zoomHelpButton);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, zoomButtonFrame); n++;
    zoomSeparator1 = XtCreateWidget("ZoomSeparator1", xmSeparatorWidgetClass,
                            zoomWindow, args, n);
    XtManageChild(zoomSeparator1);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Select area of zoom on main\ndisplay with btn 2",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, zoomSeparator1); n++;
    zoomMessage = XtCreateWidget("ZoomMessage",
                    xmLabelWidgetClass, zoomWindow, args, n);
    XtManageChild(zoomMessage);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, zoomMessage); n++;
    zoomSeparator2 = XtCreateWidget("ZoomSeparator2", xmSeparatorWidgetClass,
                            zoomWindow, args, n);
    XtManageChild(zoomSeparator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, zoomSeparator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    zoomFrame = XtCreateWidget("ZoomFrame",
                xmFrameWidgetClass, zoomWindow, args, n);
    XtManageChild(zoomFrame);

    zoomArea = XtCreateWidget("ZoomArea",
                xmDrawingAreaWidgetClass, zoomFrame, NULL, 0);
    XtManageChild(zoomArea);

    zoomDone = XtCreateWidget("ZoomDone",
                xmPushButtonWidgetClass, zoomFilePullDown, NULL, 0);
    XtManageChild(zoomDone);

    zoomMagnify = XtCreateWidget("ZoomMagnify",
                xmPushButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(zoomMagnify);

    zoomSaveImageButton = XtCreateWidget("ZoomSaveImageButton",
                xmPushButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(zoomSaveImageButton);

    zoomFlipColorButton = XtCreateWidget("ZoomFlipColorButton",
                xmPushButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(zoomFlipColorButton);

    zoomReset = XtCreateWidget("ZoomReset",
                xmPushButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(zoomReset);

    featureRemoveButton = XtCreateWidget("FeatureRemoveButton",
                xmPushButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(featureRemoveButton);

    zoomBrighten = XtCreateWidget("ZoomBrighten",
                xmCascadeButtonWidgetClass, zoomOptionsPullDown, NULL, 0);
    XtManageChild(zoomBrighten);

    zoomBrightenSubMenu = XmCreatePulldownMenu(zoomAltPullDown,
                                "ZoomBrightenSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomBrightenSubMenu); n++;
    XtSetValues(zoomBrighten, args, n);

    zoomEqualize = XtCreateManagedWidget("ZoomEqualize",
                xmPushButtonWidgetClass, zoomBrightenSubMenu, NULL, 0);

    zoomStretch = XtCreateManagedWidget("ZoomStretch",
                xmPushButtonWidgetClass, zoomBrightenSubMenu, NULL, 0);


    /*
     *  Start creating widgets for the altitude correction.
     */

    zoomCorrectAltButton = XtCreateWidget("ZoomCorrectAltButton",
                xmCascadeButtonWidgetClass, zoomAltPullDown, NULL, 0);
    XtManageChild(zoomCorrectAltButton);

    zoomCorrectAltSubMenu = XmCreatePulldownMenu(zoomAltPullDown,
                                "ZoomCorrectAltSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomCorrectAltSubMenu); n++;
    XtSetValues(zoomCorrectAltButton, args, n);

    zoomCorrectPortButton = XtCreateWidget("ZoomCorrectPortButton",
                xmPushButtonWidgetClass, zoomCorrectAltSubMenu, NULL, 0);
    XtManageChild(zoomCorrectPortButton);

    zoomCorrectStbdButton = XtCreateWidget("ZoomCorrectStbdButton",
                xmPushButtonWidgetClass, zoomCorrectAltSubMenu, NULL, 0);
    XtManageChild(zoomCorrectStbdButton);

    /*
     *  Start creating widgets for the beam pattern limit tracing.
     */

    zoomBeamButton = XtCreateWidget("ZoomBeamButton",
                xmCascadeButtonWidgetClass, zoomBeamPullDown, NULL, 0);
    XtManageChild(zoomBeamButton);

    zoomBeamSubMenu = XmCreatePulldownMenu(zoomBeamPullDown,
                                "ZoomBeamSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomBeamSubMenu); n++;
    XtSetValues(zoomBeamButton, args, n);

    zoomBeamPortButton = XtCreateWidget("ZoomBeamPortButton",
                xmPushButtonWidgetClass, zoomBeamSubMenu, NULL, 0);
    XtManageChild(zoomBeamPortButton);

    zoomBeamStbdButton = XtCreateWidget("ZoomBeamStbdButton",
                xmPushButtonWidgetClass, zoomBeamSubMenu, NULL, 0);
    XtManageChild(zoomBeamStbdButton);

    /*
     *  Button to OK writing new beam pattern correction limits
     *  to file header.
     */

    zoomWriteBeamLimits = XtCreateWidget("ZoomWriteBeamLimits",
                xmPushButtonWidgetClass, zoomBeamPullDown, NULL, 0);
    XtManageChild(zoomWriteBeamLimits);

    /*
     *  Button to OK writing new altitudes into file header.
     */

    zoomWriteAlts = XtCreateWidget("ZoomWriteAlts",
                xmPushButtonWidgetClass, zoomAltPullDown, NULL, 0);
    XtManageChild(zoomWriteAlts);

    /*
     *  Buttons to turn on or off the red line that traces the altitude
     *  in the zoomed window.
     */

    zoomPortAltButton = XtCreateWidget("ZoomPortAltButton",
                xmCascadeButtonWidgetClass, zoomAltPullDown, NULL, 0);
    XtManageChild(zoomPortAltButton);

    zoomPortAltSubMenu = XmCreatePulldownMenu(zoomAltPullDown,
                                "ZoomPortAltSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomPortAltSubMenu); n++;
    XtSetValues(zoomPortAltButton, args, n);

    zoomPortAltOnButton = XtCreateManagedWidget("ZoomPortAltOnButton",
                xmPushButtonWidgetClass, zoomPortAltSubMenu, NULL, 0);

    zoomPortAltOffButton = XtCreateManagedWidget("ZoomPortAltOffButton",
                xmPushButtonWidgetClass, zoomPortAltSubMenu, NULL, 0);

    zoomStbdAltButton = XtCreateWidget("ZoomStbdAltButton",
                xmCascadeButtonWidgetClass, zoomAltPullDown, NULL, 0);
    XtManageChild(zoomStbdAltButton);

    zoomStbdAltSubMenu = XmCreatePulldownMenu(zoomAltPullDown,
                                "ZoomStbdAltSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomStbdAltSubMenu); n++;
    XtSetValues(zoomStbdAltButton, args, n);

    zoomStbdAltOnButton = XtCreateManagedWidget("ZoomStbdAltOnButton",
                xmPushButtonWidgetClass, zoomStbdAltSubMenu, NULL, 0);

    zoomStbdAltOffButton = XtCreateManagedWidget("ZoomStbdAltOffButton",
                xmPushButtonWidgetClass, zoomStbdAltSubMenu, NULL, 0);

    /*
     *  Set the buttons and pull downs for showing the trace
     *  in the zoomed window of the range limits used in the beam pattern correction.
     */

    zoomPortShowBeamLimitButton = XtCreateWidget("ZoomPortShowBeamLimitButton",
                xmCascadeButtonWidgetClass, zoomBeamPullDown, NULL, 0);
    XtManageChild(zoomPortShowBeamLimitButton);

    zoomPortShowBeamLimitSubMenu = XmCreatePulldownMenu(zoomBeamPullDown,
                                "ZoomPortShowBeamLimitSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomPortShowBeamLimitSubMenu); n++;
    XtSetValues(zoomPortShowBeamLimitButton, args, n);

    zoomPortShowBeamLimitOnButton = XtCreateManagedWidget("ZoomPortShowBeamLimitOnButton",
                xmPushButtonWidgetClass, zoomPortShowBeamLimitSubMenu, NULL, 0);

    zoomPortShowBeamLimitOffButton = XtCreateManagedWidget("ZoomPortShowBeamLimitOffButton",
                xmPushButtonWidgetClass, zoomPortShowBeamLimitSubMenu, NULL, 0);

    zoomStbdShowBeamLimitButton = XtCreateWidget("ZoomStbdShowBeamLimitButton",
                xmCascadeButtonWidgetClass, zoomBeamPullDown, NULL, 0);
    XtManageChild(zoomStbdShowBeamLimitButton);

    zoomStbdShowBeamLimitSubMenu = XmCreatePulldownMenu(zoomBeamPullDown,
                                "ZoomStbdShowBeamLimitSubMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, zoomStbdShowBeamLimitSubMenu); n++;
    XtSetValues(zoomStbdShowBeamLimitButton, args, n);

    zoomStbdShowBeamLimitOnButton = XtCreateManagedWidget("ZoomStbdShowBeamLimitOnButton",
                xmPushButtonWidgetClass, zoomStbdShowBeamLimitSubMenu, NULL, 0);

    zoomStbdShowBeamLimitOffButton = XtCreateManagedWidget("ZoomStbdShowBeamLimitOffButton",
                xmPushButtonWidgetClass, zoomStbdShowBeamLimitSubMenu, NULL, 0);


    /*
     *    Callback lists    
     *
     */

    XtAddCallback(exitButton, XmNactivateCallback, 
                 exit_callback, NULL);

    XtAddCallback(featureRemoveButton, XmNactivateCallback,
                 featureRemove, &drawboard);

    XtAddCallback(readSonarButton, XmNactivateCallback,
                 show_image, &drawboard);

    XtAddCallback(portAltOnButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(zoomPortAltOnButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(stbdAltOnButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(zoomStbdAltOnButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(portAltOffButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(zoomPortAltOffButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(stbdAltOffButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(zoomStbdAltOffButton, XmNactivateCallback,
                 setAltDrawMode, &drawboard);

    XtAddCallback(portShowBeamLimitOnButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(zoomPortShowBeamLimitOnButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(stbdShowBeamLimitOnButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(zoomStbdShowBeamLimitOnButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(portShowBeamLimitOffButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(zoomPortShowBeamLimitOffButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(stbdShowBeamLimitOffButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(zoomStbdShowBeamLimitOffButton, XmNactivateCallback,
                 setBeamLimitDrawMode, &drawboard);

    XtAddCallback(mainEqualize, XmNactivateCallback,
                         equalize_image, &drawboard);

    XtAddCallback(mainStretch, XmNactivateCallback,
                         stretch_image, &drawboard);

    XtAddCallback(flipcolormap, XmNactivateCallback,
                 flip_color_map, &drawboard);

    XtAddCallback(zoomArea, XmNinputCallback,
             drawarea_input, &drawboard);

    XtAddCallback(drawboard.graphics, XmNinputCallback,
             drawarea_input, &drawboard);

    XtAddCallback(drawboard.graphics, XmNexposeCallback,
             repaint_image, &drawboard);

    XtAddCallback(redisplayButton, XmNactivateCallback,
             repaint_image, &drawboard);

    XtAddCallback(clearButton, XmNactivateCallback,
                 drawarea_clear, &drawboard);
    
    XtAddCallback(gotoStartButton, XmNactivateCallback,
                 goto_start, &drawboard);
    
    XtAddCallback(gotoPingButton, XmNactivateCallback,
                 goto_ping, &drawboard);
    
    XtAddCallback(getCentralLonButton, XmNactivateCallback,
                 newCentralLon, &drawboard);

    XtAddCallback(fileRetrieveButton, XmNactivateCallback,
                         file_select, &drawboard);

    XtAddCallback(mainSaveImageButton, XmNactivateCallback,
                         file_select, &drawboard);

    XtAddCallback(zoomSaveImageButton, XmNactivateCallback,
                         file_select, &drawboard);

    XtAddCallback(zoomFlipColorButton, XmNactivateCallback,
                         flip_color_map, &drawboard);

    XtAddCallback(telemetrybutton, XmNactivateCallback, 
                                    show_telemetry, &drawboard);

    XtAddCallback(zoomButton, XmNactivateCallback, 
                                  zoom_area, &drawboard);

    XtAddCallback(zoomDone, XmNactivateCallback, 
                                  zoom_done, &drawboard);

    XtAddCallback(zoomArea, XmNexposeCallback, 
                                  repaint_image, &drawboard);

    XtAddCallback(zoomReset, XmNactivateCallback, 
                                  zoom_reset, &drawboard);

    XtAddCallback(zoomMagnify, XmNactivateCallback, 
                                  zoom_magnify, &drawboard);

    XtAddCallback(zoomEqualize, XmNactivateCallback,
                                  equalize_image, &drawboard);

    XtAddCallback(zoomStretch, XmNactivateCallback,
                                  stretch_image, &drawboard);

    XtAddCallback(zoomCorrectPortButton, XmNactivateCallback,
                                  correct_altitude, &drawboard);

    XtAddCallback(zoomCorrectStbdButton, XmNactivateCallback,
                                  correct_altitude, &drawboard);

    XtAddCallback(zoomWriteAlts, XmNactivateCallback,
                                  write_altitude, &drawboard);

    XtAddCallback(zoomBeamStbdButton, XmNactivateCallback,
                                  traceBeamLimits, &drawboard);

    XtAddCallback(zoomBeamPortButton, XmNactivateCallback,
                                  traceBeamLimits, &drawboard);

    XtAddCallback(zoomWriteBeamLimits, XmNactivateCallback,
                                  writeBeamLimits, &drawboard);

    XtAddCallback(qmipsButton, XmNactivateCallback,
                                  set_file_type, &drawboard);

    XtAddCallback(usgsButton, XmNactivateCallback,
                                  set_file_type, &drawboard);

    XtAddCallback(rasterButton, XmNactivateCallback,
                                  set_file_type, &drawboard);

    XtAddCallback(undefinedButton, XmNactivateCallback,
                                  set_file_type, &drawboard);

    XtAddCallback(bitShiftShow, XmNactivateCallback,
                                  displayBitShifter, &drawboard);

    XtAddCallback(bitShiftHide, XmNactivateCallback,
                                  displayBitShifter, &drawboard);

    XtAddCallback(portBitShiftScale, XmNvalueChangedCallback,
                                  show_image, &drawboard);

    XtAddCallback(stbdBitShiftScale, XmNvalueChangedCallback,
                                  show_image, &drawboard);

    XtAddCallback(traceButton, XmNactivateCallback,
                                  create_trace, &drawboard);

    /*
     *    Let 'er roll
     */

    XtRealizeWidget(drawboard.shell);


/*    printf("shell window = %d graphics window = %d\n",
            XtWindow(drawboard.shell), XtWindow(drawboard.graphics));
*/

    fprintf(stderr, "root window:\nwidth = %d, height = %d\n",
                    DisplayWidth(XtDisplay(drawboard.shell), 0),
                    DisplayHeight(XtDisplay(drawboard.shell), 0));


    /*
     *    Select the events to monitor in the main drawing window.
     */

    XSelectInput(XtDisplay(mainWindow),XtWindow(drawboard.graphics),
        ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
        FocusChangeMask);

    /*
     *    Create a GC for the drawing area widget so it can be
     *    drawn into.
     */

    drawboard.main_gc = XCreateGC(XtDisplay(drawboard.shell),
            RootWindowOfScreen(XtScreen(drawboard.graphics)),
                None, NULL);

    label_font = XLoadQueryFont(XtDisplay(drawboard.graphics),
            "-adobe-helvetica-*-r-*-*-*-120-*-*-*-*-*-*");
/*
            "-adobe-times-bold-r-normal--12-120-75-75-m-67-iso8859-1");
            "-adobe-courier-bold-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-courier-medium-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-helvetica-medium-r-normal--8-80-75-75-p-46-iso8859-1");
*/

    if(label_font == 0)
        message_display(&drawboard, BADFONT);
    else
        XSetFont(XtDisplay(drawboard.graphics), drawboard.main_gc,
                                                 label_font->fid);



     /*
      *   Get default colormap so it can be reset after the app finishes
      */

     default_colormap = DefaultColormap(XtDisplay(drawboard.shell),
                                  DefaultScreen(XtDisplay(drawboard.shell)));

     drawboard.graphics_colormap = 
                  XCreateColormap(XtDisplay(drawboard.shell),
                  XtWindow(drawboard.shell), 
                  DefaultVisualOfScreen(XtScreen(drawboard.shell)),
                  AllocNone);

     numCells = DisplayCells(XtDisplay(drawboard.shell), 
                          DefaultScreen(XtDisplay(drawboard.shell)));

	 fprintf(stderr, "number of color cells = %d\n", numCells);

     XListInstalledColormaps(XtDisplay(drawboard.shell),
                   XtWindow(drawboard.shell), &number_of_colormaps);

     fprintf(stderr,"Number of colormaps is %d\n", number_of_colormaps);

     if(defaultVisual->class == TrueColor)
         {
         for(i = 0; i < 50; i++)
              {
              if(!XParseColor(XtDisplay(drawboard.shell), default_colormap,
                  grayNames[i], &drawboard.grayMap[i]))
                  {
                  fprintf(stderr, "Unable to alloc color gray[%d]\n", i);
                  perror("Reason");
                  return(0);
                  }
              else
                 XAllocColor(XtDisplay(drawboard.shell), 
                       drawboard.graphics_colormap, &drawboard.grayMap[i]);

              drawboard.grayPixels[i] = drawboard.grayMap[i].pixel;
              }

          if(!XParseColor(XtDisplay(drawboard.shell), default_colormap,
                    "red", &drawboard.grayMap[50]))
              {
              fprintf(stderr, "Unable to alloc color Red\n");
              perror("Reason");
              return(0);
              }
          else
              XAllocColor(XtDisplay(drawboard.shell),
                        drawboard.graphics_colormap, &drawboard.grayMap[50]);

          drawboard.grayPixels[50] = drawboard.grayMap[50].pixel;
          }


     if(defaultVisual->class == PseudoColor)
          {
          result = XAllocColorCells(XtDisplay(drawboard.shell), 
                drawboard.graphics_colormap, True, plane_masks, 8, &pixel, 1);

          if(result == 0)
               {
               printf("color cell allocation failed.\n");
               exit(-1);
               }

          for(shift = 0; plane_masks[0] != (1 << shift); shift++)
               ;

          for(cell = 0; cell < numCells; cell++)
               drawboard.gray[cell].pixel = (cell << shift) | pixel;

          XQueryColors(XtDisplay(drawboard.shell), default_colormap, 
                                               drawboard.gray, numCells);


         /*
          *   Set up the color cells for a gray map.
          *   Start at cell (pixel) 192 to preserve the lower 192 color entries.
          *   This will keep the display from going crazy.
          */

          for(cell = 192, gray_level = 0; cell < 256; cell++, gray_level+=4)
               {
               if(cell == 255)
                    gray_level = cell;
               intensity = gray_level * 65535L / 255L;
               drawboard.gray[cell].red = drawboard.gray[cell].green =
                    drawboard.gray[cell].blue = intensity;
               drawboard.gray[cell].pixel = (cell << shift) | pixel;
               drawboard.gray[cell].flags = DoRed|DoGreen|DoBlue;
               }

          drawboard.gray[191].red = 65025;
          drawboard.gray[191].green = 0;
          drawboard.gray[191].blue = 0;
          drawboard.gray[191].pixel = 191;
          drawboard.gray[191].flags = DoRed|DoGreen|DoBlue;

          /*
           *   Install the colormap.  It is specific only to this application.
           */


          XStoreColors(XtDisplay(drawboard.shell), drawboard.graphics_colormap,
                                        drawboard.gray, 256);

          main_window_attr.cursor = XCreateFontCursor(XtDisplay(drawboard.shell), XC_cross);
          main_window_attr.colormap = drawboard.graphics_colormap;
          main_window_valuemask = CWColormap | CWCursor;

          /*
           *  Change the colormap attributes for both the main shell and
           *  the subwindow (drawing area in this case) as some window
           *  managers, like the 4Dwm of SGI, need explicit windows whereas
           *  others will shuffle the info down the widget list.
           */

          XChangeWindowAttributes(XtDisplay(drawboard.shell), 
                   XtWindow(drawboard.shell), main_window_valuemask, 
                   &main_window_attr);

          XChangeWindowAttributes(XtDisplay(drawboard.shell),
                   XtWindow(drawboard.graphics), main_window_valuemask,
                   &main_window_attr);

          XFreeColormap(XtDisplay(drawboard.shell), default_colormap);

          }

    iconPixmap = XCreatePixmapFromBitmapData(XtDisplay(drawboard.shell),
              XtWindow(drawboard.shell), iconImage, 62, 52,
              WhitePixelOfScreen(XtScreen(drawboard.shell)),
              BlackPixelOfScreen(XtScreen(drawboard.shell)),
              DefaultDepth(XtDisplay(drawboard.shell),
              DefaultScreen(XtDisplay(drawboard.shell))));


    n = 0;
    XtSetArg(args[n], XmNiconPixmap, iconPixmap); n++;
    XtSetValues(drawboard.shell, args, n);


    XSetForeground(XtDisplay(drawboard.shell), drawboard.main_gc,
           WhitePixelOfScreen(XtScreen(drawboard.shell)));

    XSetBackground(XtDisplay(drawboard.shell), drawboard.main_gc,
           BlackPixelOfScreen(XtScreen(drawboard.shell)));

    /*
     *   Create a pixmap to hold the image data and use it when redraw
     *   requests come through the pipe (expose events).
     */

    XSetForeground(XtDisplay(drawboard.shell), drawboard.main_gc,
             BlackPixelOfScreen(XtScreen(drawboard.shell)));

    drawboard.imagePixmap = XCreatePixmap(XtDisplay(drawboard.shell), 
                          XtWindow(drawboard.graphics), 512, 512, 
                          DefaultDepth(XtDisplay(drawboard.shell),
                          DefaultScreen(XtDisplay(drawboard.shell))));


    XFillRectangle(XtDisplay(drawboard.shell), drawboard.imagePixmap,
                    drawboard.main_gc, 0, 0, 512, 512);

    XSetForeground(XtDisplay(drawboard.shell), drawboard.main_gc,
             WhitePixelOfScreen(XtScreen(drawboard.shell)));

    XtSetSensitive(portAltOffButton, FALSE);
    XtSetSensitive(zoomPortAltOffButton, FALSE);
    XtSetSensitive(stbdAltOffButton, FALSE);
    XtSetSensitive(zoomStbdAltOffButton, FALSE);

    XtSetSensitive(portShowBeamLimitOnButton, True);
    XtSetSensitive(portShowBeamLimitOffButton, False);
    XtSetSensitive(zoomPortShowBeamLimitOnButton, True);
    XtSetSensitive(zoomPortShowBeamLimitOffButton, False);
    XtSetSensitive(stbdShowBeamLimitOnButton, True);
    XtSetSensitive(stbdShowBeamLimitOffButton, False);
    XtSetSensitive(zoomStbdShowBeamLimitOnButton, True);
    XtSetSensitive(zoomStbdShowBeamLimitOffButton, False);

    XtSetSensitive(bitShiftButton, FALSE);
    XtSetSensitive(bitShiftHide, FALSE);
/*
    XtSetSensitive(qmipsButton, FALSE);
*/

    XtAppMainLoop(drawboard.topLevelApp);

} 

/*    Callback Procedures
 *
 */

/*
void working_message(w, client_data, call_data)
Widget         w;
XtPointer     client_data;
XtPointer        call_data; 
{

    struct draw *drawboard = (struct draw *) client_data;
    int n;
    Arg args[40];
    XmString working_string;
    Widget workingbutton;
    Widget workingbox;
    Widget working_dialog;

    working_dialog = XmCreateWorkingDialog(drawboard->shell, "workingDialog", NULL, 0);
     working_dialog = XtCreatePopupShell("workingDialog",
                               xmDialogShellWidgetClass, drawboard->shell, NULL, 0);

    working_string = XmStringCreateLtoR("Reading image data ... Please wait.",
                                    XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNmessageString, working_string); n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_WORKING); n++;
    workingbox = XtCreateWidget("workingBox", xmMessageBoxWidgetClass,
                                working_dialog, args, n);

    workingbutton = XmMessageBoxGetChild(workingbox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(workingbutton);

    workingbutton = XmMessageBoxGetChild(workingbox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(workingbutton);

    workingbutton = XmMessageBoxGetChild(workingbox, XmDIALOG_OK_BUTTON);
    XtUnmanageChild(workingbutton);

    XtManageChild(workingbox);

    XmStringFree(working_string);

    sleep(1);
    XmUpdateDisplay(workingbox);

    show_image(w, drawboard);

    XtDestroyWidget(working_dialog);
    
    return;

}
*/

void drawarea_clear(w,client_data,callback_data)
     Widget                 w;
     caddr_t                 client_data;
     XmAnyCallbackStruct         *callback_data; 
{

    struct draw *drawarea = (struct draw * ) client_data;

    XClearWindow(XtDisplay(w), XtWindow(drawarea->graphics));
    return;
}

void exit_callback(w,client_data,callback_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct     *callback_data; 
{

    extern int fp1;

    extern unsigned char *display_data;
    extern unsigned char *sub_sample_sonar;
    extern unsigned char *zoom_data;

    extern XImage *image;
    extern XImage *subimage;
    extern XImage *magnified_image;

    close(fp1);

    if(image != NULL)
        {
        XFree((char *)image);
        XtFree((char *)display_data);
        printf("destroyed image and display_data\n");
        }

    if(magnified_image != NULL)
        {
        XFree((char *)magnified_image);
        XtFree((char *)zoom_data);
        printf("destroyed magnified image\n");
        }

    if(subimage != NULL)
        {
        XFree((char *)subimage);
        XtFree((char *)sub_sample_sonar);
        printf("destroyed subimage\n");
        }

    exit(1);
}

