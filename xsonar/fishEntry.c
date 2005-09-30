/*
 *    setupFishNav()
 *
 *    Text board for inputting layback and depth of fish.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    June 11, 1995
 */


#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/DialogS.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>

#include "xsonar.h"

#include     "sonar_struct.h"

void closeLaybackDialog(Widget w, XtPointer client_data, XtPointer callback_data);


void setupFishNav(Widget w, XtPointer client_data, UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    /*char tempText[20];*/

    Widget laybackDialog;
    Widget laybackForm;
    /*Widget laybackHolder;*/
    Widget separator1;

    Widget laybackRowColumn;

    Widget topLabel;
    Widget laybackLabel1;
    Widget laybackText1;
    Widget laybackLabel2;
    Widget laybackText2;
    Widget laybackLabel3;
    Widget laybackText3;
    Widget depthLabel1;
    Widget depthText1;
    Widget depthLabel2;
    Widget depthText2;
    Widget depthLabel3;
    Widget depthText3;
    Widget useEnteredToggle;
    Widget readHeaderToggle;

    Widget buttonRowColumn;
    Widget laybackDoneButton;
    Widget laybackCancelButton;

    Arg args[40];
    /*Status status;*/
    Cardinal n;

    int doneAnswer;
    int cancelAnswer;
    /*int cancelCheck;*/

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*char warningmessage[255];*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog.
     */

    XtSetSensitive(w, FALSE);
    XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                "*NavDialog*ExitButton"), FALSE);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    laybackDialog = XtCreatePopupShell("LaybackDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    laybackForm = XtCreateWidget("LaybackForm", xmFormWidgetClass,
                  laybackDialog, args, n);
    XtManageChild(laybackForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Fish layback and depth entry",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    topLabel = XtCreateWidget("TopLabel",
                             xmLabelWidgetClass, laybackForm, args, n);
    XtManageChild(topLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, topLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 7); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNmarginWidth, 30); n++;
    laybackRowColumn = XtCreateWidget("LaybackRowColumn",
                xmRowColumnWidgetClass, laybackForm, args, n);
    XtManageChild(laybackRowColumn);

    /*
     *   Text and label widgets for initial, midpoint and endpoint
     *   layback and fish depths.
     */

    /*
     *   Initial
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Starting Layback: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackLabel1 = XtCreateWidget("LaybackLabel1", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackLabel1);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackText1 = XtCreateWidget("LaybackText1", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackText1);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Starting Depth: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthLabel1 = XtCreateWidget("DepthLabel1", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthLabel1);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthText1 = XtCreateWidget("DepthText1", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthText1);

    /*
     *   Midpoint
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Mid-point Layback: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackLabel2 = XtCreateWidget("LaybackLabel2", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackLabel2);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackText2 = XtCreateWidget("LaybackText2", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackText2);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Mid-point Depth: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthLabel2 = XtCreateWidget("DepthLabel2", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthLabel2);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthText2 = XtCreateWidget("DepthText2", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthText2);

    /*
     *   End-point
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ending Layback: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackLabel3 = XtCreateWidget("LaybackLabel3", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackLabel3);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    laybackText3 = XtCreateWidget("LaybackText3", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(laybackText3);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ending Depth: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthLabel3 = XtCreateWidget("DepthLabel3", xmLabelWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthLabel3);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    depthText3 = XtCreateWidget("DepthText3", xmTextWidgetClass,
                                     laybackRowColumn, args, n);
    XtManageChild(depthText3);

    /*
     *   Radio box for selecting values or reading the header
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Enter Values",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    useEnteredToggle = XtCreateWidget("UseEnteredToggle",
                xmToggleButtonWidgetClass, laybackRowColumn, args, n);
    XtManageChild(useEnteredToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Read Header",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    readHeaderToggle = XtCreateWidget("ReadHeaderToggle",
                xmToggleButtonWidgetClass, laybackRowColumn, args, n);
    XtManageChild(readHeaderToggle);


    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, laybackRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, laybackForm, args, n);
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
                xmRowColumnWidgetClass, laybackForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Apply      ", XmSTRING_DEFAULT_CHARSET)); n++;
    laybackDoneButton = XtCreateWidget("LaybackDoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(laybackDoneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("     Cancel     ", XmSTRING_DEFAULT_CHARSET)); n++;
    laybackCancelButton = XtCreateWidget("LaybackCancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(laybackCancelButton);

    XtAddCallback(laybackDoneButton, XmNactivateCallback, 
                                         closeLaybackDialog, sonar);
    XtAddCallback(laybackCancelButton, XmNactivateCallback, 
                                         closeLaybackDialog, sonar);

    XtRealizeWidget(laybackDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(laybackDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(laybackDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(laybackDialog, args, n);

    XtPopup(laybackDialog, XtGrabNone);

    doneAnswer = 0;
    cancelAnswer = 0;

    while(doneAnswer == 0 && cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(laybackDoneButton, XmNuserData, &doneAnswer, NULL);
        XtVaGetValues(laybackCancelButton, XmNuserData, &cancelAnswer, NULL);
        }


    XtPopdown(laybackDialog);
    XtDestroyWidget(laybackDialog);

    XtSetSensitive(w, True);
    XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                "*NavDialog*ExitButton"), True);

    return;

}

/*void closeLaybackDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeLaybackDialog(Widget w, XtPointer client_data, 
			UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget parentWidget;
 
    int answer;
    int buffer_len;
    int fishReturn;
    int readHeader;

    char message[300];
    char callingWidget[100]; 
    char buffer[255];
    char *string_ptr; 

    double depth[3], slantRange[3];

    /*
     *   Only do this if the cancel button is selected: set answer
     *   to 1 and let the user cancel the operation.
     */

    strcpy(callingWidget, XtName(w));

    XtVaGetValues(w, XmNuserData, &answer, NULL);

    if(!strcmp(callingWidget, "LaybackCancelButton"))
        {
        answer = 1;
        XtVaSetValues(w, XmNuserData, answer, NULL);
        return;
        }
    else
        {

        XtSetSensitive(w, False);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                          "*LaybackDialog*LaybackCancelButton"), False);

        string_ptr = sonar->infile;
        strcpy(buffer, string_ptr);
        buffer_len = (int) strlen(buffer);
        while(*(buffer + --buffer_len) != '.')
            strncpy((buffer + buffer_len),"\0",1);


        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel,
                                          "*LaybackDialog*ReadHeaderToggle")))
            readHeader = True;
        else
            readHeader = False;


        depth[0] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*DepthText1")));
        slantRange[0] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*LaybackText1")));
        depth[1] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*DepthText2")));
        slantRange[1] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*LaybackText2")));
        depth[2] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*DepthText3")));
        slantRange[2] = atof(XmTextGetString(
                            XtNameToWidget(sonar->toplevel, "*LaybackText3")));

        parentWidget = XtNameToWidget(sonar->toplevel, "*LaybackDialog");

        fishReturn = fishNav(parentWidget, sonar, depth, 
                                             slantRange, readHeader);

        if(fishReturn)
            {
            sprintf(message, 
                "Layback routine finished.  Re-wrote the file:\n\n%s.nav\n\n", 
                 buffer);
            strcat(buffer, "nav.orig");
            strcat(message, "The original nav file is saved as ");
            strcat(message, buffer);

            messages(sonar, message);
            }
        else
            {
            sprintf(message, "Layback routine cancelled.");
            messages(sonar, message);
            }

        answer = 1;
        XtVaSetValues(w, XmNuserData, answer, NULL);

        XtSetSensitive(w, True);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                          "*LaybackDialog*LaybackCancelButton"), True);
        }

    return;
}


