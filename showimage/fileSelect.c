
#include "showimage2.h"
#include "externals.h"


void file_select(w,client_data,callback_data)
Widget w;
XtPointer client_data;
XtPointer callback_data; 
{


    struct draw *drawarea = (struct draw *) client_data;
    XmAnyCallbackStruct *callback = (XmAnyCallbackStruct *) callback_data; 

    Arg args[40];
    Cardinal n;
    Widget getFileDialog;
    Widget zoomSaveFileDialog;
    Widget mainSaveFileDialog;

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    void file_retriever();
    void message_display();

    XtSetSensitive(w, False);

    XmUpdateDisplay(drawarea->shell);

    /*
     *    Create a colormap entry for this shell to match the grayscale
     *    colormap of the main shell.  This will keep the display from 
     *    blinking if the lower entries of the colormap are filled by 
     *    another application.  Set the colormap attributes in the 
     *    appropriate dialog creation.
     */


    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    if(w == XtNameToWidget(drawarea->shell, "*FileRetrieveButton"))
        {
        n = 0;
        XtSetArg(args[n], XmNallowShellResize, True); n++;
        XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
        XtSetArg(args[n], XmNdirMask,
        XmStringCreateLtoR(TOP_DIR, XmSTRING_DEFAULT_CHARSET)); n++;
        getFileDialog = XmCreateFileSelectionDialog(drawarea->shell,
                                              "GetFileDialog", args, n);

        XtAddCallback(getFileDialog, XmNokCallback, 
                                           file_retriever, drawarea);
        XtAddCallback(getFileDialog, XmNcancelCallback,
                                           file_retriever, drawarea);
        XtManageChild(getFileDialog);

        XChangeWindowAttributes(XtDisplay(drawarea->shell),
           XtWindow(drawarea->shell), main_window_valuemask, &main_window_attr);

        }

    if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveImageButton"))
        {
        if(subimage == NULL)
            {
            message_display(drawarea, NOZOOM);
            XtSetSensitive(w, True);
            return;
            }

        n = 0;
        XtSetArg(args[n], XmNallowShellResize, True); n++;
        XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
        XtSetArg(args[n], XmNdirMask,
        XmStringCreateLtoR(TOP_DIR, XmSTRING_DEFAULT_CHARSET)); n++;
        zoomSaveFileDialog = XmCreateFileSelectionDialog(drawarea->shell,
                                              "ZoomSaveFileDialog", args, n);

        XtAddCallback(zoomSaveFileDialog, XmNokCallback,
                                           file_retriever, drawarea);
        XtAddCallback(zoomSaveFileDialog, XmNcancelCallback,
                                           file_retriever, drawarea);
        XtManageChild(zoomSaveFileDialog);

        XChangeWindowAttributes(XtDisplay(drawarea->shell),
        XtWindow(zoomSaveFileDialog), main_window_valuemask, &main_window_attr);
        }
    
    if(w == XtNameToWidget(drawarea->shell, "*MainSaveImageButton"))
        {
        if(fp1 == -1 || fp1 == EOF)
            {
            message_display(drawarea, fp1);
            XtSetSensitive(w, True);
            return;
            }

        n = 0;
        XtSetArg(args[n], XmNallowShellResize, True); n++;
        XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
        XtSetArg(args[n], XmNdirMask,
        XmStringCreateLtoR(TOP_DIR, XmSTRING_DEFAULT_CHARSET)); n++;
        mainSaveFileDialog = XmCreateFileSelectionDialog(drawarea->shell,
                                              "MainSaveFileDialog", args, n);

        XtAddCallback(mainSaveFileDialog, XmNokCallback,
                                           file_retriever, drawarea);
        XtAddCallback(mainSaveFileDialog, XmNcancelCallback,
                                           file_retriever, drawarea);
        XtManageChild(mainSaveFileDialog);
        XChangeWindowAttributes(XtDisplay(drawarea->shell),
        XtWindow(mainSaveFileDialog), main_window_valuemask, &main_window_attr);
        }


    return;

}
