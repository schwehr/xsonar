#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void show_telemetry(w, client_data, call_data)
Widget w;
XtPointer *client_data;
XtPointer *call_data;
{


    struct draw *drawarea = (struct draw *) client_data;

    XSetWindowAttributes telemWindowAttr;    /* window attribute struct */
    unsigned long telemWindowValueMask;

    Arg args[40];
    Cardinal n;

    /*XColor grayScale[256];*/

    /*Colormap defaultColormap;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    /*int i;*/

    Widget telemDialog;
    Widget telemWindow;
    Widget telemButtonFrame;
    Widget telemMenuBar;
    Widget telemFileMenu;
    Widget telemFilePullDown;
    Widget telemClose;
    Widget telemBoard;
    Widget telemPing;
    Widget telemPingLabel;
    Widget telemTime;
    Widget telemTimeLabel;
    Widget telemPitch;
    Widget telemPitchLabel;
    Widget telemSwath;
    Widget telemSwathLabel;
    Widget telemRoll;
    /*Widget telemRollLabel;*/
    Widget telemRange;
    Widget telemRangeLabel;
    Widget telemAngle;
    Widget telemAngleLabel;
    Widget telemLatLon;
    Widget telemLatLonLabel;
    Widget telemAlt;
    Widget telemAltLabel;
    Widget telemPixel;
    Widget telemPixelLabel;
    Widget telemBearing;
    Widget telemBearingLabel;
    Widget telemHelpButton;
    Widget telemTotalDepth;
    Widget telemTotalDepthLabel;
    Widget telemFishDepth;
    Widget telemFishDepthLabel;
    Widget telemSlantRange;
    Widget telemSlantRangeLabel;
    Widget telemTemperature;
    Widget telemTemperatureLabel;

    void close_telem();

    XmUpdateDisplay(drawarea->shell);

    /*
     *   Set up the telemetry drawarea->shell.  I made the main container
     *   for this drawarea->shell a form widget.  Something different...
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    telemDialog = XtCreatePopupShell("TelemDialog",
             transientShellWidgetClass, drawarea->shell, args, n);

    telemWindow = XtCreateWidget("TelemWindow", xmFormWidgetClass,
                      telemDialog, NULL, 0);
    XtManageChild(telemWindow);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    telemButtonFrame = XtCreateWidget("TelemButtonFrame",
                   xmFrameWidgetClass, telemWindow, args, n);
    XtManageChild(telemButtonFrame);


    telemMenuBar = XmCreateMenuBar(telemButtonFrame, "TelemMenuBar", NULL, 0);
    XtManageChild(telemMenuBar);

    telemFilePullDown =
         XmCreatePulldownMenu(telemMenuBar, "TelemFilePullDown", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    telemFileMenu =  XtCreateWidget("TelemFileMenu",
                        xmCascadeButtonWidgetClass, telemMenuBar, args, n);
    XtManageChild(telemFileMenu);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, telemFilePullDown); n++;
    XtSetValues(telemFileMenu, args, n);

    telemClose = XtCreateWidget("TelemClose",
         xmPushButtonWidgetClass, telemFilePullDown, NULL, 0);
    XtManageChild(telemClose);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'H'); n++;
    telemHelpButton = XtCreateWidget("TelemHelpButton",
                   xmCascadeButtonWidgetClass, telemMenuBar, args, n);

    /*
     *   Tell the menu bar that to place the help button on
     *   the far right (Motif style).
     */

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, telemHelpButton); n++;
    XtSetValues(telemMenuBar, args, n);

    XtManageChild(telemHelpButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, telemButtonFrame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    telemBoard = XtCreateWidget("TelemBoard", xmBulletinBoardWidgetClass,
                         telemWindow, args, n);
    XtManageChild(telemBoard);

    /*
     *   Make the XmText widgets uneditable ...
     */

    n = 0;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 40); n++;
    XtSetArg(args[n], XmNwidth, 60); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    telemPing = XtCreateWidget("TelemPing", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemPing);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 80); n++;
    XtSetArg(args[n], XmNcolumns, 20); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemTime = XtCreateWidget("TelemTime", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemTime);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 120); n++;
    XtSetArg(args[n], XmNcolumns, 20); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemLatLon = XtCreateWidget("TelemLatLon", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemLatLon);


    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 160); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemPitch = XtCreateWidget("TelemPitch", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemPitch);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 160); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemSwath = XtCreateWidget("TelemSwath", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemSwath);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemRoll = XtCreateWidget("TelemRoll", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemRoll);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemRange = XtCreateWidget("TelemRange", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemRange);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 240); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemAngle = XtCreateWidget("TelemAngle", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemAngle);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 240); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemAlt = XtCreateWidget("TelemAlt", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemAlt);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 280); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemPixel = XtCreateWidget("TelemPixel", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemPixel);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 280); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemBearing = XtCreateWidget("TelemBearing", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemBearing);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 320); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemFishDepth = XtCreateWidget("TelemFishDepth", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemFishDepth);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 320); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemTotalDepth = XtCreateWidget("TelemTotalDepth", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemTotalDepth);

    n = 0;
    XtSetArg(args[n], XmNx, 80); n++;
    XtSetArg(args[n], XmNy, 360); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemSlantRange = XtCreateWidget("TelemSlantRange", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemSlantRange);

    n = 0;
    XtSetArg(args[n], XmNx, 190); n++;
    XtSetArg(args[n], XmNy, 360); n++;
    XtSetArg(args[n], XmNcolumns, 4); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    telemTemperature = XtCreateWidget("TelemTemperature", xmTextWidgetClass,
                                       telemBoard, args, n);
    XtManageChild(telemTemperature);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ping #: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 40); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    telemPingLabel = XtCreateWidget("TelemPingLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemPingLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 120); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Lat, Lon: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemLatLonLabel = XtCreateWidget("TelemLatLonLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemLatLonLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 80); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Time: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemTimeLabel = XtCreateWidget("TelemTimeLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemTimeLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 160); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Pitch: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemPitchLabel = XtCreateWidget("TelemPitchLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemPitchLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 160); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Swath: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemSwathLabel = XtCreateWidget("TelemSwathLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemSwathLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Range: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemRangeLabel = XtCreateWidget("TelemRangeLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemRangeLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Roll: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemPitchLabel = XtCreateWidget("TelemPitchLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemPitchLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 240); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Beam Ang: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemAngleLabel = XtCreateWidget("TelemAngleLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemAngleLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 240); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Alt: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemAltLabel = XtCreateWidget("TelemAltLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemAltLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 280); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Pixel DN: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemPixelLabel = XtCreateWidget("TelemPixelLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemPixelLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 280); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Bearing: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemBearingLabel = XtCreateWidget("TelemBearingLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemBearingLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 320); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Fish   \nDepth: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemFishDepthLabel = XtCreateWidget("TelemFishDepthLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemFishDepthLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 320); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Depth: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemTotalDepthLabel = XtCreateWidget("TelemTotalDepthLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemTotalDepthLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 360); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Slant  \nRange: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemSlantRangeLabel = XtCreateWidget("TelemSlantRangeLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemSlantRangeLabel);

    n = 0;
    XtSetArg(args[n], XmNx, 120); n++;
    XtSetArg(args[n], XmNy, 360); n++;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Temp: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    telemTemperatureLabel = XtCreateWidget("TelemTemperatureLabel",
                             xmLabelWidgetClass, telemBoard, args, n);
    XtManageChild(telemTemperatureLabel);


    XtAddCallback(telemClose, XmNactivateCallback, 
                                      close_telem, drawarea);

    XtRealizeWidget(telemDialog);

   /*
    *    Create a colormap entry for this drawarea->shell to match the grayscale
    *    colormap of the main drawarea->shell.  This will keep the display from
    *    blinking if the lower entries of the colormap are filled by
    *    another application.
    */

    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(telemDialog),
                                      main_window_valuemask, &main_window_attr);



    /*
     *   Set the main format for each XmText widget.
     */

    XmTextSetString(telemPing, "    0");
    XmTextSetString(telemTime, "00/00/00 00:00:00");
    XmTextSetString(telemLatLon, "00d 00.00N -000d 00.00W");

    XtSetSensitive(XtNameToWidget(drawarea->shell, "*TelemetryButton"), False);
    XtPopup(telemDialog, XtGrabNone);
 
    telemWindowAttr.cursor = XCreateFontCursor(XtDisplay(telemDialog),XC_arrow);
    telemWindowValueMask = CWCursor;
    XChangeWindowAttributes(XtDisplay(telemDialog), XtWindow(telemDialog),
                         telemWindowValueMask, &telemWindowAttr);

    return;

}

void close_telem(w, client_data, call_data)
Widget    w;
XtPointer client_data;
XtPointer call_data;
{

    struct draw *drawarea = (struct draw *) client_data;
    Widget parent_widget;
    Widget button;

    parent_widget = w;

    while(!XtIsTransientShell(parent_widget))
         parent_widget = XtParent(parent_widget);


    button = XtNameToWidget(drawarea->shell, "*TelemetryButton");
    XtSetSensitive(button, TRUE);

    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    return;

}

typedef struct {
    char *time_buf;
    char *latlon_buf;
    char *ping_buf;
    char *pitch_buf;
    char *roll_buf;
    char *swath_buf;
    char *range_buf;
    char *angle_buf;
    char *alt_buf;
    char *pixel_buf;
    char *bearing_buf;
    char *depth_buf;
    char *fishDepth_buf;
    char *slantRange_buf;
    char *temperature_buf;
   } Telemetry;

void print_telem(x, y, drawarea)
unsigned long x;
unsigned long y;
struct draw *drawarea;
{
 
    Widget telemPing;
    Widget telemTime;
    Widget telemLatLon;
    Widget telemPitch;
    Widget telemRoll;
    Widget telemSwath;
    Widget telemRange;
    Widget telemAngle;
    Widget telemAlt;
    Widget telemPixel;
    Widget telemBearing;
    Widget telemFishDepth;
    Widget telemTotalDepth;
    Widget telemSlantRange;
    Widget telemTemperature;

    Widget testShell;

    Telemetry telemetry;

    unsigned long ping;

    void message_display();
    void usgsTelem();
    void qmipsTelem();

    /*Arg args[40];*/
    /*Cardinal  n;*/

    int num_scans;

    num_scans = inbytes / scansize;

    if(y < 0)
         y = 0;

    if(y > 511)
         y = 511;

    ping = y * data_reducer;

    if(ping > num_scans)
         {
     /*
      *  Check to see if the message display shell has been realized.  If so,
      *  do not pop up another one, just return.
      */

         testShell = XtNameToWidget(drawarea->shell, "*MessageDialog");
         if(testShell == NULL)
             message_display(drawarea, NODATA);
         return;
         }

    telemPing = XtNameToWidget(drawarea->shell, "*TelemPing");
    telemLatLon = XtNameToWidget(drawarea->shell, "*TelemLatLon");
    telemTime = XtNameToWidget(drawarea->shell, "*TelemTime");
    telemPitch = XtNameToWidget(drawarea->shell, "*TelemPitch");
    telemRoll = XtNameToWidget(drawarea->shell, "*TelemRoll");
    telemSwath = XtNameToWidget(drawarea->shell, "*TelemSwath");
    telemRange = XtNameToWidget(drawarea->shell, "*TelemRange");
    telemAngle = XtNameToWidget(drawarea->shell, "*TelemAngle");
    telemAlt = XtNameToWidget(drawarea->shell, "*TelemAlt");
    telemPixel = XtNameToWidget(drawarea->shell, "*TelemPixel");
    telemBearing = XtNameToWidget(drawarea->shell, "*TelemBearing");
    telemFishDepth = XtNameToWidget(drawarea->shell, "*TelemFishDepth");
    telemTotalDepth = XtNameToWidget(drawarea->shell, "*TelemTotalDepth");
    telemSlantRange = XtNameToWidget(drawarea->shell, "*TelemSlantRange");
    telemTemperature = XtNameToWidget(drawarea->shell, "*TelemTemperature");

    if(image != NULL)
        {
        telemetry.time_buf = (char *) XtCalloc(50, 1);
        telemetry.ping_buf = (char *) XtCalloc(50, 1);
        telemetry.latlon_buf = (char *) XtCalloc(50, 1);
        telemetry.pitch_buf = (char *) XtCalloc(50, 1);
        telemetry.roll_buf = (char *) XtCalloc(50, 1);
        telemetry.swath_buf = (char *) XtCalloc(50, 1);
        telemetry.range_buf = (char *) XtCalloc(50, 1);
        telemetry.angle_buf = (char *) XtCalloc(50, 1);
        telemetry.alt_buf = (char *) XtCalloc(50, 1);
        telemetry.pixel_buf = (char *) XtCalloc(50, 1);
        telemetry.bearing_buf = (char *) XtCalloc(50, 1);
        telemetry.fishDepth_buf = (char *) XtCalloc(50, 1);
        telemetry.depth_buf = (char *) XtCalloc(50, 1);
        telemetry.slantRange_buf = (char *) XtCalloc(50, 1);
        telemetry.temperature_buf = (char *) XtCalloc(50, 1);

        if(drawarea->fileType == USGS)
            usgsTelem(&telemetry, drawarea, x, y, ping);
        if(drawarea->fileType == QMIPS)
            qmipsTelem(&telemetry, drawarea, x, y, ping);

        XmTextSetString(telemPing, telemetry.ping_buf);
        XmTextSetString(telemLatLon, telemetry.latlon_buf);
        XmTextSetString(telemTime, telemetry.time_buf);
        XmTextSetString(telemPitch, telemetry.pitch_buf);
        XmTextSetString(telemRoll, telemetry.roll_buf);
        XmTextSetString(telemSwath, telemetry.swath_buf);
        XmTextSetString(telemRange, telemetry.range_buf);
        XmTextSetString(telemAngle, telemetry.angle_buf);
        XmTextSetString(telemAlt, telemetry.alt_buf);
        XmTextSetString(telemPixel, telemetry.pixel_buf);
        XmTextSetString(telemBearing, telemetry.bearing_buf);
        XmTextSetString(telemTotalDepth, telemetry.depth_buf);
        XmTextSetString(telemFishDepth, telemetry.fishDepth_buf);
        XmTextSetString(telemSlantRange, telemetry.slantRange_buf);
        XmTextSetString(telemTemperature, telemetry.temperature_buf);

        XtFree(telemetry.time_buf);
        XtFree(telemetry.latlon_buf);
        XtFree(telemetry.ping_buf);
        XtFree(telemetry.pitch_buf);
        XtFree(telemetry.roll_buf);
        XtFree(telemetry.swath_buf);
        XtFree(telemetry.range_buf);
        XtFree(telemetry.angle_buf);
        XtFree(telemetry.pixel_buf);
        XtFree(telemetry.bearing_buf);
        XtFree(telemetry.depth_buf);
        XtFree(telemetry.fishDepth_buf);
        XtFree(telemetry.slantRange_buf);
        XtFree(telemetry.temperature_buf);
        }
    else
        message_display(drawarea, NODATA);

    return;

}


void calcPosition(x, scaler, azimuth, east, north, calcEast, calcNorth)
int x; 
double scaler;
double azimuth;
double east, north;
double *calcEast, *calcNorth;
{

    int quadrangle;
    int port, stbd;

    double distance; 
    double newazi;
    double xdiff, ydiff;

    port = TRUE;
    stbd = TRUE;

    /*
     *    Determine if the point selected is on port or starboard.
     */

    if(x > 255)
        {
        stbd = False;
        x += 1;
        }
    else if(x < 255)
        port = False;
    else
        {
        *calcEast = east;    /*  Set the northings and eastings */
        *calcNorth = north;  /*  to the passed values as        */
        return;              /*  we are on the nadir line.      */
        }

    distance = (double) (x - 256) * scaler;

    /*
     *    Calculate which quadrangle the azimuth falls in.
     */

    if(azimuth > M_PI_2)
        {
        quadrangle = 2;
        newazi = azimuth - M_PI_2;
        }
    else if(azimuth > M_PI)
        {
        quadrangle = 3;
        newazi = azimuth - M_PI;
        }
    else if(azimuth > M_PI + M_PI_2)
        {
        quadrangle = 4;
        newazi = azimuth - (M_PI + M_PI_2);
        }
    else 
        {
        quadrangle = 1;
        newazi = azimuth;
        }

    /*
     *    Begin calculating new position for selected point.
     */

    switch(quadrangle)
        {
        case 1:
            if(port)
                {
                xdiff = -distance * cos(newazi);
                ydiff = distance * sin(newazi);
                }
            else
                {
                xdiff = -distance * cos(newazi);
                ydiff = distance * sin(newazi);
                }
            break;
        case 2:
            if(port)
                {
                xdiff = distance * sin(newazi);
                ydiff = distance * cos(newazi);
                }
            else
                {
                xdiff = distance * sin(newazi);
                ydiff = distance * cos(newazi);
                }
            break;
        case 3:
            if(port)
                {
                xdiff = distance * cos(newazi);
                ydiff = -distance * sin(newazi);
                }
            else
                {
                xdiff = distance * cos(newazi);
                ydiff = -distance * sin(newazi);
                }
            break;
        case 4:
            if(port)
                {
                xdiff = -distance * sin(newazi);
                ydiff = -distance * cos(newazi);
                }
            else
                {
                xdiff = -distance * sin(newazi);
                ydiff = -distance * cos(newazi);
                }
            break;
        default:
            break;    /*  do nothing  */
        }             /*  end switch  */


    /*
     *    Now calculate the true easting and northing 
     *    for the selected point and return.
     */
/*
fprintf(stdout, "x = %d\n", x);
fprintf(stdout, "x diff = %f\tydiff = %f\tnewazi = %f\n", xdiff, ydiff, newazi);
fprintf(stdout, "distance = %f\t-distance = %f\n\n", distance, -distance);
*/


    *calcEast = east + xdiff;
    *calcNorth = north + ydiff;

    return;

}

void usgsTelem(telem, drawarea, x, y, ping)
Telemetry *telem;
struct draw *drawarea;
unsigned long x, y;
unsigned long ping;
{
 
    int i;
    /*int hour, min;*/
    int range;

    /*double sec;*/

    short cal[6];
    short invstardate();

    long stardate_factor;
    long julian();
    /*long pixelValue;*/

    double metersPerPixel;
    double beamAngle;
    double lon, lat;
    double east, north;
    /*double aziDegrees;*/

    void getCentralLon();
    void calcPosition();
    void utmgeo();

    if(scans[ping]->year < 0)
        {
        scans[ping]->year = 70;
        scans[ping]->djday = 1.0;
        }

    cal[0] = (scans[ping]->year + 1900);
    cal[1] = 1;
    for(i = 2; i < 6; i++)
         cal[i] = 0;
    stardate_factor = julian(cal);

    invstardate((scans[ping]->djday + (double)stardate_factor), cal);


    sprintf(telem->time_buf, "%02d/%02d/%04d %02d:%02d:%02d",
             cal[1], cal[2], scans[ping]->year + 1900, cal[3], cal[4], cal[5]);

    sprintf(telem->ping_buf, "%5d", ping + drawarea->top_ping);
    sprintf(telem->pitch_buf, "%5.1f", scans[ping]->pitch);
    sprintf(telem->roll_buf, "%5.1f", scans[ping]->roll);
    sprintf(telem->swath_buf, "%d", scans[ping]->swath_width);
    sprintf(telem->alt_buf, "%6.1f", scans[ping]->alt);
    sprintf(telem->depth_buf, "%6.1f", scans[ping]->total_depth);
    sprintf(telem->fishDepth_buf, "%6.1f", scans[ping]->depth);
    sprintf(telem->slantRange_buf, "%6.1f", scans[ping]->range);
    sprintf(telem->temperature_buf, "%6.1f", scans[ping]->temperature);

    if(scans[ping]->swath_width > 0)
        {
        metersPerPixel = ((double)swath_width / (datasize / data_reducer));

        range = (int)((double)abs(x - 256) * metersPerPixel);

        if(scans[ping]->alt > 0.0)
            beamAngle = RAD_TO_DEG * 
                           atan((double)range / (double) scans[ping]->alt);
        else
            beamAngle = 0;
        }

    sprintf(telem->range_buf, "%5d", range);
    sprintf(telem->angle_buf, "%5.1lf", beamAngle);
/*
    pixelValue = XGetPixel(image, x, y) - 192;
    sprintf(telem->pixel_buf, "%5d", (int) ((float)pixelValue / 63.0 * 255.0));
*/

    if(!drawarea->clon)
        getCentralLon(drawarea);

    calcPosition(x, metersPerPixel, scans[ping]->utm_azi,
                   scans[ping]->utm_e, scans[ping]->utm_n, &east, &north);
    utmgeo (&lat, &lon, (double) drawarea->clon, east, north, 'n');


    if(lat < 90 && fabs(lon) < 180)
        {
        sprintf(telem->latlon_buf, "%2dd %4.2fN %3dd %4.2fW",
          (int)lat, (lat - (int) lat) * 60.0,
          (int)lon, fabs(lon - (int)lon) * 60.0);
        }

    sprintf(telem->bearing_buf, "%5.1f", scans[ping]->utm_azi * 57.295780);

    return;

}


void qmipsTelem(telem, drawarea, x, y, ping)
Telemetry *telem;
struct draw *drawarea;
unsigned long x, y;
unsigned long ping;
{

  /*int range;*/

    /*long pixelValue;*/

    /*double metersPerPixel;*/
    /*double beamAngle;*/
    /*double lon, lat;*/
/*
    fprintf(stdout, "day = %d\n", qmipsScans[ping]->day);
    fprintf(stdout, "month = %d\n", qmipsScans[ping]->month);
    fprintf(stdout, "year = %d\n", qmipsScans[ping]->year);
    fprintf(stdout, "hour = %d\n", qmipsScans[ping]->hour);
    fprintf(stdout, "minute = %d\n", qmipsScans[ping]->minute);
    fprintf(stdout, "sec = %d\n", qmipsScans[ping]->seconds);
    fprintf(stdout, "sec tenths = %f\n", (float)qmipsScans[ping]->tenthsSeconds
/ 1000);
    fprintf(stdout, "lon = %f\n", qmipsScans[ping]->navLongitude);
    fprintf(stdout, "latitude = %f\n", qmipsScans[ping]->navEasting);
    fprintf(stdout, "ship speed = %f\n", qmipsScans[ping]->navFishSpeed);
    fprintf(stdout, "ship's course = %f\n", qmipsScans[ping]->navShipGyro);
    fprintf(stdout, "altSource = %d\n", qmipsScans[ping]->altSource);
    fprintf(stdout, "alt = %f\n", qmipsScans[ping]->APaltitude);
    fprintf(stdout, "ping = %d\n", qmipsScans[ping]->pingNumber);

    sprintf(telem->time_buf, "%02d/%02d/%02d %02d:%02d:%02d",
              qmipsScans[ping]->day, qmipsScans[ping]->month, 
              qmipsScans[ping]->year, qmipsScans[ping]->hour, 
              qmipsScans[ping]->minute, qmipsScans[ping]->seconds);

    sprintf(telem->ping_buf, "%5d", ping + drawarea->top_ping);

    sprintf(telem->pitch_buf, "%5.1f", qmipsScans[ping]->telemFishPitch);
    sprintf(telem->roll_buf, "%5.1f", qmipsScans[ping]->telemFishRoll);
    sprintf(telem->swath_buf, "%5d", qmipsScans[ping]->ch1_rangeScale * 2);
    sprintf(telem->alt_buf, "%6.1f", qmipsScans[ping]->APaltitude);

    metersPerPixel = ((double) qmipsScans[ping]->ch1_rangeScale * 2 / 
                                                 (datasize / data_reducer));
    if(!metersPerPixel)
        metersPerPixel = ((double)swath_width / (datasize / data_reducer));

    range = (int)((double)abs(x - 256) * metersPerPixel);
    beamAngle = RAD_TO_DEG * atan((double)range / qmipsScans[ping]->APaltitude);

    sprintf(telem->range_buf, "%5d", range);
    sprintf(telem->angle_buf, "%5.1f", beamAngle);

    pixelValue = XGetPixel(image, x, y) - 192;
    sprintf(telem->pixel_buf, "%5d", (int) ((float)pixelValue / 63.0 * 255.0));

    lat = qmipsScans[ping]->navEasting;
    lon = qmipsScans[ping]->navLongitude;

    if(lat < 90 && fabs(lon) < 180)
        {
        sprintf(telem->latlon_buf, "%2dd %4.2fN %3dd %4.2fW",
          (int)lat, (lat - (int) lat) * 60.0,
          (int)lon, fabs(lon - (int)lon) * 60.0);
        }

    sprintf(telem->bearing_buf, "%5.1f", qmipsScans[ping]->navShipGyro);

*/
    return;

}
