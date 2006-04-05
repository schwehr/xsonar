#include <unistd.h>
#include "showimage2.h" 
#include "externals.h" 
#include "sonar.h" 
void create_trace(w, client_data, call_data) 
Widget         w; 
XtPointer     client_data; 
XtPointer     call_data; 
{ 
    struct draw *drawarea = (struct draw * ) client_data; 

    Widget traceDialog;
    Widget traceForm;
    Widget traceFileMenuBar;
    Widget traceFilePullDownMenu;
    Widget traceFileButton;
    Widget traceExitButton;
    Widget traceFrame;
    Widget traceWindow;
    Widget separator;

    XFontStruct *label_font;
    GC trace_gc;

    /*Status result;*/

    /*char lon_label[255];*/

    /*XColor grayScale[256];*/

    /*Colormap traceColormap;*/
    /*XColor colors[256];*/
    /*Colormap defaultColormap;*/              /* colormap from server */


    unsigned long main_window_valuemask;
    /*unsigned long cell;*/                      /* pixel number */
    /*unsigned long pixels[256];*/               /* pixel values in colormap */
    /*unsigned long plane_masks[8];*/            /* number of planes in system */

    XSetWindowAttributes main_window_attr;  /* window attribute struct */

    int data_offset;
    int i;

    void message_display();
    void drawScales();

    void closeTraceWindow();

    Arg args[40];
    Cardinal n;

    if(fp1 == -1)
        {
        message_display(drawarea, fp1);
        return;
        }

    if(drawarea->fileType == USGS)
        data_offset = headsize;
    else
        data_offset = 0;

    if(drawarea->fileType == USGS)
        drawarea->top_ping = lseek(fp1, 0L, SEEK_CUR) / scansize;
    if(drawarea->fileType == QMIPS)
        drawarea->top_ping = (lseek(fp1, 0L, SEEK_CUR) - 1024) / scansize;


     /*
      *   Start creating widgets for this app
      */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     traceDialog = XtCreatePopupShell("TraceDialog",
               transientShellWidgetClass, drawarea->shell, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     traceForm = XtCreateWidget("TraceForm", xmFormWidgetClass,
                       traceDialog, args, n);
     XtManageChild(traceForm);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     traceFileMenuBar = XmCreateMenuBar(traceForm, 
                                              "TraceFileMenuBar", args, n);
     XtManageChild(traceFileMenuBar);

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'N'); n++;
     traceFileButton = XtCreateWidget("TraceFileButton",
                    xmCascadeButtonWidgetClass, traceFileMenuBar, args, n);
     XtManageChild(traceFileButton);

     traceFilePullDownMenu = XmCreatePulldownMenu(traceFileMenuBar,
                                        "TraceFilePullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, traceFilePullDownMenu); n++;
     XtSetValues(traceFileButton, args, n);

     traceExitButton = XtCreateWidget("TraceExitButton", 
                            xmPushButtonWidgetClass,
                            traceFilePullDownMenu, NULL, 0);
     XtManageChild(traceExitButton);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, traceFileMenuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator = XtCreateWidget("Separator",
                    xmSeparatorWidgetClass, traceForm, args, n);
     XtManageChild(separator);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     traceFrame = XtCreateWidget("TraceFrame", xmFrameWidgetClass,
                              traceForm, args, n);
     XtManageChild(traceFrame);

     n = 0;
     XtSetArg(args[n], XmNheight, 350); n++;
     XtSetArg(args[n], XmNwidth, 550); n++;
     traceWindow = XtCreateWidget("TraceWindow", xmDrawingAreaWidgetClass,
                              traceFrame, args, n);
     XtManageChild(traceWindow);

     XtAddCallback(traceExitButton, XmNactivateCallback, 
                                       closeTraceWindow, NULL);
/*
     XtAddCallback(drawarea->traceWindow, XmNexposeCallback, redraw, sonar);
*/

     XtRealizeWidget(traceDialog);
    XtPopup(traceDialog, XtGrabNone);

    /*
     *    Create a GC for the drawing area widget so it can be
     *    drawn into.
     */

    trace_gc = XCreateGC(XtDisplay(traceDialog), XtWindow(traceWindow), 
                                                                None, NULL);

    label_font = XLoadQueryFont(XtDisplay(traceDialog),
            "-adobe-helvetica-*-r-*-*-*-120-*-*-*-*-*-*");
/*
            "-adobe-times-bold-r-normal--12-120-75-75-m-67-iso8859-1");
            "-adobe-courier-bold-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-courier-medium-r-normal--8-80-75-75-m-50-iso8859-1");
            "-adobe-helvetica-medium-r-normal--8-80-75-75-p-46-iso8859-1");
*/

    if(label_font == 0)
        message_display(drawarea, BADFONT);
    else
        XSetFont(XtDisplay(traceDialog), trace_gc, label_font->fid);

    /*
     *    Create a colormap entry for this shell to match the grayscale
     *    colormap of the main shell.  This will keep the display from
     *    blinking if the lower entries of the colormap are filled by
     *    another application.

    defaultColormap = DefaultColormap(XtDisplay(traceDialog),
                    DefaultScreen(XtDisplay(traceDialog)));

    traceColormap = XCreateColormap(XtDisplay(traceDialog), XtWindow(traceWindow),
                    DefaultVisualOfScreen(XtScreen(traceDialog)), AllocNone);

    result = XAllocColorCells(XtDisplay(traceDialog), traceColormap,
                              True, plane_masks, 8, pixels, 1);
    if(result == 0)
        {
        fprintf(stderr, "Color cell allocation failed.\n");
        perror("Reason");
        exit(-1);
        }

    for(cell = 0; cell < 256; cell++)
        colors[cell].pixel = cell;

    XQueryColors(XtDisplay(traceDialog), defaultColormap, colors, 256);


    for(i = 0; i < 191; i++)
        XQueryColor(XtDisplay(traceDialog), defaultColormap, &colors[i]);
    for(i = 191; i < 256; i++)
        XQueryColor(XtDisplay(drawarea->shell),
                            drawarea->graphics_colormap,&colors[i]);

    colors[90].red = 65535;
    colors[90].blue = 0;
    colors[90].green = 0;
    colors[90].pixel = 90;
    colors[90].flags = DoRed | DoGreen | DoBlue;

    XStoreColors(XtDisplay(drawarea->shell), traceColormap, colors, 256);

     */
    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(traceDialog), XtWindow(traceDialog),
                                      main_window_valuemask, &main_window_attr);
    XChangeWindowAttributes(XtDisplay(traceDialog), XtWindow(traceWindow),
                                      main_window_valuemask, &main_window_attr);

    /*
    XFreeColormap(XtDisplay(drawarea->shell), defaultColormap);
    */

    XSetBackground(XtDisplay(traceDialog), trace_gc,
             BlackPixelOfScreen(XtScreen(traceWindow)));

    XSetForeground(XtDisplay(traceDialog), trace_gc,
             WhitePixelOfScreen(XtScreen(traceWindow)));

    drawScales(traceDialog, traceWindow, trace_gc);

    XSetLineAttributes(XtDisplay(traceDialog), trace_gc, 2,
                                        LineSolid, CapButt, JoinRound);

    for(i = 25; i < 325; i += 5)
    XDrawLine(XtDisplay(traceDialog), XtWindow(traceWindow),
                            trace_gc, 25, i, 20, i);

    XDrawLine(XtDisplay(traceDialog), XtWindow(traceWindow),
                            trace_gc, 25, 25, 25, 325);

    XDrawLine(XtDisplay(traceDialog), XtWindow(traceWindow),
                            trace_gc, 25, 325, 525, 325);

    XDrawLine(XtDisplay(traceDialog), XtWindow(traceWindow),
                            trace_gc, 525, 325, 525, 25);

    return;
}

void closeTraceWindow(w, client_data, call_data)
Widget      w;
XtPointer   client_data;
XtPointer   call_data;
{

     Widget parent_widget;

     parent_widget = w;

     while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);

     XtPopdown(parent_widget);
     XtDestroyWidget(parent_widget);

     return;
}


void drawScales(dialog, traceWindow, traceGC)
Widget dialog;
Widget traceWindow;
GC traceGC;
{

    int i;

    XSetLineAttributes(XtDisplay(dialog), traceGC, 2,
                                        LineSolid, CapButt, JoinRound);

    for(i = 25; i < 325; i += 5)
    XDrawLine(XtDisplay(dialog), XtWindow(traceWindow),
                            traceGC, 25, i, 20, i);

    XDrawLine(XtDisplay(dialog), XtWindow(traceWindow),
                            traceGC, 25, 25, 25, 325);

    XDrawLine(XtDisplay(dialog), XtWindow(traceWindow),
                            traceGC, 25, 325, 525, 325);

    XDrawLine(XtDisplay(dialog), XtWindow(traceWindow),
                            traceGC, 525, 325, 525, 25);

    return;

}
