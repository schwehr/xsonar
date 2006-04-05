/*
 *      Browse through file callback.
 */
#include <unistd.h>

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void goto_start(w,client_data,call_data)
Widget        w;
XtPointer        client_data;
XtPointer        call_data; 
{

    long position;
    struct draw *drawarea = (struct draw *) client_data;

    drawarea->top_ping = 0;

    position = lseek(fp1, 0L, SEEK_SET);    

    return;

}

void goto_ping(w, client_data, call_data)
Widget        w;
XtPointer        client_data;
XtPointer        call_data; 
{

    struct draw *drawarea = (struct draw *) client_data;

    Arg args[40];
    int /*i,*/ n;
    Widget pingdialog;
    Widget ping_bb;
    Widget pingbutton;
    Widget pingtext;

    /*XColor grayScale[256];*/

    /*Colormap defaultColormap;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */


    void close_ping_input();

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    pingdialog = XtCreatePopupShell("pingInput",
            xmDialogShellWidgetClass, drawarea->shell, args, n);

    /*
     *    Create a colormap entry for this shell to match the grayscale
     *    colormap of the main shell.  This will keep the display from
     *    blinking if the lower entries of the colormap are filled by
     *    another application.
     */

    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    ping_bb = XtCreateWidget("pingBoard", xmBulletinBoardWidgetClass,
                     pingdialog, NULL, 0);
    XtManageChild(ping_bb);

    pingbutton = XtCreateWidget("pingButton", xmPushButtonWidgetClass, ping_bb,
        NULL, 0);
    XtManageChild(pingbutton);

    pingtext = XtCreateWidget("pingText",
                    xmTextWidgetClass, ping_bb, NULL, 0);
    XtManageChild(pingtext);

    drawarea->spare = pingtext;

    XtAddCallback(pingbutton, XmNactivateCallback,
                                         close_ping_input, drawarea);

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(pingdialog),
                                      main_window_valuemask, &main_window_attr);

    return;


}

void close_ping_input(w,client_data,callback_data)
Widget                 w;
XtPointer             client_data;
XmAnyCallbackStruct     *callback_data; 
{

    struct draw *drawarea = (struct draw *) client_data;
 
    char *temp;
    long offset, position;
    int ping;

    temp = XmTextGetString(drawarea->spare);
    ping = atoi(temp);

    if(lseek(fp1, 0L, SEEK_CUR) == 0)
        drawarea->bitsPerPixel = getHeaderInfo(drawarea);

    offset = (long) ping * (long) scansize;
    drawarea->top_ping = ping;

    position = lseek(fp1, offset, SEEK_SET);

    XtFree(temp);

    return;
}
