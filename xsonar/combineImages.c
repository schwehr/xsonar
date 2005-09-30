/*
 *    combineImages.c
 *
 *    Toggle board for selecting various image combinations to be applied to
 *    two input sonar files.  The output sonar file will contain the resulting
 *    combination of the the two input images.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    Jul 18, 1993
 */

#include <unistd.h>

#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>

#include "xsonar.h"

#include "sonar_struct.h"

void closeComboDialog(Widget w, XtPointer client_data, XtPointer callback_data);
int readImages(int fd1, int fd2, unsigned char *Image1, unsigned char *Image2, int columns, MainSonar *sonar);
int writeImages(int fd1, int output, unsigned char *Image1, int columns, MainSonar *sonar);
int outputName(MainSonar *sonar, char *filterType, int *output);
unsigned char *initializeImage(int columns, MainSonar *sonar);


void combineImages(Widget w, XtPointer client_data,  UNUSED XtPointer call_data)
{


    MainSonar *sonar = (MainSonar *) client_data;

    Widget comboDialog;
    Widget comboForm;
    Widget comboHolder;
    /*Widget separator1;*/
    Widget separator2;
    Widget separator3;
    Widget imageCombineLabel;

    Widget andToggle;
    Widget orToggle;
    Widget addToggle;
    Widget subtractToggle;
    Widget averageToggle;
    Widget flipToggle;

    Widget addbackScale;
    Widget image1WeightScale;
    Widget image2WeightScale;

    Widget buttonRowColumn;
    Widget doneButton;
    Widget cancelButton;
    Widget applyButton;

    Arg args[10];
    Status status;
    Cardinal n;

    int answer;
    int cancelCheck;
    int infd;

    char warningMessage[100];
    char inputFile[255];

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*void closeComboDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/

    /*
     *    Check the input file to make sure it exists and can be opened.
     *    If not, return.
     */

    if((infd = open(sonar->infile, O_RDWR)) == -1)
        {
        sprintf(warningMessage, "Error opening input sonar file.\nPlease select or reselect a file in Setup.");
        messages(sonar, warningMessage);
        return;
        }

    close(infd);

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the image combining function, and a push button
     *    row-column to apply or cancel the function.
     */

    XtSetSensitive(w, FALSE);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    comboDialog = XtCreatePopupShell("ComboDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    /*
     *     Create a form for various toggle widgets and the
     *     push buttons needed to close the dialog.
     */

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    comboForm = XtCreateWidget("ComboForm", xmFormWidgetClass,
                  comboDialog, args, n);
    XtManageChild(comboForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Functions for Combining Images:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    imageCombineLabel = XtCreateWidget("ImageCombineLabel",
                             xmLabelWidgetClass, comboForm, args, n);
    XtManageChild(imageCombineLabel);

    /*
     *   Create a row-column widget to hold the toggles used in this app.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, imageCombineLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    comboHolder = XtCreateWidget("ComboHolder",
                             xmRowColumnWidgetClass, comboForm, args, n);
    XtManageChild(comboHolder);

    /*
     *    Create the various toggles for each of the image combining
     *    functions.
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Add",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    addToggle = XtCreateWidget("AddToggle",
                     xmToggleButtonWidgetClass, comboHolder, args, n);
    XtManageChild(addToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Subtract",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    subtractToggle = XtCreateWidget("SubtractToggle",
                     xmToggleButtonWidgetClass, comboHolder, args, n);
    XtManageChild(subtractToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Average",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    averageToggle = XtCreateWidget("AverageToggle",
                     xmToggleButtonWidgetClass, comboHolder, args, n);
    XtManageChild(averageToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Invert",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    flipToggle = XtCreateWidget("FlipToggle",
                     xmToggleButtonWidgetClass, comboHolder, args, n);
    XtManageChild(flipToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("And (&)",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    andToggle = XtCreateWidget("AndToggle", xmToggleButtonWidgetClass,
                                              comboHolder, args, n);
    XtManageChild(andToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Or (|)",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    orToggle = XtCreateWidget("OrToggle", xmToggleButtonWidgetClass,
                                              comboHolder, args, n);
    XtManageChild(orToggle);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, comboHolder); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, comboForm, args, n);
    XtManageChild(separator2);

    /*
     *    Create sliders for the addback values to adjust filtered output,
     *    as well as sliders to input the weighting coefficients for
     *    each input image (used in the weighted combine function).
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Addback Value for Filters",
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 127); n++;
    XtSetArg(args[n], XmNminimum, -127); n++;
    XtSetArg(args[n], XmNvalue,    0); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    addbackScale = XtCreateWidget("AddbackScale",
                    xmScaleWidgetClass, comboForm, args, n);
    XtManageChild(addbackScale);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, addbackScale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Image 1 Weighting Coefficient",
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,    0); n++;
    XtSetArg(args[n], XmNdecimalPoints, 2); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    image1WeightScale = XtCreateWidget("Image1WeightScale",
                    xmScaleWidgetClass, comboForm, args, n);
    XtManageChild(image1WeightScale);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, image1WeightScale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Image 2 Weighting Coefficient",
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,    0); n++;
    XtSetArg(args[n], XmNdecimalPoints, 2); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    image2WeightScale = XtCreateWidget("Image2WeightScale",
                    xmScaleWidgetClass, comboForm, args, n);
    XtManageChild(image2WeightScale);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, image2WeightScale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator3 = XtCreateWidget("Separator3",
                   xmSeparatorWidgetClass, comboForm, args, n);
    XtManageChild(separator3);

    /*
     *   Create a menu for cancelling or ok'ing the function.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNadjustLast, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, comboForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Apply      ", XmSTRING_DEFAULT_CHARSET)); n++;
    applyButton = XtCreateWidget("ApplyButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(applyButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Cancel", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(cancelButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Done", XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneButton);

    XtAddCallback(doneButton, XmNactivateCallback, 
                                         closeComboDialog, &answer);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                         cancelCallback, NULL);
    XtAddCallback(applyButton, XmNactivateCallback, 
                                         File_select, sonar);
/*
    XtAddCallback(applyButton, XmNactivateCallback, 
                                         runFunction, sonar);
*/

    XtRealizeWidget(comboDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(comboDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(comboDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(comboDialog, args, n);

    XtPopup(comboDialog, XtGrabNone);

    answer = 0;
    cancelCheck = False;

    XtVaSetValues(cancelButton, XmNuserData, cancelCheck, NULL);
    XtVaSetValues(doneButton, XmNuserData, &answer, NULL);
    XtVaSetValues(applyButton, XmNuserData, inputFile, NULL);

    while(answer == 0)
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

    XtPopdown(comboDialog);
    XtDestroyWidget(comboDialog);

    if(answer == 2)
        status = 0;
    else
        status = 1;

    XtSetSensitive(w, True);

    return;


}

void runFunction(Widget w, MainSonar *sonar)
{

    PingHeader sonarHeader;

    Widget toggle;
    Widget doneButton;
    Widget parent_widget;
    Widget progressWindow;

    XmString toggleName;
    XmString compareName;

    GC progressGC;

    Dimension progressWidth;
    Dimension progressHeight;
    Dimension drawAmount;

    int i, j;
    int File1;
    int File2;
    int rows;
    int columns;
    int fileSize;
    int output;
    int tempInt;
    int addback;
    int infd;
    int slider1Value;
    int slider2Value;

    unsigned char *Image1;
    unsigned char *Image2;

    float percentDone;
    float image1coef;
    float image2coef;

    long filePosition;

    char *inputFile;
    char warningMessage[100];
    static char *toggles[] = {
          "*AndToggle",
          "*OrToggle",
          "*AddToggle",
          "*SubtractToggle",
          "*AverageToggle",
          "*WeightedToggle",
          "*FlipToggle",
          };


    XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*ComboDialog"));

    /*
     *  If the invert toggle is selected, invert the current image and
     *  return.
     */

    if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, "*FlipToggle")))
        {

        /*
         *    Check the input file to make sure it exists and can be opened.
         *        If not, return.
         */

        if((infd = open(sonar->infile, O_RDWR)) == -1)
            {
            sprintf(warningMessage, "Error opening input sonar file.\nPlease select or reselect a file in Setup.");
            messages(sonar, warningMessage);
            return;
            }


        flipImage(sonar, infd);
        close(infd);
        return;
        }

    /*
     *    Get the done button widget ID and set it to be insensitive
     *    while the filter application runs.
     */

    doneButton = XtNameToWidget(sonar->toplevel, "*ComboDialog*DoneButton");
    XtSetSensitive(doneButton, False);


    /*
     *    Get the name of the first input file.  This was set by adding
     *    the name to the resource "XmNuserData" of the ApplyButton widget.
     *    The new name selected is saved in sonar->infile (in the file select 
     *    dialog, see fileSelect.c), while inputFile contains the first 
     *    filename.
     */

    XtVaGetValues(w, XmNuserData, &inputFile, NULL);

    if((File1 = open(inputFile, O_RDONLY)) == -1)
        {
        sprintf(warningMessage, "Error opening first input sonar file.\nPlease select or reselect a file in Setup.");
        messages(sonar, warningMessage);
        XtSetSensitive(doneButton, True);
        return;
        }

    if((File2 = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningMessage, "Error opening second input sonar file.\n");
        messages(sonar, warningMessage);
        XtSetSensitive(doneButton, True);
        return;
        }

    if (read(File1, &sonarHeader, 256) != 256)
        {
        sprintf(warningMessage, "Cannot read sonar header from first file");
        messages(sonar, warningMessage);
        close(File1);
        XtSetSensitive(doneButton, True);
        return;
        }

    if (read(File2, &sonarHeader, 256) != 256)
        {
        sprintf(warningMessage, "Cannot read sonar header from second file");
        messages(sonar, warningMessage);
        close(File1);
        close(File2);
        XtSetSensitive(doneButton, True);
        return;
        }

    filePosition = lseek(File1, 0L, SEEK_SET);
    filePosition = lseek(File2, 0L, SEEK_SET);

    fileSize = getFileSize(sonar->infile);

    rows  = fileSize / sonarHeader.sdatasize;
    columns  = sonarHeader.sdatasize - HEADSIZE;

    Image1 = initializeImage(columns, sonar);
    Image2 = initializeImage(columns, sonar);

    /*
     *  Get the addback and weighting coefficients.  Only apply weighting
     *  coefficients if they are non-zero, otherwise use full value of
     *  pixel.
     */

    XmScaleGetValue(XtNameToWidget(sonar->toplevel,"*AddbackScale"), &addback); 
    XmScaleGetValue(XtNameToWidget(sonar->toplevel,
                                       "*Image1WeightScale"), &slider1Value); 
    XmScaleGetValue(XtNameToWidget(sonar->toplevel,
                                       "*Image2WeightScale"), &slider2Value); 

    image1coef = (float) slider1Value / 100.0;
    image2coef = (float) slider2Value / 100.0;

    if(image1coef == 0 && image2coef == 0)
        {
        image1coef = 0.5;
        image2coef = 0.5;
        }

    if(image1coef + image2coef != 1.0)
        {
        strcpy(sonar->infile, inputFile);
        sprintf(warningMessage, "Weighting values must sum to 1.0.\n \
                                 Please re-enter values using the sliders");
        messages(sonar, warningMessage);
        XtSetSensitive(doneButton, True);
        close(File1);
        close(File2);
        return;
        }

        

    /*
     *   Fill in progress monitor while reading the data.
     *   Also tell showProgress() which shell to pop up in.
     */

    progressWindow = setUpProgressWindow(sonar,"*ComboDialog", 
                      "Combining Images", &progressWidth, &progressHeight,
                      &progressGC);

    /*
     *  Cycle through the toggle names and find out which toggle was selected.
     */

    parent_widget = w;
    while(!XtIsTransientShell(parent_widget))
        parent_widget = XtParent(parent_widget);

    for(i = 0; i < 7; i++)
        {
        toggle = XtNameToWidget(parent_widget, toggles[i]);
        if(XmToggleButtonGetState(toggle))
            i = 6;

        }

    toggleName = XmStringCreateLtoR(XtName(toggle),
                                             XmSTRING_DEFAULT_CHARSET);

    compareName = XmStringCreateLtoR("AverageToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        if(outputName(sonar, "ave", &output))
            {

            for(i = 0; i < rows; i++)
                {
                percentDone = ((float)i / (float)rows);
                drawAmount = (Dimension) (percentDone * (float)progressWidth);
        
                XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

                XmUpdateDisplay(sonar->toplevel);

                if(!readImages(File1, File2, Image1, Image2, columns, sonar))
                    break;

                for(j = 0; j < columns; j++)
                    Image1[j] = (int) ((float)Image1[j] * image1coef + 
                                                 (float)Image2[j] * image2coef);

                if(!writeImages(File1, output, Image1, columns, sonar))
                    i = rows;
                }

            XtFree((char *)Image1);
            XtFree((char *)Image2);
            }
        }
    XmStringFree(compareName);

    compareName = XmStringCreateLtoR("SubtractToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        if(outputName(sonar, "sub", &output))
            {
            for(i = 0; i < rows; i++)
                {
                percentDone = ((float)i / (float)rows);
                drawAmount = (Dimension) (percentDone * (float)progressWidth);

                XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

                XmUpdateDisplay(sonar->toplevel);

                if(!readImages(File1, File2, Image1, Image2, columns, sonar))
                    break;

                for(j = 0; j < columns; j++)
                    {
                    tempInt = (int)Image1[j] - (int)Image2[j];
                    Image1[j] = (unsigned char) tempInt + addback;
                    }

                if(!writeImages(File1, output, Image1, columns, sonar))
                    i = rows;
                }

            XtFree((char *)Image1);
            XtFree((char *)Image2);
            }
        }
    XmStringFree(compareName);

    compareName = XmStringCreateLtoR("AddToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        if(outputName(sonar, "add", &output))
            {
            for(i = 0; i < rows; i++)
                {
                percentDone = ((float)i / (float)rows);
                drawAmount = (Dimension) (percentDone * (float)progressWidth);

                XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

                XmUpdateDisplay(sonar->toplevel);

                if(!readImages(File1, File2, Image1, Image2, columns, sonar))
                    break;

                for(j = 0; j < columns; j++)
                    {
                    tempInt = (int)Image1[j] + (int)Image2[j];
                    Image1[j] = (unsigned char) tempInt + addback;
                    }

                if(!writeImages(File1, output, Image1, columns, sonar))
                    i = rows;
                }

            XtFree((char *)Image1);
            XtFree((char *)Image2);
            }
        }

    XmStringFree(compareName);

    compareName = XmStringCreateLtoR("OrToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        if(outputName(sonar, "or", &output))
            {
            for(i = 0; i < rows; i++)
                {
                percentDone = ((float)i / (float)rows);
                drawAmount = (Dimension) (percentDone * (float)progressWidth);

                XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

                XmUpdateDisplay(sonar->toplevel);

                if(!readImages(File1, File2, Image1, Image2, columns, sonar))
                    break;

                for(j = 0; j < columns; j++)
                    {
                    tempInt = (int)Image1[j] | (int)Image2[j];
                    Image1[j] = (unsigned char) tempInt + addback;
                    }

                if(!writeImages(File1, output, Image1, columns, sonar))
                    i = rows;
                }

            XtFree((char *)Image1);
            XtFree((char *)Image2);
            }
        }

    XmStringFree(compareName);

    compareName = XmStringCreateLtoR("AndToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        if(outputName(sonar, "and", &output))
            {
            for(i = 0; i < rows; i++)
                {
                percentDone = ((float)i / (float)rows);
                drawAmount = (Dimension) (percentDone * (float)progressWidth);

                XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

                XmUpdateDisplay(sonar->toplevel);

                if(!readImages(File1, File2, Image1, Image2, columns, sonar))
                    break;

                for(j = 0; j < columns; j++)
                    {
                    tempInt = (int)Image1[j] & (int)Image2[j];
                    Image1[j] = (unsigned char) tempInt + addback;
                    }

                if(!writeImages(File1, output, Image1, columns, sonar))
                    i = rows;
                }

            XtFree((char *)Image1);
            XtFree((char *)Image2);
            }
        }

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));

    XmStringFree(compareName);
    XmStringFree(toggleName);

    XtSetSensitive(doneButton, True);

    strcpy(sonar->infile, inputFile);

    close(File1);
    close(File2);
    close(output);

    return;

}

/*void closeComboDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeComboDialog(Widget w, 
		       UNUSED  XtPointer client_data, 
		       UNUSED  XtPointer callback_data)
{

  /*MainSonar *sonar = (MainSonar *) client_data;*/
    int *answer;
    Widget parent_widget;

    parent_widget = w;

    XtVaGetValues(w, XmNuserData, &answer, NULL);

    while(!XtIsTransientShell(parent_widget))
         parent_widget = XtParent(parent_widget);

    if(w == XtNameToWidget(parent_widget, "*CancelButton"))
        *answer = 2;
    else
        *answer = 1;

    return;
}

unsigned char *initializeImage(int columns, MainSonar *sonar)
{

    char warningMessage[100];

    unsigned char *tempBuffer;

    /*void messages();*/

    if((tempBuffer = (unsigned char *) XtCalloc(columns, 
                              sizeof(unsigned char))) == NULL)
        {
        sprintf(warningMessage, "Error allocating image memory.");
        messages(sonar, warningMessage);
        return(0);
        }

    return tempBuffer;

}

int readImages(int fd1, int fd2, unsigned char *Image1, unsigned char *Image2, int columns, MainSonar *sonar)
{

    PingHeader sonarHeader;

    int inbytes;
    long filePosition;

    char warningMessage[100];

    /*void messages();*/

    if((inbytes = read(fd1, &sonarHeader, HEADSIZE)) != HEADSIZE)
        {
        sprintf(warningMessage, "Cannot read first file sonar header");
        messages(sonar, warningMessage);
        return 0;
        }

    if((inbytes = read(fd1, Image1, columns)) != columns)
        {
        sprintf(warningMessage, "Cannot read first file image array");
        messages(sonar, warningMessage);
        return 0;
        }

    filePosition = lseek(fd1, (long) ((columns + HEADSIZE) * -1), SEEK_CUR);

    if((inbytes = read(fd2, &sonarHeader, HEADSIZE)) != HEADSIZE)
        {
        sprintf(warningMessage, "Cannot read second file sonar header");
        messages(sonar, warningMessage);
        return 0;
        }

    if((inbytes = read(fd2, Image2, columns)) != columns)
        {
        sprintf(warningMessage, "Cannot read second image array");
        messages(sonar, warningMessage);
        return 0;
        }

    return 1;
}

int writeImages(int fd1, int output, unsigned char *Image1, int columns, MainSonar *sonar)
{

    PingHeader sonarHeader;

    int outbytes;
    int inbytes;

    long filePosition;

    char warningMessage[100];

    /*void messages();*/

    if((inbytes = read(fd1, &sonarHeader, HEADSIZE)) != HEADSIZE)
        {
        sprintf(warningMessage, "Cannot read first file sonar header");
        messages(sonar, warningMessage);
        return 0;
        }

    filePosition = lseek(fd1, (long)columns, SEEK_CUR);

    if((outbytes = write(output, &sonarHeader, HEADSIZE)) != HEADSIZE)
        {
        sprintf(warningMessage, "Cannot write sonar header from 1st file.");
        messages(sonar, warningMessage);
        return 0;
        }

    if((outbytes = write(output, Image1, columns)) != columns)
        {
        sprintf(warningMessage, "Cannot write first file image array");
        messages(sonar, warningMessage);
        return 0;
        }

    return 1;

}

int outputName(MainSonar *sonar, char *filterType, int *output)
{

    char *buffer;
    char *stringPtr;
    char warningMessage[100];

    /*void messages();*/

    buffer = (char *) XtCalloc(100, sizeof(char));

    strcpy(buffer, sonar->infile);

    if((stringPtr = strrchr(buffer, '/')) == NULL)
        stringPtr = buffer;
    else
        stringPtr++;

    while(*stringPtr != '.')
         stringPtr++;    /*  do nothing, just find the "." */

    *++stringPtr = '\0';
    strncat(buffer, filterType, strlen(filterType));

    if((*output = open(buffer, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningMessage, "Error opening output file");
        messages(sonar, warningMessage);
        XtFree((char *)buffer);
        return 0;
        }

    XtFree((char *)buffer);

    return 1;

}
