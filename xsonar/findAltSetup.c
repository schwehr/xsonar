/*
 *    
 *
 *    Text board for selecting options to auto trace the first
 *    sonar return for the slant range and beam pattern corrections.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    Sept 4, 2001
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>

#include <map.h>
#include "xsonar.h"

#include     "sonar_struct.h"

void closeFindAltSetupDialog(Widget w, XtPointer client_data, XtPointer callback_data);


void FindAltSetup(Widget w, XtPointer client_data, UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    char tempText[20];

    Widget findAltSetupDialog;
    Widget findAltSetupForm;
    /*Widget findAltSetupHolder;*/
    Widget separator1;
    Widget separator2;

    Widget findAltSetupRowColumn;
    Widget findAltSetupLabel;
    Widget altThresholdLabel;
    Widget altThresholdText;

    Widget buttonRowColumn;
    Widget doneButton;
    Widget cancelButton;
    Widget findAltToggleLabel;
    Widget findAltToggleRowColumn;
    Widget findAltOnToggle;
    Widget findAltOffToggle;

    Arg args[30];
    /*Status status;*/
    Cardinal n;

    /*int cancelCheck;*/
    int doneAnswer;
    int cancelAnswer;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*void closeFindAltSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/
    /*void Get_toggle_values();*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the filter kernel size, and a push button
     *    row-column to apply or cancel the filtering function.

     */

    XtSetSensitive(w, FALSE);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    findAltSetupDialog = XtCreatePopupShell("FindAltSetupDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    findAltSetupForm = XtCreateWidget("FindAltSetupForm", xmFormWidgetClass,
                  findAltSetupDialog, args, n);
    XtManageChild(findAltSetupForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Options For Auto Tracing First Return",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    findAltSetupLabel = XtCreateWidget("FindAltSetupLabel",
                             xmLabelWidgetClass, findAltSetupForm, args, n);
    XtManageChild(findAltSetupLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, findAltSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 5); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    findAltSetupRowColumn = XtCreateWidget("FindAltSetupRowColumn",
                xmRowColumnWidgetClass, findAltSetupForm, args, n);
    XtManageChild(findAltSetupRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Altitude Search Strength: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    altThresholdLabel = XtCreateWidget("AltThresholdLabel", xmLabelWidgetClass,
                                     findAltSetupRowColumn, args, n);
    XtManageChild(altThresholdLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    altThresholdText = XtCreateWidget("AltThresholdText", 
                  xmTextFieldWidgetClass, findAltSetupRowColumn, args, n);
    XtManageChild(altThresholdText);
    itoa((int)sonar->findAltThreshold, tempText);
    XmTextFieldSetString(altThresholdText, tempText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, findAltSetupRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, findAltSetupForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Auto Trace First Return", 
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    findAltToggleLabel = XtCreateWidget("FindAltToggleLabel", xmLabelWidgetClass,
                                     findAltSetupForm, args, n);
    XtManageChild(findAltToggleLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, findAltToggleLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNmarginWidth, 60); n++;
    findAltToggleRowColumn = XtCreateWidget("FindAltToggleRowColumn",
                xmRowColumnWidgetClass, findAltSetupForm, args, n);
    XtManageChild(findAltToggleRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("On",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    findAltOnToggle = XtCreateWidget("FindAltOnToggle",
                xmToggleButtonWidgetClass, findAltToggleRowColumn, args, n);
    XtManageChild(findAltOnToggle);
    XtAddCallback(findAltOnToggle, XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Off",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    findAltOffToggle = XtCreateWidget("FindAltOffToggle",
                xmToggleButtonWidgetClass, findAltToggleRowColumn, args, n);
    XtManageChild(findAltOffToggle);
    XtAddCallback(findAltOffToggle,XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    if(sonar->findAltFlag)
        XmToggleButtonSetState(findAltOnToggle, True, False);
    else
        XmToggleButtonSetState(findAltOffToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, findAltToggleRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, findAltSetupForm, args, n);
    XtManageChild(separator2);

    /*
     *   Create a menu for cancelling or ok'ing the filter selection.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, findAltSetupForm, args, n);
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
                                       closeFindAltSetupDialog, sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                       closeFindAltSetupDialog, sonar);

    XtRealizeWidget(findAltSetupDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */


    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(findAltSetupDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(findAltSetupDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(findAltSetupDialog, args, n);


    XtPopup(findAltSetupDialog, XtGrabNone);

    doneAnswer = 0;
    cancelAnswer = 0;

    while(doneAnswer == 0 && cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(cancelButton, XmNuserData, &doneAnswer, NULL);
        XtVaGetValues(doneButton, XmNuserData, &cancelAnswer, NULL);
        }


    XtPopdown(findAltSetupDialog);
    XtDestroyWidget(findAltSetupDialog);

    XtSetSensitive(w, True);

    return;


}

/*void closeFindAltSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeFindAltSetupDialog(Widget w, XtPointer client_data, 
			     UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    int answer;
    char callingWidget[50];
    char message[100];
    /*void messages();*/

    strcpy(callingWidget, XtName(w));

    if(!strcmp(callingWidget, "DoneButton"))
        {
        sonar->findAltThreshold =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel, 
                                                        "*AltThresholdText")));
        if(sonar->findAltThreshold == 0)
            {
            sprintf(message, "Number of pings for findAlt correction is 0\n");
            strcat(message, "Please re-enter number of pings to use.");
            messages(sonar, message);
            }
        else
            {
            answer = 1;
            XtVaSetValues(w, XmNuserData, answer, NULL);
            }
        }

    if(!strcmp(callingWidget, "CancelButton"))
        {
        answer = 1;
        XtVaSetValues(w, XmNuserData, answer, NULL);
        }

    return;
}


