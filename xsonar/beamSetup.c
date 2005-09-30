/*
 *    
 *
 *    Text board for selecting number of pings to use in 
 *    the beam pattern correction.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    Oct 14, 1994
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

#include "sonar_struct.h"


void closeBeamSetupDialog(Widget w, XtPointer client_data,XtPointer callback_data);

void BeamPatternSetup(Widget w, XtPointer client_data, 
		      UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    char tempText[20];

    Widget beamSetupDialog;
    Widget beamSetupForm;
    /*Widget beamSetupHolder;*/
    Widget separator1;
    Widget separator2;

    Widget beamSetupRowColumn;
    Widget beamSetupLabel;
    Widget numLinesLabel;
    Widget numLinesText;
    Widget overlapText;
    Widget overlapLabel;
    Widget responseAngleText;
    Widget responseAngleLabel;
    Widget normalizeText;
    Widget normalizeLabel;
    Widget maxBeamText;
    Widget maxBeamLabel;

    Widget buttonRowColumn;
    Widget beamBalanceLabel;
    Widget beamAdjustRowColumn;
    Widget onToggle;
    Widget offToggle;

    Widget limitBeamCorrectionLabel;
    Widget limitBeamRowColumn;
    Widget limitBeamOnToggle;
    Widget limitBeamOffToggle;

    Widget doneButton;
    Widget cancelButton;

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

    /*void closeBeamSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/
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
    beamSetupDialog = XtCreatePopupShell("BeamSetupDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    beamSetupForm = XtCreateWidget("BeamSetupForm", xmFormWidgetClass,
                  beamSetupDialog, args, n);
    XtManageChild(beamSetupForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Setup Options",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    beamSetupLabel = XtCreateWidget("BeamSetupLabel",
                             xmLabelWidgetClass, beamSetupForm, args, n);
    XtManageChild(beamSetupLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, beamSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 5); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    beamSetupRowColumn = XtCreateWidget("BeamSetupRowColumn",
                xmRowColumnWidgetClass, beamSetupForm, args, n);
    XtManageChild(beamSetupRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Number of lines: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    numLinesLabel = XtCreateWidget("NumLinesLabel", xmLabelWidgetClass,
                                     beamSetupRowColumn, args, n);
    XtManageChild(numLinesLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    numLinesText = XtCreateWidget("NumLinesText", xmTextFieldWidgetClass,
                                     beamSetupRowColumn, args, n);
    XtManageChild(numLinesText);
    itoa((int)sonar->beamNumLines, tempText);
    XmTextFieldSetString(numLinesText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ping Overlap: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    overlapLabel = XtCreateWidget("OverlapLabel", xmLabelWidgetClass,
                                     beamSetupRowColumn, args, n);
    XtManageChild(overlapLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    overlapText = XtCreateWidget("OverlapText", xmTextFieldWidgetClass,
                                     beamSetupRowColumn, args, n);
    XtManageChild(overlapText);
    itoa((int)sonar->beamAverageLines, tempText);
    XmTextFieldSetString(overlapText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Max Beam Angle: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    maxBeamLabel = XtCreateWidget("MaxBeamLabel", xmLabelWidgetClass,
                                     beamSetupRowColumn, args, n);
    XtManageChild(maxBeamLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    maxBeamText = XtCreateWidget("MaxBeamText", xmTextFieldWidgetClass,
                                    beamSetupRowColumn, args, n);
    XtManageChild(maxBeamText);
    itoa((int)sonar->maxBeamAngle, tempText);
    XmTextFieldSetString(maxBeamText, tempText);


    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Response Angle: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    responseAngleLabel = XtCreateWidget("ResponseAngleLabel", 
                  xmLabelWidgetClass, beamSetupRowColumn, args, n);
    XtManageChild(responseAngleLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    responseAngleText = XtCreateWidget("ResponseAngleText", 
                  xmTextFieldWidgetClass, beamSetupRowColumn, args, n);
    XtManageChild(responseAngleText);
    itoa((int)sonar->beamResponseAngle, tempText);
    XmTextFieldSetString(responseAngleText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Data Normalization (0-255): ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    normalizeLabel = XtCreateWidget("NormalizeLabel",
                  xmLabelWidgetClass, beamSetupRowColumn, args, n);
    XtManageChild(normalizeLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    normalizeText = XtCreateWidget("NormalizeText",
                  xmTextFieldWidgetClass, beamSetupRowColumn, args, n);
    XtManageChild(normalizeText);
    itoa((int)sonar->normalize, tempText);
    XmTextFieldSetString(normalizeText, tempText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, beamSetupRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, beamSetupForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Port/Stbd Tone Adjust", 
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    beamBalanceLabel = XtCreateWidget("BeamBalanceLabel", xmLabelWidgetClass,
                                     beamSetupForm, args, n);
    XtManageChild(beamBalanceLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, beamBalanceLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNmarginWidth, 60); n++;
    beamAdjustRowColumn = XtCreateWidget("BeamAdjustRowColumn",
                xmRowColumnWidgetClass, beamSetupForm, args, n);
    XtManageChild(beamAdjustRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("On",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    onToggle = XtCreateWidget("OnToggle",
                xmToggleButtonWidgetClass, beamAdjustRowColumn, args, n);
    XtManageChild(onToggle);
    XtAddCallback(onToggle, XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Off",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    offToggle = XtCreateWidget("OffToggle",
                xmToggleButtonWidgetClass, beamAdjustRowColumn, args, n);
    XtManageChild(offToggle);
    XtAddCallback(offToggle,XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    if(sonar->adjustBeam)
        XmToggleButtonSetState(onToggle, True, False);
    else
        XmToggleButtonSetState(offToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, beamAdjustRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Max Beam Correction Range",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    limitBeamCorrectionLabel = XtCreateWidget("LimitBeamCorrectionLabel", xmLabelWidgetClass,
                                     beamSetupForm, args, n);
    XtManageChild(limitBeamCorrectionLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, limitBeamCorrectionLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNmarginWidth, 60); n++;
    limitBeamRowColumn = XtCreateWidget("LimitBeamRowColumn", 
                          xmRowColumnWidgetClass, beamSetupForm, args, n);
    XtManageChild(limitBeamRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("On",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    limitBeamOnToggle = XtCreateWidget("LimitBeamOnToggle",
                xmToggleButtonWidgetClass, limitBeamRowColumn, args, n);
    XtManageChild(limitBeamOnToggle);
    XtAddCallback(limitBeamOnToggle, XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Off",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    limitBeamOffToggle = XtCreateWidget("LimitBeamOffToggle",
                xmToggleButtonWidgetClass, limitBeamRowColumn, args, n);
    XtManageChild(limitBeamOffToggle);
    XtAddCallback(limitBeamOffToggle,XmNvalueChangedCallback,
                                           Get_toggle_values, sonar);

    if(sonar->useBeamLimits)
        XmToggleButtonSetState(limitBeamOnToggle, True, False);
    else
        XmToggleButtonSetState(limitBeamOffToggle, True, False);


    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, limitBeamRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, beamSetupForm, args, n);
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
                xmRowColumnWidgetClass, beamSetupForm, args, n);
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
                                       closeBeamSetupDialog, sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                       closeBeamSetupDialog, sonar);

    XtRealizeWidget(beamSetupDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */


    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(beamSetupDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(beamSetupDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(beamSetupDialog, args, n);


    XtPopup(beamSetupDialog, XtGrabNone);

    doneAnswer = 0;
    cancelAnswer = 0;

    while(doneAnswer == 0 && cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(cancelButton, XmNuserData, &doneAnswer, NULL);
        XtVaGetValues(doneButton, XmNuserData, &cancelAnswer, NULL);
        }


    XtPopdown(beamSetupDialog);
    XtDestroyWidget(beamSetupDialog);

    XtSetSensitive(w, True);

    return;


}

void closeBeamSetupDialog(Widget w, XtPointer client_data,
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
        sonar->beamNumLines =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel, 
                                                        "*NumLinesText")));
        sonar->beamAverageLines =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel, 
                                                        "*OverlapText")));

        sonar->beamResponseAngle =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel, 
                                                        "*ResponseAngleText")));

        sonar->maxBeamAngle =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel,
                                                        "*MaxBeamText")));


        sonar->normalize =
            atoi(XmTextFieldGetString(XtNameToWidget(sonar->toplevel, 
                                                        "*NormalizeText")));
        if(sonar->beamNumLines == 0)
            {
            sprintf(message, "Number of pings for beam correction is 0\n");
            strcat(message, "Please re-enter number of pings to use.");
            messages(sonar, message);
            }
        if(sonar->beamResponseAngle == 0 || sonar->beamResponseAngle == 90)
            {
            sprintf(message, "Response Angle for normalization is %d\n",
                             sonar->beamResponseAngle);
            strcat(message, 
                    "Only angles between 1 and 89 are valid, please re-enter");
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


