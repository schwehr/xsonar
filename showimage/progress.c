/*
 *    Create a progress monitor for data processing or data reads
 *    to inform the user that something is happening while the application
 *    is busy.
 */
#include <assert.h>

#include "showimage2.h"
#include "externals.h"

void showProgress(struct draw *drawarea, Widget w, char *messageBuffer)
/*struct draw *drawarea;
Widget w;
char *messageBuffer;*/
{

    Widget statusDialog;
    Widget statusForm;
    Widget statusMessage;
    Widget progressFrame;
    Widget drawProgress;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    Boolean checkingEvents;

    /*XColor grayScale[256];*/
    /*Colormap defaultColormap;*/
    /*int i;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    XEvent event;

    Arg args[40];
    Cardinal n;

    assert(NULL != messageBuffer);
    assert(NULL != drawarea);
    assert(NULL != w);

    XmUpdateDisplay(drawarea->shell);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    statusDialog = XtCreatePopupShell("StatusDialog",
              transientShellWidgetClass, w, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    statusForm = XtCreateWidget("StatusForm", xmFormWidgetClass,
                  statusDialog, args, n);
    XtManageChild(statusForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(messageBuffer,
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    statusMessage = XtCreateWidget("StatusMessage", xmLabelWidgetClass, 
                                     statusForm, args, n);
    XtManageChild(statusMessage);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, statusMessage); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    progressFrame = XtCreateWidget("ProgressFrame",
                xmFrameWidgetClass, statusForm, args, n);
    XtManageChild(progressFrame);

    n = 0;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    drawProgress = XtCreateWidget("DrawProgress", xmDrawingAreaWidgetClass,
                    progressFrame, args, n);
    XtManageChild(drawProgress);

    XtRealizeWidget(statusDialog);


    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(w, XmNx, &shellx, NULL);
    XtVaGetValues(w, XmNy, &shelly, NULL);
    XtVaGetValues(w, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(w, XmNheight, &shellHeight, NULL);
    XtVaGetValues(statusDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(statusDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(statusDialog, args, n);

    /*
     *    Create a colormap entry for this shell to match the grayscale
     *    colormap of the main shell.  This will keep the display from 
     *    blinking if the lower entries of the colormap are filled by 
     *    another application.
     */


    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(statusDialog),
                                      main_window_valuemask, &main_window_attr);


    XtPopup(statusDialog, XtGrabNone);

    /*
     *    Check events coming in from the server looking for an Expose event.
     *    Expose will be the last event when this dialog, specifically the
     *    draw window, is mapped to the screen.  This will ensure that
     *    this dialog is popped up and viewable before drawing into it.
     *    
     */

    checkingEvents = True;
    while(checkingEvents)
        {
        if(XCheckWindowEvent(XtDisplay(drawarea->shell), XtWindow(drawProgress),
            ExposureMask, &event))
            {
            XtDispatchEvent(&event);
            checkingEvents = False;
            }
/*
        if(event.type == Expose && event.xexpose.window == XtWindow(drawProgress))
*/
        }

    return;

}
