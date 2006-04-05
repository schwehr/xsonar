 
#include "showimage2.h"
#include "externals.h"

void get_swath(drawarea)
struct draw *drawarea;
{


     Arg args[40];
     Cardinal n;

     Widget swathDialog;
     Widget swathbutton;
     Widget swathForm;
     Widget swathRowColumn1;
     Widget swathRowColumn2;
     Widget buttonRowColumn;
     Widget swathMessage;
     Widget swathText;
     Widget doneButton;
     Widget cancelButton;
     Widget separator1;

     int answer;

     char *temp;

     Dimension shellx;
     Dimension shelly;
     Dimension shellWidth;
     Dimension shellHeight;
     Dimension dialogHeight;
     Dimension dialogWidth;

     void closeSwathInput();

     answer = 0;

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     swathDialog = XtCreatePopupShell("SwathDialog",
               transientShellWidgetClass, drawarea->shell, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     swathForm = XtCreateManagedWidget("SwathForm",
                    xmFormWidgetClass, swathDialog, args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     swathRowColumn1 = XtCreateWidget("SwathRowColumn1",
                    xmRowColumnWidgetClass, swathForm, args, n);
     XtManageChild(swathRowColumn1);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Enter the swath width:",
          XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     swathMessage = XtCreateWidget("SwathMessage",
          xmLabelWidgetClass, swathRowColumn1, args, n);
     XtManageChild(swathMessage);

     n = 0;
     XtSetArg(args[n], XmNwidth, 10); n++;
     XtSetArg(args[n], XmNcolumns, 10); n++;
     swathText = XtCreateWidget("SwathText",
                         xmTextFieldWidgetClass, swathRowColumn1, args, n);
     XtManageChild(swathText);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, swathRowColumn1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator1 = XtCreateWidget("Separator1",
                    xmSeparatorWidgetClass, swathForm, args, n);
     XtManageChild(separator1);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     XtSetArg(args[n], XmNadjustLast, False); n++;
     XtSetArg(args[n], XmNnumColumns, 1); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                    xmRowColumnWidgetClass, swathForm, args, n);
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
                                        closeSwathInput, drawarea);
     XtAddCallback(cancelButton, XmNactivateCallback, 
                                        closeSwathInput, drawarea);

     XtVaSetValues(cancelButton, XmNuserData, &answer, NULL);

     XtRealizeWidget(swathDialog);

     XtVaGetValues(drawarea->shell, XmNx, &shellx, NULL);
     XtVaGetValues(drawarea->shell, XmNy, &shelly, NULL);
     XtVaGetValues(drawarea->shell, XmNwidth, &shellWidth, NULL);
     XtVaGetValues(drawarea->shell, XmNheight, &shellHeight, NULL);
     XtVaGetValues(swathDialog, XmNwidth, &dialogWidth, NULL);
     XtVaGetValues(swathDialog, XmNheight, &dialogHeight, NULL);

     shellx = (shellWidth / 2) + shellx; 
     shellx -= (dialogWidth / 2);
     shelly = (shellHeight / 2) + shelly;
     shelly -= (dialogHeight / 2);

     n = 0;
     XtSetArg(args[n], XmNx, shellx); n++;
     XtSetArg(args[n], XmNy, shelly); n++;
     XtSetValues(swathDialog, args, n);

     XtPopup(swathDialog, XtGrabNone);

     while(answer == 0)
          XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);

     if(answer == 1)
         {
         temp = XmTextFieldGetString(swathText);
         swath_width = atoi(temp);
         XtFree(temp);
         }

     XtPopdown(swathDialog);
     XtDestroyWidget(swathDialog);

     XmUpdateDisplay(swathDialog);

     return;


}                    

void closeSwathInput(w,client_data,callback_data)
Widget         w;
XtPointer      client_data;
XtPointer      callback_data;
{

     struct draw *drawarea = (struct draw *) client_data;

     int *answer;

     XtVaGetValues(XtNameToWidget(drawarea->shell, 
                     "*SwathDialog*CancelButton"), XmNuserData, &answer, NULL);

     if(w == XtNameToWidget(drawarea->shell, "*SwathDialog*CancelButton"))
         {
         *answer = 2;
         return;
         }

     *answer = 1;

     return;
}

