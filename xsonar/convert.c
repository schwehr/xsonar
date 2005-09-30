/* 
 *
 *    callback to input and convert an image file into the selected format:
 *    Xsonar, MIPS, or WHIPS.
 *
 *    William W. Danforth, U.S. Geological Survey
 *    May, 1995.
 *
 *
 */

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>

#include "xsonar.h"
#include "sonar_struct.h"

void closeConvertDialog(Widget w, XtPointer client_data, XtPointer callback_data);
/*void File_select();*/

void processFile(FileFormat *fileToConvert, MainSonar *sonar);

void fileConvert(Widget w, XtPointer client_data, UNUSED  XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    Widget convertDialog;
    Widget convertForm;

    Widget fileMenuBar;
    Widget filePullDownMenu;
    Widget fileButton;
    Widget fileRetrieveButton;

    Widget separator1;
    Widget separator2;

    Widget convertLabel1;
    Widget convertLabel2;

    Widget toggleHolder1;
    Widget xsonarToggle1;
    /*Widget whipsToggle1;*/
    Widget mipsToggle1;

    Widget toggleHolder2;
    Widget xsonarToggle2;
    Widget whipsToggle2;
    Widget mipsToggle2;

    Widget buttonRowColumn;
    Widget cancelButton;
    Widget doneButton;

    Arg args[10];
    Cardinal n;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    FileFormat fileToConvert;

    /*
     *    Set the calling widget's sensitivity to false
     */

    XtSetSensitive(w, False);

    /*
     *    Create the main dialog for this portion of the application
     *    which will be a Form widget based on a transient shell.
     */

    n = 0;
    XtSetArg(args[n], XmNtitle, "File Format Conversion"); n++;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    convertDialog = XtCreatePopupShell("ConvertDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    convertForm = XtCreateWidget("ConvertForm", xmFormWidgetClass,
                                                 convertDialog, args, n);
    XtManageChild(convertForm);

    /*
     *    Create a menu pulldown for getting the input file,
     *     storing the setup values, and exiting.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    fileMenuBar = XmCreateMenuBar(convertForm, "FileMenuBar", args, n);
    XtManageChild(fileMenuBar);


    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("File",
         XmSTRING_DEFAULT_CHARSET)); n++;
    fileButton = XtCreateWidget("FileButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
    XtManageChild(fileButton);

    filePullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                "FilePullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, filePullDownMenu); n++;
    XtSetValues(fileButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Retrieve",
         XmSTRING_DEFAULT_CHARSET)); n++;
    fileRetrieveButton = XtCreateWidget("FileRetrieveButton",
                    xmPushButtonWidgetClass, filePullDownMenu, args, n);
    XtManageChild(fileRetrieveButton);

    /*
     *   Create the label for the input formats.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, fileMenuBar); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Input format:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    convertLabel1 = XtCreateWidget("ConvertLabel1",
                         xmLabelWidgetClass, convertForm, args, n);
    XtManageChild(convertLabel1);


    /*
     *   Create the input row-column widget holding toggles for input
     *   formats.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, convertLabel1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    toggleHolder1 = XtCreateWidget("ToggleHolder1",
                        xmRowColumnWidgetClass, convertForm, args, n);
    XtManageChild(toggleHolder1);


    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Xsonar",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    xsonarToggle1 = XtCreateWidget("XsonarToggle1",
                        xmToggleButtonWidgetClass, toggleHolder1, args, n);
    XtManageChild(xsonarToggle1);
/*
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("WHIPS",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    whipsToggle1 = XtCreateWidget("WhipsToggle1",
                        xmToggleButtonWidgetClass, toggleHolder1, args, n);
    XtManageChild(whipsToggle1);
*/

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("MIPS",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    mipsToggle1 = XtCreateWidget("MipsToggle1",
                        xmToggleButtonWidgetClass, toggleHolder1, args, n);
    XtManageChild(mipsToggle1);

    /*
     *   Create a separator for the input and output toggle row columns
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, toggleHolder1); n++;
    separator1 = XtCreateWidget("Separator1", xmSeparatorWidgetClass,
                                                      convertForm, args, n);
    XtManageChild(separator1);


    /*
     *   Create the label for the output formats.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Output format:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    convertLabel2 = XtCreateWidget("ConvertLabel2",
                         xmLabelWidgetClass, convertForm, args, n);
    XtManageChild(convertLabel2);

    /*
     *   Create the output row-column widget holding toggles for output
     *   formats.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, convertLabel2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    toggleHolder2 = XtCreateWidget("ToggleHolder2",
                        xmRowColumnWidgetClass, convertForm, args, n);
    XtManageChild(toggleHolder2);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Xsonar",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    xsonarToggle2 = XtCreateWidget("XsonarToggle2",
                        xmToggleButtonWidgetClass, toggleHolder2, args, n);
    XtManageChild(xsonarToggle2);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("WHIPS",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    whipsToggle2 = XtCreateWidget("WhipsToggle2",
                        xmToggleButtonWidgetClass, toggleHolder2, args, n);
    XtManageChild(whipsToggle2);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("MIPS",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    mipsToggle2 = XtCreateWidget("MipsToggle2",
                        xmToggleButtonWidgetClass, toggleHolder2, args, n);
    XtManageChild(mipsToggle2);

    /*
     *   Create a separator for output toggle row column and the 
     *   OK and cancel buttons.
     */

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, toggleHolder2); n++;
    separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                                                         convertForm, args, n);
    XtManageChild(separator2);

    /*
     *   Create the row column to hold the OK and CANCEL buttons.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, convertForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("       Apply        ", 
        XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("       Cancel       ", 
        XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(cancelButton);

    XtAddCallback(doneButton, XmNactivateCallback,
                                       closeConvertDialog, sonar);

    XtAddCallback(cancelButton, XmNactivateCallback,
                                       closeConvertDialog, sonar);

    XtAddCallback(fileRetrieveButton, XmNactivateCallback,
                                       File_select, sonar);

    XtRealizeWidget(convertDialog);

    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(convertDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(convertDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(convertDialog, args, n);

    XtPopup(convertDialog, XtGrabNone);

    fileToConvert.answer = 0;

    XtVaSetValues(doneButton, XmNuserData, &fileToConvert, NULL);
    XtVaSetValues(cancelButton, XmNuserData, &fileToConvert, NULL);

    while(fileToConvert.answer == 0)
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

    if(fileToConvert.answer == 1)
        processFile(&fileToConvert, sonar);

    XtDestroyWidget(convertDialog);

    XtSetSensitive(w, True);

    return;

}

void closeConvertDialog(Widget w, XtPointer client_data, 
			UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    /*int *answer;*/
    char callingWidget[50];
    /*char message[100];*/
    /*void messages();*/

    FileFormat *fileToConvert;

    strcpy(callingWidget, XtName(w));

    /*
     *   Only set this if the done button is selected, otherwise set answer
     *   to 1 and let the user cancel the operation.
     */

    XtVaGetValues(w, XmNuserData, &fileToConvert, NULL);

    if(!strcmp(callingWidget, "DoneButton"))
        {

        fileToConvert->answer = 1;


        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*XsonarToggle1")))
            fileToConvert->inputFormat = XSONAR;

/*
        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*WhipsToggle1")))
            fileToConvert->inputFormat = WHIPS;
*/

        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*MipsToggle1")))
            fileToConvert->inputFormat = MIPS;

        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*XsonarToggle2")))
            fileToConvert->outputFormat = XSONAR;

        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*WhipsToggle2")))
            fileToConvert->outputFormat = WHIPS;

        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                         "*ConvertDialog*MipsToggle2")))
            fileToConvert->outputFormat = MIPS;


        }

    if(!strcmp(callingWidget, "CancelButton"))
        {
        fileToConvert->answer = 2;
        fileToConvert->inputFormat = 0;
        fileToConvert->outputFormat = 0;
        }

/*
fprintf(stderr, "answer = %d\n", fileToConvert->answer);
fprintf(stderr, "input format = %d\n", fileToConvert->inputFormat);
fprintf(stderr, "output format = %d\n", fileToConvert->outputFormat);
*/

    return;
}


void processFile(FileFormat *fileToConvert, MainSonar *sonar)
{

    FILE *whipsNavFP=0;  /* FIX: can be used uninitialized */

    /*char *stringPtr;*/
    char buffer[255];
    char fileBuffer[255];
    char whipsNavBuffer[255];
    char navBuffer[255];
    char warningMessage[255];

    int buffer_len;
    int inputNavFP; /* FIX: can be used uninitialized */

    /*
     *   Check to make sure the input file can be opened for reading.
     */

    if((fileToConvert->inputFileFP = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningMessage,
                 "Error opening input file.\nPlease reselect a file.");
        messages(sonar, warningMessage);
        return;
        }

    /*
     *   Create the output file based on the selected conversion type.
     */

    strcpy(buffer, sonar->infile);

    buffer_len = (int) strlen(buffer);
    while(*(buffer + --buffer_len) != '.')      /* just find the "." */
        strncpy((buffer + buffer_len),"\0",1);

    strcpy(fileBuffer, buffer);

    /*
     *    Place the appropriate suffixes onto the output files
     */

    switch(fileToConvert->inputFormat)
        {
        case XSONAR:
            if(fileToConvert->outputFormat == WHIPS) 
                {

                strcpy(navBuffer, buffer);
                strncat(navBuffer, "nav", 3);
    
                strncat(fileBuffer, "whp", 3);
                strcpy(whipsNavBuffer, fileBuffer);
                strncat(whipsNavBuffer, ".hdr", 4);
        
                inputNavFP =  open(navBuffer, O_RDONLY);
                whipsNavFP =  fopen(whipsNavBuffer, "w");

                }
            /*
             *  If the output type and input type are XSONAR, leave the
             *  file name the same, just append ".cnv" to the input file.
             */

            if(fileToConvert->outputFormat == XSONAR) 
                {
                strcpy(fileBuffer, sonar->infile);
                strcat(fileBuffer, "cnv");
                }

            break;
                
        case MIPS:
#if 0
            if(fileToConvert->outputFormat == XSONAR) 
                ;    /*  do nothing, just use the prefix of the input file */
#endif
            break;

        }


    /*
     *    Open the output file to check that it can be done and to 
     *    pass it on to the appropriate processing routine.
     */

    if((fileToConvert->outputFileFP = open(fileBuffer, O_RDWR|O_CREAT|O_TRUNC, 
        PMODE)) == -1)
        {
        sprintf(warningMessage,
             "Error opening output file.\nCheck directory permissions.");
        messages(sonar, warningMessage);
        return;
        }


    /*
     *    Begin processing ....
     */

    XtPopdown(XtNameToWidget(sonar->toplevel, "*ConvertDialog"));

    switch(fileToConvert->inputFormat)
        {
        case XSONAR:
            if(fileToConvert->outputFormat == WHIPS)
                whipsWrite(fileToConvert, sonar, inputNavFP, whipsNavFP);
            if(fileToConvert->outputFormat == XSONAR)
                convertXsonar(fileToConvert, sonar);
            break;
        case MIPS:
            if(fileToConvert->outputFormat == XSONAR)
                convertMips(fileToConvert, sonar, fileBuffer);
            break;
        default:
            break;
        }


    return;

}

