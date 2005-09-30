/*
 *    
 *
 *    Form for selecting ellipsoid to use in mapping functions.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    July 9, 1995
 */

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include "xsonar.h"

#include     "sonar_struct.h"

void closeEllipsoidSetupDialog(Widget w, XtPointer client_data, XtPointer callback_data);

void ellipsoidSetup(Widget w, XtPointer client_data, UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    /*char tempText[20];*/

    Widget ellipsoidSetupDialog;
    Widget ellipsoidSetupForm;
    Widget separator1;

    Widget ellipsoidSetupRowColumn;
    Widget ellipsoidSetupLabel;

    Widget buttonRowColumn;
    Widget doneButton;
    Widget cancelButton;
    Widget parent;

    Widget wgs84Toggle;
    Widget clarkeToggle;

    Arg args[10];
    /*Status status;*/
    Cardinal n;

    int answer;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*void closeEllipsoidSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the filter kernel size, and a push button
     *    row-column to apply or cancel the filtering function.
     */

    XtSetSensitive(w, FALSE);

    parent = XtNameToWidget(sonar->toplevel, "*NavDialog");

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNtitle, "Ellipsoid Setup"); n++;
    ellipsoidSetupDialog = XtCreatePopupShell("EllipsoidSetupDialog",
               transientShellWidgetClass, parent, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    ellipsoidSetupForm = XtCreateWidget("EllipsoidSetupForm", xmFormWidgetClass,
                  ellipsoidSetupDialog, args, n);
    XtManageChild(ellipsoidSetupForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Available Ellipsoids:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    ellipsoidSetupLabel = XtCreateWidget("EllipsoidSetupLabel",
                             xmLabelWidgetClass, ellipsoidSetupForm, args, n);
    XtManageChild(ellipsoidSetupLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, ellipsoidSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    ellipsoidSetupRowColumn = XtCreateWidget("EllipsoidSetupRowColumn",
                xmRowColumnWidgetClass, ellipsoidSetupForm, args, n);
    XtManageChild(ellipsoidSetupRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("WGS-84",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    wgs84Toggle = XtCreateWidget("Wgs84Toggle",
                xmToggleButtonWidgetClass, ellipsoidSetupRowColumn, args, n);
    XtManageChild(wgs84Toggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Clarke 1866",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    clarkeToggle = XtCreateWidget("ClarkeToggle",
                xmToggleButtonWidgetClass, ellipsoidSetupRowColumn, args, n);
    XtManageChild(clarkeToggle);

    if(sonar->ellipsoid == WGS84)
        XmToggleButtonSetState(wgs84Toggle, True, False);

    if(sonar->ellipsoid == CLARKE1866)
        XmToggleButtonSetState(clarkeToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, ellipsoidSetupRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, ellipsoidSetupForm, args, n);
    XtManageChild(separator1);

    /*
     *   Create a menu for cancelling or ok'ing the filter selection.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, ellipsoidSetupForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("    Done    ", XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("   Cancel   ", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(cancelButton);

    XtAddCallback(doneButton, XmNactivateCallback, 
                                  closeEllipsoidSetupDialog, sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                  closeEllipsoidSetupDialog, sonar);

    XtRealizeWidget(ellipsoidSetupDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(parent, XmNx, &shellx, NULL);
    XtVaGetValues(parent, XmNy, &shelly, NULL);
    XtVaGetValues(parent, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(parent, XmNheight, &shellHeight, NULL);
    XtVaGetValues(ellipsoidSetupDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(ellipsoidSetupDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(ellipsoidSetupDialog, args, n);

    XtPopup(ellipsoidSetupDialog, XtGrabNone);

    answer = 0;

    XtVaSetValues(doneButton, XmNuserData, &answer, NULL);
    XtVaSetValues(cancelButton, XmNuserData, &answer, NULL);

    while(answer == 0)
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

    XtPopdown(ellipsoidSetupDialog);
    XtDestroyWidget(ellipsoidSetupDialog);

    XtSetSensitive(w, True);

    return;

}

/*void closeEllipsoidSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeEllipsoidSetupDialog(Widget w, XtPointer client_data, 
			       UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    int *answer;

    XtVaGetValues(w, XmNuserData, &answer, NULL);

    if(!strcmp(XtName(w), "DoneButton"))
        {
        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                                         "*Wgs84Toggle")))
            sonar->ellipsoid = WGS84;

        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                                         "*ClarkeToggle")))
            sonar->ellipsoid = CLARKE1866;

        }
        
    *answer = 1;

    return;
}


