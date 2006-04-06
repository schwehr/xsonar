/*
 *    Message display function.  Will display messages
 *    in a pop up shell based on what is passed to it.
 */
#include <assert.h>

#include "showimage2.h"
#include "externals.h"

void message_display(struct draw *drawarea, int file_status)
/*struct draw *drawarea;
int file_status;
*/
{
  assert(drawarea);
    Arg args[40];

    int n;
    int check;
    int answer;
    /*int i;*/

    Widget messagedialog;
    Widget messagebox;
    Widget messagebutton;

    /*XColor grayScale[256];*/

    /*Colormap defaultColormap;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */


    void close_message_display();

    if(file_status == -1)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("A file has not been selected.\nPlease select a file.",
            XmSTRING_DEFAULT_CHARSET)); n++;

        XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
        }
    else if(file_status == 0)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("End of file reached.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        }
    else if(file_status == BADFILE)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("File opened for reading only.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
        }
    else if(file_status == NOSONAR)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("Please display sonar in window.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        }
    else if(file_status == NOTELEM)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("Please select telemetry window on Display button.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        }
    else if(file_status == NOZOOMWIN)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("Please select zoom window on Image button.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        }
    else if(file_status == NOZOOM)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("Please outline zoom area on main window.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
        }
    else if(file_status == NODATA)
        {
        n = 0;
        XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("No data in that area.",
            XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
        }
     else if(file_status == 9)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
               XmStringCreateLtoR("Please pop up the trace window.",
               XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
          }
     else if(file_status == NOTYPE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
               XmStringCreateLtoR("Need to select the sonar file type (File button).",
               XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
          }
     else if(file_status == BADFILE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
               XmStringCreateLtoR("Could not read data from file.",
               XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING); n++;
          }
     else if(file_status == NOSCAN)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
               XmStringCreateLtoR("Please re-display sonar data\nand enter a scan width (in pixels).",
               XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
          }
     else if(file_status == BADWRITE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
               XmStringCreateLtoR("Error writing image file.",
               XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == BADFONT)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
            XmStringCreateLtoR("Could not find requested font, using default.",
            XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == BEGINFILE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("End of processing.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
          }
     else if(file_status == NOSTBDIMAGE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("Starboard image not present in zoom.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == NOPORTIMAGE)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("Port image not present in zoom.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == NOTXSONAR)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("File type is not XSONAR, please reselect.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == NOTQMIPS)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("File type is not QMIPS, please reselect.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
     else if(file_status == NOTRASTER)
          {
          n = 0;
          XtSetArg(args[n], XmNmessageString,
             XmStringCreateLtoR("File type is not RASTER, please reselect.",
             XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
          }
    else
        ;

    messagedialog = XtCreatePopupShell("MessageDialog",
            xmDialogShellWidgetClass, drawarea->shell, NULL, 0);

    messagebox = XtCreateWidget("MessageBox",
        xmMessageBoxWidgetClass, messagedialog, args, n);

    messagebutton = XmMessageBoxGetChild(messagebox,
                XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(messagebutton);

    messagebutton = XmMessageBoxGetChild(messagebox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(messagebutton);

    XtAddCallback(messagebox, XmNokCallback, 
                               close_message_display, &answer);

    if(file_status == READING)
        {
        messagebutton = XmMessageBoxGetChild(messagebox,
                XmDIALOG_OK_BUTTON);
        XtUnmanageChild(messagebutton);
        }

    /*
     *    Create a colormap entry for this shell to match the grayscale
     *    colormap of the main shell.  This will keep the display from
     *    blinking if the lower entries of the colormap are filled by
     *    another application.
     */

    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(messagedialog),
                                      main_window_valuemask, &main_window_attr);

    XtManageChild(messagebox);

    answer = 0;

    if(file_status != READING)
        {
        while(answer == 0)
            XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);
        }
    else
        {
        check = 1;
        while(check == 1)
            {
            if(XtAppPending(drawarea->topLevelApp))
                {
                XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);
                XmUpdateDisplay(drawarea->shell);
                }
            else
                check = 0;
            }
        }

     XtPopdown(messagedialog);
     XtDestroyWidget(messagedialog);


}

void close_message_display(w,client_data,callback_data)
Widget         w;
XtPointer        client_data;
XtPointer     callback_data; 
{

    int *answer = (int *) client_data;

    *answer = 1;

    return;
}

