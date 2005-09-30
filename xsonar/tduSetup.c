
/*
 *    tduSpacing.c
 *
 *    Text board for selecting number of pixels used to map sonar in
 *    the north-south direction (per cm) for the Raytheon TDU-850.  The 850 
 *    has a constant 203 pixels/cm across the print head, but will vary in the
 *    paper feed direction depending on the amount of paper, current supply,
 *    and general overall operating conditions.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    January 17, 1995
 */




#include     "sonar_struct.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>

#include "xsonar.h"

void closeTduSetup(Widget w, XtPointer client_data, XtPointer callback_data);


void tduSetup(Widget w, XtPointer client_data, UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    char tempText[20];

    Widget tduSetupDialog;
    Widget tduSetupForm;
    /*Widget tduSetupHolder;*/
    Widget separator1;
    Widget separator2;

    Widget tduSetupRowColumn;
    Widget tduSetupLabel;
    Widget numPixelsLabel;
    Widget numPixelsText;

    Widget buttonRowColumn;
    Widget doneButton;
    Widget cancelButton;

    Widget hardReturnRowColumn;
    Widget hardReturnLabel;
    Widget blackToggle;
    Widget whiteToggle;

    Arg args[10];
    /*Status status;*/
    Cardinal n;

    int doneAnswer;
    int cancelAnswer;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*void closeTduSetup(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/
    /*void setHardReturn();*/             /*  In utilities.c  */


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
    tduSetupDialog = XtCreatePopupShell("TduSetupDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    tduSetupForm = XtCreateWidget("TduSetupForm", xmFormWidgetClass,
                  tduSetupDialog, args, n);
    XtManageChild(tduSetupForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Printer Setup",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tduSetupLabel = XtCreateWidget("TduSetupLabel",
                             xmLabelWidgetClass, tduSetupForm, args, n);
    XtManageChild(tduSetupLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, tduSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, tduSetupForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, tduSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    tduSetupRowColumn = XtCreateWidget("TduSetupRowColumn",
                xmRowColumnWidgetClass, tduSetupForm, args, n);
    XtManageChild(tduSetupRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Number of pixels / inch: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 40); n++;
    numPixelsLabel = XtCreateWidget("NumPixelsLabel", xmLabelWidgetClass,
                                     tduSetupRowColumn, args, n);
    XtManageChild(numPixelsLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    numPixelsText = XtCreateWidget("NumPixelsText", xmTextFieldWidgetClass,
                                     tduSetupRowColumn, args, n);
    XtManageChild(numPixelsText);
    sprintf(tempText, "%.1f", sonar->tduPixelsNS);
    XmTextFieldSetString(numPixelsText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Hard Return: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    hardReturnLabel = XtCreateWidget("HardReturnLabel",
                             xmLabelWidgetClass, tduSetupRowColumn, args, n);
    XtManageChild(hardReturnLabel);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
/*
    XtSetArg(args[n], XmNmarginWidth, 30); n++;
*/
    hardReturnRowColumn = XtCreateWidget("HardReturnRowColumn",
                xmRowColumnWidgetClass, tduSetupRowColumn, args, n);
    XtManageChild(hardReturnRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Black",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    blackToggle = XtCreateWidget("BlackToggle",
                xmToggleButtonWidgetClass, hardReturnRowColumn, args, n);
    XtManageChild(blackToggle);
    XtAddCallback(blackToggle, XmNvalueChangedCallback,
                                           setHardReturn, sonar);
    if(sonar->setReturnBlack)
            XmToggleButtonSetState(blackToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("White",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    whiteToggle = XtCreateWidget("WhiteToggle",
                xmToggleButtonWidgetClass, hardReturnRowColumn, args, n);
    XtManageChild(whiteToggle);
    XtAddCallback(whiteToggle,XmNvalueChangedCallback,
                                           setHardReturn, sonar);
    if(sonar->setReturnWhite)
            XmToggleButtonSetState(whiteToggle, True, False);


    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, tduSetupRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, tduSetupForm, args, n);
    XtManageChild(separator2);

    /*
     *   Create a menu for cancelling or ok'ing the selection.
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
                xmRowColumnWidgetClass, tduSetupForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Done      ", XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("     Cancel     ", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(cancelButton);

    XtAddCallback(doneButton, XmNactivateCallback, 
		  closeTduSetup, sonar);

    XtAddCallback(cancelButton, XmNactivateCallback, 
		  closeTduSetup, sonar);

    XtRealizeWidget(tduSetupDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(tduSetupDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(tduSetupDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(tduSetupDialog, args, n);

    XtPopup(tduSetupDialog, XtGrabNone);

    doneAnswer = 0;
    cancelAnswer = 0;

    while(doneAnswer == 0 && cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(doneButton, XmNuserData, &doneAnswer, NULL);
        XtVaGetValues(cancelButton, XmNuserData, &cancelAnswer, NULL);
        }

    XtPopdown(tduSetupDialog);
    XtDestroyWidget(tduSetupDialog);

    XtSetSensitive(w, True);

    return;

}

/*void closeTduSetup(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeTduSetup(Widget w, XtPointer client_data, UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    int answer;
    char callingWidget[50];
    char message[100];
    /*void messages();*/


    strcpy(callingWidget, XtName(w));

    /*
     *   Only set this if the done button is selected, otherwise set answer
     *   to 1 and let the user cancel the operation.
     */

    if(!strcmp(callingWidget, "DoneButton"))
        {
        sonar->tduPixelsNS = 
             atof(XmTextFieldGetString(XtNameToWidget(sonar->toplevel,
                                                            "*NumPixelsText")));

        if(sonar->tduPixelsNS == 0)
            {
            sprintf(message, "Number of pixels for printer correction is 0\n");
            strcat(message, "Please re-enter number of pixels to use.");
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


