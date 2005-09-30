 
#include <Xm/DialogS.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>

#include "sonar_struct.h"

#include "xsonar.h"

void closeScanInput(Widget w, XtPointer client_data, UNUSED  XtPointer callback_data);

int getScanSize(MainSonar *sonar, int *scansize, int *headsize)
{
     Arg args[20];
     Cardinal n;

     Widget scanDialog;
     /*Widget scanbutton;*/
     Widget scanForm;
     Widget scanRowColumn1;
     Widget scanRowColumn2;
     Widget buttonRowColumn;
     Widget scanMessage;
     Widget scanText;
     Widget headerMessage;
     Widget headerText;
     Widget doneButton;
     Widget cancelButton;
     Widget separator1;

     int answer;

     char *temp1;
     char scanBuf[20];

     Dimension shellx;
     Dimension shelly;
     Dimension shellWidth;
     Dimension shellHeight;
     Dimension dialogHeight;
     Dimension dialogWidth;

     /*void closeScanInput(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/

     answer = 0;

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     scanDialog = XtCreatePopupShell("ScanDialog",
     		transientShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     scanForm = XtCreateManagedWidget("ScanForm",
     			xmFormWidgetClass, scanDialog, args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     scanRowColumn1 = XtCreateWidget("ScanRowColumn1",
     			xmRowColumnWidgetClass, scanForm, args, n);
     XtManageChild(scanRowColumn1);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, scanRowColumn1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     scanRowColumn2 = XtCreateWidget("ScanRowColumn2",
     			xmRowColumnWidgetClass, scanForm, args, n);
     XtManageChild(scanRowColumn2);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
     	XmStringCreateLtoR("scan width\n(including header):",
     	XmSTRING_DEFAULT_CHARSET)); n++;
     scanMessage = XtCreateWidget("ScanMessage",
     	xmLabelWidgetClass, scanRowColumn1, args, n);
     XtManageChild(scanMessage);

     sprintf(scanBuf, "%d", sonar->ping_header.sdatasize);
     n = 0;
     XtSetArg(args[n], XmNeditable, True); n++;
     XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
     XtSetArg(args[n], XmNwidth, 15); n++;
     XtSetArg(args[n], XmNcolumns, 15); n++;
     XtSetArg(args[n], XmNvalue, scanBuf); n++;
     scanText = XtCreateWidget("ScanText",
                         xmTextWidgetClass, scanRowColumn1, args, n);
     XtManageChild(scanText);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
     	XmStringCreateLtoR("          scan header\n(0 for none):",
     	XmSTRING_DEFAULT_CHARSET)); n++;
     headerMessage = XtCreateWidget("HeaderMessage",
     	xmLabelWidgetClass, scanRowColumn2, args, n);
     XtManageChild(headerMessage);

     n = 0;
     XtSetArg(args[n], XmNwidth, 15); n++;
     XtSetArg(args[n], XmNcolumns, 15); n++;
     headerText = XtCreateWidget("HeaderText",
                         xmTextWidgetClass, scanRowColumn2, args, n);
     XtManageChild(headerText);

     sprintf(scanBuf, "%d", sonar->ping_header.sdatasize);
     XmTextSetString(scanText, scanBuf);
     sprintf(scanBuf, "%d", 256);
     XmTextSetString(headerText, scanBuf);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, scanRowColumn2); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator1 = XtCreateWidget("Separator1",
                    xmSeparatorWidgetClass, scanForm, args, n);
     XtManageChild(separator1);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     XtSetArg(args[n], XmNnumColumns, 1); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     XtSetArg(args[n], XmNspacing, 10); n++;
     buttonRowColumn = XtCreateWidget("ButtonRowColumn",
     			xmRowColumnWidgetClass, scanForm, args, n);
     XtManageChild(buttonRowColumn);

     n = 0;
     XtSetArg(args[n], XmNwidth, 20); n++;
     XtSetArg(args[n], XmNlabelString,
     	XmStringCreateLtoR("Cancel", XmSTRING_DEFAULT_CHARSET)); n++;
     cancelButton = XtCreateWidget("CancelButton",
     			xmPushButtonWidgetClass, buttonRowColumn, args, n);
     XtManageChild(cancelButton);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
     	XmStringCreateLtoR("Ok", XmSTRING_DEFAULT_CHARSET)); n++;
     doneButton = XtCreateWidget("DoneButton",
     			xmPushButtonWidgetClass, buttonRowColumn, args, n);
     XtManageChild(doneButton);

     XtAddCallback(doneButton, XmNactivateCallback, 
                                        closeScanInput, &answer);
     XtAddCallback(cancelButton, XmNactivateCallback, 
                                        closeScanInput,&answer);

     XtRealizeWidget(scanDialog);

     XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
     XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
     XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
     XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
     XtVaGetValues(scanDialog, XmNwidth, &dialogWidth, NULL);
     XtVaGetValues(scanDialog, XmNheight, &dialogHeight, NULL);

     shellx = (shellWidth / 2) + shellx; 
     shellx -= (dialogWidth / 2);
     shelly = (shellHeight / 2) + shelly;
     shelly -= (dialogHeight / 2);

     n = 0;
     XtSetArg(args[n], XmNx, shellx); n++;
     XtSetArg(args[n], XmNy, shelly); n++;
     XtSetValues(scanDialog, args, n);

     XtPopup(scanDialog, XtGrabNone);

     while(answer == 0)
          XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

     temp1 = XmTextGetString(scanText);
     *scansize = atoi(temp1);

     XtFree((char *)temp1);

     temp1 = XmTextGetString(headerText);
     *headsize = atoi(temp1);

     XtFree((char *)temp1);

     XtPopdown(scanDialog);
     XtDestroyWidget(scanDialog);

     return(answer);


}     			
     
void closeScanInput(Widget w, XtPointer client_data, UNUSED  XtPointer callback_data)
{

     int *answer = (int *) client_data;
     Widget parent_widget = w;

     while(!XtIsTransientShell(parent_widget))
     	parent_widget = XtParent(parent_widget);

     if(w == XtNameToWidget(parent_widget, "*CancelButton"))
     	{
     	*answer = 2;
     	return;
     	}

     *answer = 1;

     return;
}
