 
#include "showimage2.h"
#include <stdlib.h>
#include "externals.h"

void getCentralLon(drawarea)
struct draw *drawarea;
{


     Arg args[40];
     Cardinal n;

     Widget clonDialog;
     Widget clonbutton;
     Widget clonForm;
     Widget clonRowColumn1;
     Widget clonRowColumn2;
     Widget buttonRowColumn;
     Widget clonMessage;
     Widget clonText;
     Widget doneButton;
     Widget cancelButton;
     Widget separator1;

     int answer;

     char *temp;
     char *clon_buf;

     Dimension shellx;
     Dimension shelly;
     Dimension shellWidth;
     Dimension shellHeight;
     Dimension dialogHeight;
     Dimension dialogWidth;

     float getcentlon();

     void closeClonInput();

     answer = 0;

     XmUpdateDisplay(drawarea->shell);

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     clonDialog = XtCreatePopupShell("ClonDialog",
               transientShellWidgetClass, drawarea->shell, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     clonForm = XtCreateManagedWidget("ClonForm",
                    xmFormWidgetClass, clonDialog, args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     clonRowColumn1 = XtCreateWidget("ClonRowColumn1",
                    xmRowColumnWidgetClass, clonForm, args, n);
     XtManageChild(clonRowColumn1);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Enter the central longitude:",
          XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     clonMessage = XtCreateWidget("ClonMessage",
          xmLabelWidgetClass, clonRowColumn1, args, n);
     XtManageChild(clonMessage);

     n = 0;
     XtSetArg(args[n], XmNwidth, 10); n++;
     XtSetArg(args[n], XmNcolumns, 10); n++;
     clonText = XtCreateWidget("ClonText",
                         xmTextFieldWidgetClass, clonRowColumn1, args, n);
     XtManageChild(clonText);
     clon_buf = (char *) XtCalloc(1, 6);
     sprintf(clon_buf, "%.1f", drawarea->clon);
     XmTextSetString(clonText, clon_buf);
     XtFree(clon_buf);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, clonRowColumn1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     separator1 = XtCreateWidget("Separator1",
                    xmSeparatorWidgetClass, clonForm, args, n);
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
                    xmRowColumnWidgetClass, clonForm, args, n);
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
                                          closeClonInput, drawarea);
     XtAddCallback(cancelButton, XmNactivateCallback, 
                                          closeClonInput, drawarea);

     XtVaSetValues(cancelButton, XmNuserData, &answer, NULL);
     XtVaSetValues(doneButton, XmNuserData, &answer, NULL);

     XtRealizeWidget(clonDialog);

     XtVaGetValues(drawarea->shell, XmNx, &shellx, NULL);
     XtVaGetValues(drawarea->shell, XmNy, &shelly, NULL);
     XtVaGetValues(drawarea->shell, XmNwidth, &shellWidth, NULL);
     XtVaGetValues(drawarea->shell, XmNheight, &shellHeight, NULL);
     XtVaGetValues(clonDialog, XmNwidth, &dialogWidth, NULL);
     XtVaGetValues(clonDialog, XmNheight, &dialogHeight, NULL);

     shellx = (shellWidth / 2) + shellx; 
     shellx -= (dialogWidth / 2);
     shelly = (shellHeight / 2) + shelly;
     shelly -= (dialogHeight / 2);

     n = 0;
     XtSetArg(args[n], XmNx, shellx); n++;
     XtSetArg(args[n], XmNy, shelly); n++;
     XtSetValues(clonDialog, args, n);

     XtPopup(clonDialog, XtGrabNone);

     while(answer == 0)
          XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);

     if(answer == 1)
        {
        temp = XmTextFieldGetString(clonText);
        drawarea->clon = (float) atof(temp);
        drawarea->clon = getcentlon(drawarea->clon);

        XtFree(temp);
        }

     XtPopdown(clonDialog);
     XtDestroyWidget(clonDialog);

     return;


}                    

/*   getcentlon()
 *
 *   Returns the central longitude for the array lon in the UTM standard
 */

float getcentlon (lon)
float lon;
{
     int fuse;
     float c_lon;


     if (lon > 0.0)
          lon += 6.0;
     else
          lon -= 6.0;
     fuse = lon / 6;

     if (lon > 0.0)
          fuse -= 1;
     c_lon = 3.0 + fuse * 6.0;

     return(c_lon);
}

void closeClonInput(w,client_data,callback_data)
Widget         w;
XtPointer      client_data;
XtPointer      callback_data;
{

     struct draw *drawarea = (struct draw *) client_data;

     int *answer;

     XtVaGetValues(w, XmNuserData, &answer, NULL);

     if(w == XtNameToWidget(drawarea->shell, "*ClonDialog*CancelButton"))
         {
         *answer = 2;
         return;
         }

     *answer = 1;

     return;
}

