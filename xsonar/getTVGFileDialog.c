/*
 *    getTVGFileDialog.c
 *
 *    Callback function for popping up a file selection dialog and retrieving
 *    the name of the file to use for correcting TVG ramping problems.
 *
 */

#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/ToggleB.h> /* XmToggleButtonGetState */

#include "xsonar.h"
#include "sonar_struct.h"

void tvgFileName(Widget w, XtPointer client_data, XtPointer callback_data);

void tvgRampFileSelect(Widget w, XtPointer client_data, UNUSED XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    /*XmAnyCallbackStruct *callback = (XmAnyCallbackStruct *) callback_data;*/

    Arg args[20];
    Cardinal n;

    Widget tvgFileBox;
    Widget getTVGFileDialog;

    /*XEvent event;*/

    char *stringPtr;
    int answer;


    /*
     *  If turning off using a text model for removing the TVG ramp
     *  return;
     */

    if(!XmToggleButtonGetState(w))
        {
        sonar->getCurveModelText = 0;
        printf("ramp text flag is %d\n", sonar->getCurveModelText);
        return;
        }

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    getTVGFileDialog = XtCreatePopupShell("GetTVGFileDialog",
                xmDialogShellWidgetClass, sonar->toplevel, args, n);

    stringPtr = strrchr(sonar->currentDirectory, '/');
    stringPtr++;
    if(*stringPtr != '*')
        strcat(sonar->currentDirectory, "*");
    n = 0;

    XtSetArg(args[n], XmNdirMask, XmStringCreateLtoR(sonar->currentDirectory, 
                           XmSTRING_DEFAULT_CHARSET)); n++;
    tvgFileBox = XtCreateWidget("TvgFileBox", xmFileSelectionBoxWidgetClass,
                getTVGFileDialog, args, n);

    XtAddCallback(tvgFileBox, XmNokCallback, tvgFileName, sonar);
    XtAddCallback(tvgFileBox, XmNcancelCallback, tvgFileName, sonar);
    XtManageChild(tvgFileBox);

    answer = 0;

    while(answer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(tvgFileBox, XmNuserData, &answer, NULL);
        }

    if(answer == 1)
        XmToggleButtonSetState(w, False, False);

    XtUnmanageChild(tvgFileBox);
    XtDestroyWidget(getTVGFileDialog);

    return;

}



void tvgFileName(Widget w, XtPointer client_data, XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    XmFileSelectionBoxCallbackStruct *file_callback =
                (XmFileSelectionBoxCallbackStruct *) callback_data;

    /*XmString fileName; */
    Widget parent_widget;
    /*Arg args[10];*/
    /*Cardinal n;*/

    int answer;

    char *tempFilePtr;

    /*void messages();*/

    parent_widget = w;

    if(file_callback->reason == XmCR_CANCEL)
        {
        sonar->getCurveModelText = 0;
        answer = 1;
        XtVaSetValues(XtNameToWidget(sonar->toplevel,
             "*TvgFileBox"), XmNuserData, answer, NULL);
        sonar->getCurveModelText = 0;
        printf("ramp text flag is %d\n", sonar->getCurveModelText);
        return;
        }

    XmStringGetLtoR(file_callback->value,
            XmSTRING_DEFAULT_CHARSET, &tempFilePtr);
	
    strcpy(sonar->tvgRampTextFileName, tempFilePtr);
    sonar->getCurveModelText = 1;
    printf("ramp text flag is %d\n", sonar->getCurveModelText);
    printf("ramp file is %s\n", sonar->tvgRampTextFileName);

    answer = 2;
    XtVaSetValues(XtNameToWidget(sonar->toplevel,
         "*TvgFileBox"), XmNuserData, answer, NULL);

    printf("here\n");

    return;
}

