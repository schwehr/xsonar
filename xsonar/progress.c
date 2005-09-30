/*
 *    Create a progress monitor for data processing or data reads
 *    to inform the user that something is happening while the application
 *    is busy.
 */

#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>

#include "xsonar.h"

#include "sonar_struct.h"

void showProgress(MainSonar *sonar, Widget w, char *messageBuffer);

Widget setUpProgressWindow(MainSonar *sonar, char *dialog, char *messageBuffer, Dimension *width, Dimension *height, GC *gc)
{

    Widget callingDialog;
    Widget progressWindow;


    callingDialog = XtNameToWidget(sonar->toplevel, dialog);
    showProgress(sonar, callingDialog, messageBuffer);

    progressWindow = XtNameToWidget(sonar->toplevel,
                                      "*StatusDialog*DrawProgress");

    XtVaGetValues(progressWindow, XmNwidth, width, NULL);
    XtVaGetValues(progressWindow, XmNheight, height, NULL);

    *gc = XCreateGC(XtDisplay(sonar->toplevel),
                              XtWindow(progressWindow),None,NULL);

    XSetForeground(XtDisplay(sonar->toplevel), *gc,
           WhitePixelOfScreen(XtScreen(sonar->toplevel)));

    XSetBackground(XtDisplay(sonar->toplevel), *gc,
           BlackPixelOfScreen(XtScreen(sonar->toplevel)));

    XmUpdateDisplay(sonar->toplevel);

    return(progressWindow);

}

void showProgress(MainSonar *sonar, Widget w, char *messageBuffer)
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

    /*XSetWindowAttributes main_window_attr;*/  /* window attribute struct */
    /*unsigned long main_window_valuemask;*/    /* mask for attribute values */

    XEvent event;

    Arg args[30];
    Cardinal n;

    /*int i;*/

    n = 0;
    XtSetArg(args[n], XmNtitle, "XSonar Monitor"); n++;
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
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    statusMessage = XtCreateWidget("StatusMessage", xmLabelWidgetClass, 
                                     statusForm, args, n);
    XtManageChild(statusMessage);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, statusMessage); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNborderWidth, 5); n++;
    XtSetArg(args[n], XmNmarginWidth, 5); n++;
    XtSetArg(args[n], XmNmarginHeight, 5); n++;
    XtSetArg(args[n], XmNshadowThickness, 4); n++;
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

    defaultColormap = DefaultColormap(XtDisplay(sonar->toplevel),
                    DefaultScreen(XtDisplay(sonar->toplevel)));

    for(i = 0; i < 256; i++)
         grayScale[i].pixel = i;

    for(i = 0; i < 256; i++)
        XQueryColor(XtDisplay(sonar->toplevel), defaultColormap, &grayScale[i]);

    XStoreColors(XtDisplay(sonar->toplevel), sonar->colormap, grayScale, 256);

    main_window_attr.colormap = sonar->colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(sonar->toplevel), XtWindow(statusDialog),
                                      main_window_valuemask, &main_window_attr);

    XFreeColormap(XtDisplay(sonar->toplevel), defaultColormap);
     */

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
        XNextEvent(XtDisplay(sonar->toplevel),&event);
        XtDispatchEvent(&event);
        if(event.type == Expose && event.xexpose.window == XtWindow(drawProgress))
            checkingEvents = False;
        }

    return;

}
