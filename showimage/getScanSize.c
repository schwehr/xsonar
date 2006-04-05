#include <unistd.h>
 
#include "sonar.h"
#include "showimage2.h"
#include "externals.h"

int get_scansize(drawarea, warning)
struct draw *drawarea;
int warning;
{


    Arg args[40];
    Cardinal n;

    struct long_pos_buf usgs;

    Widget scanDialog;
    /*Widget scanbutton;*/
    Widget scanForm;
    Widget scanRowColumn1;
    Widget buttonRowColumn;

    Widget scanMessage;
    Widget scanText;
    Widget scanHeaderMessage;
    Widget scanHeaderText;
    Widget fileHeaderMessage;
    Widget fileHeaderText;
    Widget doneButton;
    Widget cancelButton;
    Widget separator1;

    int answer;
    int status;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    char scanBuf[10];

    void closeScanInput();

    answer = 0;

    XmUpdateDisplay(drawarea->shell);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNtitle, "Record Length"); n++;
    XtSetArg(args[n], XmNtransientFor, drawarea->shell); n++;
    scanDialog = XtCreatePopupShell("ScanDialog",
            transientShellWidgetClass, drawarea->shell, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    scanForm = XtCreateManagedWidget("ScanForm",
                xmFormWidgetClass, scanDialog, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    scanRowColumn1 = XtCreateWidget("ScanRowColumn1",
                xmRowColumnWidgetClass, scanForm, args, n);
    XtManageChild(scanRowColumn1);
/*
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, scanRowColumn1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    scanRowColumn2 = XtCreateWidget("ScanRowColumn2",
                xmRowColumnWidgetClass, scanForm, args, n);
    XtManageChild(scanRowColumn2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, scanRowColumn2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    scanRowColumn3 = XtCreateWidget("ScanRowColumn3",
                xmRowColumnWidgetClass, scanForm, args, n);
    XtManageChild(scanRowColumn3);
*/
    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Scan width (including header):",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    scanMessage = XtCreateWidget("ScanMessage",
        xmLabelWidgetClass, scanRowColumn1, args, n);
    XtManageChild(scanMessage);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Scan header size (0 for none):",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    scanHeaderMessage = XtCreateWidget("ScanHeaderMessage",
        xmLabelWidgetClass, scanRowColumn1, args, n);
    XtManageChild(scanHeaderMessage);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("File header size (0 for none):",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    fileHeaderMessage = XtCreateWidget("FileHeaderMessage",
        xmLabelWidgetClass, scanRowColumn1, args, n);
    XtManageChild(fileHeaderMessage);

    n = 0;
    XtSetArg(args[n], XmNwidth, 10); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    scanText = XtCreateWidget("ScanText",
                        xmTextFieldWidgetClass, scanRowColumn1, args, n);
    XtManageChild(scanText);

    n = 0;
    XtSetArg(args[n], XmNwidth, 10); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    if(warning)
        {
        sprintf(scanBuf, "%d", 256);
        XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(scanBuf,
                           XmSTRING_DEFAULT_CHARSET)); n++;
        }
    scanHeaderText = XtCreateWidget("ScanHeaderText",
                        xmTextFieldWidgetClass, scanRowColumn1, args, n);
    XtManageChild(scanHeaderText);

    if(warning)
        {
        inbytes = read(fp1, &usgs, 256);
        lseek(fp1, -256L, SEEK_CUR);
        sprintf(scanBuf, "%d", usgs.sdatasize);
        XmTextFieldSetString(scanText, scanBuf);
        sprintf(scanBuf, "%d", 256);
        XmTextFieldSetString(scanHeaderText, scanBuf);
        }

    n = 0;
    XtSetArg(args[n], XmNwidth, 10); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    fileHeaderText = XtCreateWidget("FileHeaderText",
                        xmTextFieldWidgetClass, scanRowColumn1, args, n);
    XtManageChild(fileHeaderText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, scanRowColumn1); n++;
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
    XtSetArg(args[n], XmNadjustLast, False); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNspacing, 15); n++;
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
                                         closeScanInput, drawarea);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                         closeScanInput, drawarea);

    XtVaSetValues(cancelButton, XmNuserData, &answer, NULL);
    XtVaSetValues(doneButton, XmNuserData, &answer, NULL);

    XtRealizeWidget(scanDialog);

    XtVaGetValues(drawarea->shell, XmNx, &shellx, NULL);
    XtVaGetValues(drawarea->shell, XmNy, &shelly, NULL);
    XtVaGetValues(drawarea->shell, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(drawarea->shell, XmNheight, &shellHeight, NULL);
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
        XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);

    XtPopdown(scanDialog);
    XtDestroyWidget(scanDialog);


    if(answer == 2)
        status = 0;
    else
        status = 1;

    return(status);


}                
    
void closeScanInput(w,client_data,callback_data)
Widget                 w;
XtPointer                client_data;
XmAnyCallbackStruct     callback_data; 
{

    struct draw *drawarea = (struct draw *) client_data;
    int *answer;

    Widget scantext;
    Widget headertext;

    char *temp1;
    char *temp2;

    XtVaGetValues(w, XmNuserData, &answer, NULL);

    if(w == XtNameToWidget(drawarea->shell, "*ScanDialog*CancelButton"))
        {
        *answer = 2;
        return;
        }

    scantext = XtNameToWidget(drawarea->shell, "*ScanText");
    temp1 = XmTextFieldGetString(scantext);
    scansize = atoi(temp1);

    headertext = XtNameToWidget(drawarea->shell, "*ScanHeaderText");
    temp2 = XmTextFieldGetString(headertext);
    headsize = atoi(temp2);
    datasize = scansize - headsize;


    XtFree(temp2);

    headertext = XtNameToWidget(drawarea->shell, "*FileHeaderText");
    temp2 = XmTextFieldGetString(headertext);
    fileHeaderSize = atoi(temp2);

    if(scansize == 0)
        *answer = 2;
    else
        *answer = 1;

    XtFree(temp1);
    XtFree(temp2);

    return;
}
