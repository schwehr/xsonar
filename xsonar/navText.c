/*
 *    Scrollable text function.
 */

#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>

#include "xsonar.h"

#include "sonar_struct.h"

void closeNavText(Widget w, XtPointer client_data, XtPointer call_data);

int createNavText(Widget callingWidget, char *navString, MainSonar *sonar,
		  UNUSED  int numNodes)
{


    Arg args[20];
    Cardinal n;

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    Widget navTextDialog;
    Widget navTextForm;
    Widget navTextFrame;
    /*Widget navTextMenuBar;*/
    /*Widget navTextPullDownMenu;*/
    Widget navText;
    /*Widget navTextCloseButton;*/
    /*Widget popDownButton;*/
    /*Widget playBackButton;*/
    /*Widget textWidget;*/
    Widget doneButton;
    Widget cancelButton;
    Widget cancelLabel;
    Widget separator;
    Widget buttonRowColumn;


    /*int i, j;*/
    int answer;
    /*int asciiIndex;*/

    char *tempString;

    /*
     *    Create the top level shell to hold the scrolled 
     *    text widget.
     */

    n = 0;
    if(callingWidget == XtNameToWidget(sonar->toplevel, "*LaybackDialog"))
        XtSetArg(args[n], XmNtitle, "Edit Navigation Nodes");
    else
        XtSetArg(args[n], XmNtitle, "Duplicate ASCII Navigation Nodes"); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    navTextDialog = XtCreatePopupShell("NavTextDialog",
            transientShellWidgetClass, sonar->toplevel, args, n);

    /*
     *    Create a form to hold the widgets.
     */

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    navTextForm = XtCreateWidget("NavTextForm",
                    xmFormWidgetClass, navTextDialog, args, n);
    XtManageChild(navTextForm);

    /*
     *    Now create the rest of the widgets.
     */

    /*
     *    Create a frame to holder the scrollable text for looks,
     *    and to separate it from the menubar.
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    navTextFrame = XtCreateWidget("NavTextFrame",
                   xmFrameWidgetClass, navTextForm, args, n);
    XtManageChild(navTextFrame);

    n = 0;
    XtSetArg(args[n], XmNrows,              20); n++;
    XtSetArg(args[n], XmNcolumns,           80); n++;
    XtSetArg(args[n], XmNeditMode,          XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNeditable,          True);
    XtSetArg(args[n], XmNcursorPositionVisible, True); n++;
    XtSetArg(args[n], XmNscrollHorizontal,  False); n++;
    navText = XmCreateScrolledText(navTextFrame, "NavText",args, n);
    XtManageChild(navText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, navTextFrame); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator = XtCreateWidget("Separator",
                   xmSeparatorWidgetClass, navTextForm, args, n);
    XtManageChild(separator);

    n = 0;
    if(callingWidget == XtNameToWidget(sonar->toplevel, "*LaybackDialog"))
        XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Click apply to use the above values",
         XmSTRING_DEFAULT_CHARSET));
    else
        XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Delete the flagged selections from the nav file?",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    cancelLabel = XtCreateWidget("CancelLabel",
                             xmLabelWidgetClass, navTextForm, args, n);
    XtManageChild(cancelLabel);

    /*
     *   Create a menu for cancelling or ok'ing the selection.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, cancelLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, navTextForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    if(callingWidget == XtNameToWidget(sonar->toplevel, "*LaybackDialog"))
        XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("    Apply     ", XmSTRING_DEFAULT_CHARSET));
    else
        XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Ok      ", XmSTRING_DEFAULT_CHARSET)); n++;
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
                                                closeNavText,sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                                closeNavText,sonar);

    XmTextSetString(navText, navString);

    XtRealizeWidget(navTextDialog);

    if(sonar->colormap != (int) NULL)
        {
        main_window_attr.colormap = sonar->colormap;
        main_window_valuemask = CWColormap;

        XChangeWindowAttributes(XtDisplay(navTextDialog), 
                   XtWindow(navTextDialog),
                   main_window_valuemask, &main_window_attr);
        }

    XtPopup(navTextDialog, XtGrabNone);

    answer = 0;

    XtVaSetValues(doneButton, XmNuserData, &answer, NULL);
    XtVaSetValues(cancelButton, XmNuserData, &answer, NULL);

    while(answer == 0)
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

    if(answer == 1)
        {
        tempString = XmTextGetString(navText);
        sprintf(navString, "%s", tempString);
        XtFree(tempString);
        }


    XtPopdown(navTextDialog);
    XtDestroyWidget(navTextDialog);

    return(answer);
}

void closeNavText(Widget w, UNUSED  XtPointer client_data,
		  UNUSED  XtPointer call_data)
{

  /*MainSonar *sonar = (MainSonar *) client_data;*/

    int *answer;

    char callingWidget[100];

    XtVaGetValues(w, XmNuserData, &answer, NULL);

    strcpy(callingWidget, XtName(w));

    /*
     *   Only set this if the done button is selected, otherwise set answer
     *   to 1 and let the user cancel the operation.
     */

    if(!strcmp(callingWidget, "DoneButton"))
        *answer = 1;

    if(!strcmp(callingWidget, "CancelButton"))
        *answer = 2;

    return;

}
