/*
 *    Message display function.  Will display messages
 *    in a pop up shell based on what is passed to it.
 */

#include <Xm/MessageB.h>
#include <Xm/DialogS.h>

#include "xsonar.h"

#include "sonar_struct.h"

void close_message_display(Widget w, XtPointer client_data, XtPointer call_data);

void messages(MainSonar *sonar, char *text)
{
    Arg args[20];

    int n;
    int answer;

    Widget messageDialog;
    Widget messageBox;
    Widget messageButton;


    answer = 0;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    messageDialog = XtCreatePopupShell("MessageDialog",
            xmDialogShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNmessageString,
        XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING); n++;
    messageBox = XtCreateWidget("MessageBox",
        xmMessageBoxWidgetClass, messageDialog,
        args, n);

    messageButton = XmMessageBoxGetChild(messageBox,
                XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(messageButton);

    messageButton = XmMessageBoxGetChild(messageBox,
                XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(messageButton);

    /* FIX: close_message_display has wrong type?? */
    XtAddCallback(messageBox, XmNokCallback,
                               close_message_display, &answer);

    XtManageChild(messageBox);
    XFlush(XtDisplay(sonar->toplevel));
    XmUpdateDisplay(sonar->toplevel);

    while(answer == 0)
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

    return;

}

/* FIX: verify that this function does what it is supposed to do.  And what is that? */
void close_message_display(UNUSED Widget w, 
			   XtPointer client_data, XtPointer call_data)
{
    int *answer = (int *) client_data;
    const int reason = ((XmAnyCallbackStruct*)call_data)->reason;

    /*switch(call_data->reason)*/
    switch(reason)
        {
        case XmCR_OK:
            *answer = 1;
            break;
        }

    return;
}
