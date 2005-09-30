
#include "showimage2.h"

void create_trace(w, client_data, call_data)
Widget 		w;
XtPointer 	client_data;
XtPointer 	call_data;
{

	struct draw *drawarea = (struct draw * ) client_data;

	void message_display();

	Arg args[10];
	Cardinal n;

	if(fp1 == EOF)
		{
		message_display(fp1);
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

printf("top ping = %d\n", drawarea->top_ping);

     /*
      *   Start creating widgets for this app
      */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     traceDialog = XtCreatePopupShell("TraceDialog",
               transientShellWidgetClass, shell, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     mainTraceForm = XtCreateWidget("MainTraceForm", xmFormWidgetClass,
                       traceDialog, args, n);
     XtManageChild(mainTraceForm);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     fileMenuBar = XmCreateMenuBar(mainTraceForm, "FileMenuBar", args, n);
     XtManageChild(fileMenuBar);

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'N'); n++;
     fileButton = XtCreateWidget("FileButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
     XtManageChild(fileButton);

     filePullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                        "FilePullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, filePullDownMenu); n++;
     XtSetValues(fileButton, args, n);

     exitButton = XtCreateWidget("ExitButton", xmPushButtonWidgetClass,
                                             filePullDownMenu, NULL, 0);
     XtManageChild(exitButton);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, fileMenuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator = XtCreateWidget("separator",
                    xmSeparatorWidgetClass, mainNavForm, args, n);
     XtManageChild(separator);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     frame = XtCreateWidget("WindowFrame", xmFrameWidgetClass,
                              mainNavForm, args, n);
     XtManageChild(frame);

     n = 0;
     XtSetArg(args[n], XmNheight, 100); n++;
     XtSetArg(args[n], XmNwidth, 700); n++;
     drawarea->traceWindow = XtCreateWidget("TraceWindow", xmDrawingAreaWidgetClass,
                              frame, args, n);
     XtManageChild(drawarea->traceWindow);

     XtAddCallback(exitButton, XmNactivateCallback, 
                                                 close_app, sonar);
/*
     XtAddCallback(drawarea->traceWindow, XmNexposeCallback, redraw, sonar);
*/

     XtRealizeWidget(traceDialog);
     XtPopup(traceDialog, XtGrabNone);

	return;
}

void close_app(w, client_data, call_data)
Widget      w;
XtPointer   client_data;
XtPointer   call_data;
{

     Widget parent_widget;

     parent_widget = w;

     while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);

fprintf(stdout, "parent = %s\n", XtName(parent_widget));

     XtPopdown(parent_widget);
     XtDestroyWidget(parent_widget);

     return;
}

