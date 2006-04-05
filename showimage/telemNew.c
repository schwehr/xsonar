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
    Widget telemRollLabel;
    Widget telemRange;
    Widget telemRangeLabel;
    Widget telemAngle;
    Widget telemAngleLabel;
    Widget telemLatLon;
    Widget telemLatLonLabel;
    Widget telemAlt;
    Widget telemAltLabel;
    Widget telemHelpButton;

    void close_telem();

    /*
     *   Set up the telemetry shell.  I made the main container
     *   for this shell a form widget.  Something different...
     */

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    telemDialog = XtCreatePopupShell("TelemDialog",
             topLevelShellWidgetClass, shell, args, n);

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

    XtAddCallback(telemClose, XmNactivateCallback, 
                                      close_telem, drawarea);

    XtRealizeWidget(telemDialog);

    /*
     *   Set the main format for each XmText widget.
     */

    XmTextSetString(telemPing, "    0");
    XmTextSetString(telemTime, "00/00/00 00:00:00");
    XmTextSetString(telemLatLon, "00d 00.00N -000d 00.00W");

    XtSetSensitive(telemetrybutton, False);
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

    while(!XtIsTopLevelShell(parent_widget))
         parent_widget = XtParent(parent_widget);


    button = XtNameToWidget(shell, "*TelemetryButton");
    XtSetSensitive(button, TRUE);

    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    return;

}

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

    int i;
    int hour, min;
    int num_scans;
    int range;

    double sec;

    short cal[6];
    short invstardate();

    char *time_buf;
    char *latlon_buf;
    char *ping_buf;
    char *pitch_buf;
    char *roll_buf;
    char *swath_buf;
    char *range_buf;
    char *angle_buf;
    char *alt_buf;

    long stardate_factor;
    long julian();
  
    double metersPerPixel;
    double beamAngle;

    unsigned long ping;

    void message_display();

    Arg args[40];
    int n;

    num_scans = inbytes / scansize;

    if(y < 0)
         y = 0;

    if(y > 511)
         y = 511;

    ping = y * data_reducer;

    if(ping > num_scans)
         {
         message_display(drawarea, NODATA);
         return;
         }

    telemPing = XtNameToWidget(shell, "*TelemPing");
    telemLatLon = XtNameToWidget(shell, "*TelemLatLon");
    telemTime = XtNameToWidget(shell, "*TelemTime");
    telemPitch = XtNameToWidget(shell, "*TelemPitch");
    telemRoll = XtNameToWidget(shell, "*TelemRoll");
    telemSwath = XtNameToWidget(shell, "*TelemSwath");
    telemRange = XtNameToWidget(shell, "*TelemRange");
    telemAngle = XtNameToWidget(shell, "*TelemAngle");
    telemAlt = XtNameToWidget(shell, "*TelemAlt");

    if(image != NULL)
        {
        cal[0] = (scans[ping]->year + 1900);
        cal[1] = 1;
        for(i = 2; i < 6; i++)
             cal[i] = 0;
        stardate_factor = julian(cal);

        time_buf = (char *) XtCalloc(1, 20);
        ping_buf = (char *) XtCalloc(1, 5);
        latlon_buf = (char *) XtCalloc(1, 23);
        pitch_buf = (char *) XtCalloc(1, 5);
        roll_buf = (char *) XtCalloc(1, 5);
        swath_buf = (char *) XtCalloc(1, 5);
        range_buf = (char *) XtCalloc(1, 5);
        angle_buf = (char *) XtCalloc(1, 5);
        alt_buf = (char *) XtCalloc(1, 5);

        invstardate(((double)scans[ping]->djday + stardate_factor), cal);
        sprintf(time_buf, "%02d/%02d/%02d %02d:%02d:%02d",
                  cal[1], cal[2], scans[ping]->year, cal[3], cal[4], cal[5]);

        sprintf(ping_buf, "%5d", ping + drawarea->top_ping);
        sprintf(pitch_buf, "%5.1f", scans[ping]->pitch);
        sprintf(roll_buf, "%5.1f", scans[ping]->roll);
        sprintf(swath_buf, "%5d", scans[ping]->swath_width);
        sprintf(alt_buf, "%6.1f", scans[ping]->alt);

/*
        sprintf(latlon_buf, "%2dd %4.2fN %3dd %4.2fW",
          (int)scans[ping]->latitude,
          (scans[ping]->latitude - (int)scans[ping]->latitude) * 60.0,
          (int)scans[ping]->longitude,
          fabs((scans[ping]->longitude - (int)scans[ping]->longitude) * 60.0));
*/

        metersPerPixel = ((double)swath_width / (datasize / data_reducer));

        range = (int)((double)abs(x - 256) * metersPerPixel);
        beamAngle = RAD_TO_DEG * atan((double)range / scans[ping]->alt);

        sprintf(range_buf, "%5d", range);
        sprintf(angle_buf, "%5.1f", beamAngle);

        /*
         *    Figure out the lat and lon of the selected point.
         *    Includes a calculation for the offset from nadir based on
         *    towfish azimuth.
         */

        lat = (int)scans[ping]->latitude;
        lon = (int)scans[ping]->longitude;

        geoutm (lat, lon, sonar->c_lon, &east, &north);

        east += 

        XmTextSetString(telemPing, ping_buf);
        XmTextSetString(telemLatLon, latlon_buf);
        XmTextSetString(telemTime, time_buf);
        XmTextSetString(telemPitch, pitch_buf);
        XmTextSetString(telemRoll, roll_buf);
        XmTextSetString(telemSwath, swath_buf);
        XmTextSetString(telemRange, range_buf);
        XmTextSetString(telemAngle, angle_buf);
        XmTextSetString(telemAlt, alt_buf);
/*
        for(i = 0; i < datasize / 2; i+=data_reducer)
             {
             range = (int)((double)abs(i) * metersPerPixel);
             beamAngle = RAD_TO_DEG * atan(((double)abs(i) * metersPerPixel)
                                                  / scans[ping]->alt);
             fprintf(stdout, "%.3f\t%5d\t%5d\n", beamAngle, range, 
                          sonar_data[(ping * datasize)+ i + datasize/2]);
             }
*/

        XtFree(time_buf);
        XtFree(latlon_buf);
        XtFree(ping_buf);
        XtFree(pitch_buf);
        XtFree(roll_buf);
        XtFree(swath_buf);
        XtFree(range_buf);
        XtFree(angle_buf);
        }
    else
        message_display(drawarea, NODATA);

    return;

}
