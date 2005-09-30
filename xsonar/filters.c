/* 
 *    filters.c
 *
 *    Toggle board for selecting various filter types to be applied to
 *    the input sonar file.  Basic routines taken from the book "C
 *    Language Algorithms for Digital Signal Processing" by Paul M. Embree
 *    and Bruce Kimble, 1991, Prentice Hall, Inc., ISBN 0-13-133406-9.
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
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>

#include "xsonar.h"

#include "sonar_struct.h"
#include "kernels.h"

void closeFilterDialog(Widget w, XtPointer client_data, XtPointer callback_data);

void filter(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget filterDialog;
    Widget filterForm;
    Widget filterHolder;
    Widget separator1;
    Widget separator2;
    Widget separator3;

    Widget kernelTypeLabel;
    Widget hiPassKernelToggle;
    Widget hiPassSubtractToggle;
    Widget lowPassToggle;
    Widget medianToggle;
    Widget edgeDetectToggle;
    Widget customToggle;

    Widget filterSizeRowColumn;
    Widget filterSizeLabel;
    Widget rowsLabel;
    Widget rowsText;
    Widget columnsLabel;
    Widget columnsText;

    Widget addbackScale;

    Widget buttonRowColumn;
    Widget doneFilterButton;
    Widget cancelButton;
    Widget applyButton;

    Arg args[20];
    Cardinal n;

    int answer;
    int cancelCheck;
    /*int fileSize;*/

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    char warningMessage[100];

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the filter kernel size, and a push button
     *    row-column to apply or cancel the filtering function.
     */

    XtSetSensitive(w, FALSE);

    /*
     *   Get the size of the file to be filtered (used to determine
     *   number of rows and columns in file).
     */

    if(!getFileSize(sonar->infile))
        {
        sprintf(warningMessage, "Could not determine file size for filter.\n");
        strcat(warningMessage, "Please select or reselect a file in Setup.");
        messages(sonar, warningMessage);
        XtSetSensitive(w, True);
        return;
        }

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    filterDialog = XtCreatePopupShell("FilterDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    filterForm = XtCreateWidget("FilterForm", xmFormWidgetClass,
                  filterDialog, args, n);
    XtManageChild(filterForm);

    /*
     *     Create a form for various toggle widgets and the
     *     push buttons needed to close the dialog.
     */
    
    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Filtering Kernels Available:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    kernelTypeLabel = XtCreateWidget("KernelTypeLabel",
                    xmLabelWidgetClass, filterForm, args, n);
    XtManageChild(kernelTypeLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, kernelTypeLabel); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    filterHolder = XtCreateWidget("FilterHolder",
                             xmRowColumnWidgetClass, filterForm, args, n);
    XtManageChild(filterHolder);

    /*
     *    Create the various toggles for each of the filter
     *     types.
     */

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Low Pass\nKernel",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    lowPassToggle = XtCreateWidget("LowPassToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(lowPassToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("High Pass\nKernel",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    hiPassKernelToggle = XtCreateWidget("HiPassKernelToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(hiPassKernelToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("High Pass\nSubtract",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    hiPassSubtractToggle = XtCreateWidget("HiPassSubtractToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(hiPassSubtractToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Median\nKernel",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    medianToggle = XtCreateWidget("MedianToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(medianToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Edge Detect\nKernel",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    edgeDetectToggle = XtCreateWidget("EdgeDetectToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(edgeDetectToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Custom User\nKernel",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    customToggle = XtCreateWidget("CustomToggle",
                xmToggleButtonWidgetClass, filterHolder, args, n);
    XtManageChild(customToggle);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, filterHolder); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, filterForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Size of Filter Kernel: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    filterSizeLabel = XtCreateWidget("FilterSizeLabel",
                             xmLabelWidgetClass, filterForm, args, n);
    XtManageChild(filterSizeLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, filterSizeLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    filterSizeRowColumn = XtCreateWidget("FilterSizeRowColumn",
                xmRowColumnWidgetClass, filterForm, args, n);
    XtManageChild(filterSizeRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Rows: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    rowsLabel = XtCreateWidget("RowsLabel", xmLabelWidgetClass,
                                     filterSizeRowColumn, args, n);
    XtManageChild(rowsLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 3); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    rowsText = XtCreateWidget("RowsText", xmTextWidgetClass, 
                                     filterSizeRowColumn, args, n);
    XtManageChild(rowsText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Columns: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    columnsLabel = XtCreateWidget("ColumnsLabel", xmLabelWidgetClass,
                                     filterSizeRowColumn, args, n);
    XtManageChild(columnsLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 3); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    columnsText = XtCreateWidget("ColumnsText", xmTextWidgetClass,
                                     filterSizeRowColumn, args, n);
    XtManageChild(columnsText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, filterSizeRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, filterForm, args, n);
    XtManageChild(separator2);

    /*
     *    Create a slider for the addback values to adjust filtered output.
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
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    addbackScale = XtCreateWidget("AddbackScale",
                    xmScaleWidgetClass, filterForm, args, n);
    XtManageChild(addbackScale);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, addbackScale); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator3 = XtCreateWidget("Separator3",
                   xmSeparatorWidgetClass, filterForm, args, n);
    XtManageChild(separator3);

    /*
     *   Create a menu for cancelling or ok'ing the filter selection.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, filterForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Apply      ", XmSTRING_DEFAULT_CHARSET)); n++;
    applyButton = XtCreateWidget("    ApplyButton    ",
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
    doneFilterButton = XtCreateWidget("DoneFilterButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneFilterButton);

    XtAddCallback(doneFilterButton, XmNactivateCallback,
                                     closeFilterDialog, NULL);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                      cancelCallback, NULL);
    XtAddCallback(applyButton, XmNactivateCallback, 
                                     runFilter, sonar);

    XtRealizeWidget(filterDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(filterDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(filterDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(filterDialog, args, n);

    XtPopup(filterDialog, XtGrabNone);

    answer = 0;
    cancelCheck = False;

    XtVaSetValues(cancelButton, XmNuserData, cancelCheck, NULL);

    while(answer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(doneFilterButton, XmNuserData, &answer, NULL);
        }

    XtPopdown(filterDialog);
    XtDestroyWidget(filterDialog);

    XtSetSensitive(w, True);

    return;


}

void runFilter(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    PingHeader sonarHeader;

    Widget parent_widget;
    Widget toggle;
    Widget cancelButton;
    Widget doneButton;
    /*Widget dialog;*/
    Widget progressWindow;

    Arg args[20];

    Cardinal n;

    /*XEvent nextEvent;*/

    GC progressGC;

    Dimension progressWidth;
    Dimension progressHeight;
    Dimension drawAmount;

    /*char messageBuffer[40];*/

    XmString compareName;
    XmString toggleName;

    int i/*, j*/;
    int fileSize;
    int inbytes;
    int outbytes;
    int rowCenter, columnCenter;
    int cancelCheck;
    int addBack;
    int infd;
    int outfd; /* FIX: can be used uninitialized */

    float percentDone;

    /*double kernelLength;*/

    long position;

    Matrix output;
    Matrix input;
    Matrix filter;

    char warningMessage[100];
    char type[100];
    static char *buttons[] = {
          "*LowPassToggle",
          "*HiPassSubtractToggle",
          "*HiPassKernelToggle",
          "*MedianToggle",
          "*EdgeDetectToggle",
          "*CustomToggle",
          };

    /*
     *  Get and check the size of the filter kernel.  Must be greater than
     *  0 and have an odd value.
     */

    filter.columns = atoi(XmTextGetString(XtNameToWidget(sonar->toplevel,
                                                         "*ColumnsText")));
    filter.rows = atoi(XmTextGetString(XtNameToWidget(sonar->toplevel,
                                                         "*RowsText")));

    if(!filter.rows || !filter.columns)
        {
        sprintf(warningMessage,"Must select the kernel\nsize to apply filter.");
        messages(sonar, warningMessage);
        return;
        }

    if(!(filter.rows % 2) || !(filter.columns % 2))
        {
        sprintf(warningMessage, "Each kernel size must be an odd value.\nPlease check and reselect the kernel values");
        messages(sonar, warningMessage);
        return;
        }

    /*
     *    Check the input file to make sure it exists and can be opened.
     */

    if((infd = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningMessage, "Error opening input sonar file.\nPlease select or reselect a file in Setup.");
        messages(sonar, warningMessage);
        return;
        }


    /*
     *    Get the done button widget ID and set it to be insensitive
     *    as well as the calling button (apply) while the filter
     *    application runs.
     */

    doneButton = XtNameToWidget(sonar->toplevel, "*FilterDialog*DoneFilterButton");
    XtSetSensitive(doneButton, False);
    XtSetSensitive(w, False);

    position = lseek(infd, 0L, SEEK_SET);

    if (read (infd, &sonarHeader, 256) != 256)
        {
        sprintf(warningMessage, "Cannot read sonar header");
        messages(sonar, warningMessage);
        close(infd);
        return;
        }

    position = lseek(infd, 0L, SEEK_SET);

    fileSize = getFileSize(sonar->infile);

    input.rows = output.rows = fileSize / sonarHeader.sdatasize;
    input.columns = output.columns = sonarHeader.sdatasize - HEADSIZE;
    input.elementSize = output.elementSize = sizeof(short);


    allocateMatrix(&input);
    allocateMatrix(&output);

    /*
     *   Fill in progress monitor while reading the data.
     *   Also tell showProgress() which shell to pop up in.
     */

    progressWindow = setUpProgressWindow(sonar, "*FilterDialog",
                      "Reading Data ...", &progressWidth, &progressHeight,
                      &progressGC);

   /*
    *  Read the input matrix.
    */

    for(i = 0 ; i < (int)input.rows ; i++)
        {
        percentDone = ((float)i / (float)input.rows) / 3;
        drawAmount = (Dimension) (percentDone * (float)progressWidth);

        XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

        XmUpdateDisplay(sonar->toplevel);


        if((inbytes = read(infd, &sonarHeader, HEADSIZE)) != HEADSIZE)
            fprintf(stderr, "Error reading sonar header # %d\n", i);

        if((inbytes = read(infd, input.elements[i],
                           input.columns)) != (int)input.columns)
            fprintf(stderr, "Error reading original record # %d\n", i);
        }

    position = lseek(infd, 0L, SEEK_SET);

    /*
     *  Find out which filter widget has been called
     */

    parent_widget = w;
    while(!XtIsTransientShell(parent_widget))
        parent_widget = XtParent(parent_widget);

    for(i = 0; i < 6; i++)
        {
        toggle = XtNameToWidget(parent_widget, buttons[i]);
        if(XmToggleButtonGetState(toggle))
            i = 6;

        }

    /*
     *  Get the addback value to scale the filtered output.
     */

    XmScaleGetValue(XtNameToWidget(sonar->toplevel, "*AddbackScale"), &addBack);

    /*
     *  Cycle through the toggle names and initialize filter kernel
     *  based on which toggle was pressed.
     */

    toggleName = XmStringCreateLtoR(XtName(toggle),
                                             XmSTRING_DEFAULT_CHARSET);

    compareName = XmStringCreateLtoR("LowPassToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        strcpy(type, "lpf");
        initializeFilter(sonar, &filter, type, 1);
        outfd = outputFile(sonar, type);
        if(outfd == -1)
            return;
        XtSetSensitive(doneButton, False);
        XtSetSensitive(w, False);

        }

    compareName = XmStringCreateLtoR("HiPassKernelToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        strcpy(type, "hpfk");
        initializeFilter(sonar, &filter, type, -1);
        rowCenter = (filter.rows - 1) / 2;
        columnCenter = (filter.columns - 1) / 2;
        filter.elements[rowCenter][columnCenter] = filter.rows * filter.columns;
        outfd = outputFile(sonar, type);
        if(outfd == -1)
            return;
        XtSetSensitive(doneButton, False);
        XtSetSensitive(w, False);
        }

    compareName = XmStringCreateLtoR("HiPassSubtractToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        strcpy(type, "hpfs");
        initializeFilter(sonar, &filter, type, 1);
        outfd = outputFile(sonar, type);
        if(outfd == -1)
            return;
        XtSetSensitive(doneButton, False);
        XtSetSensitive(w, False);
        }

    compareName = XmStringCreateLtoR("MedianToggle",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(toggleName, compareName))
        {
        strcpy(type, "med");
        initializeFilter(sonar, &filter, type, 1);
        outfd = outputFile(sonar, type);
        if(outfd == -1)
            return;
        XtSetSensitive(doneButton, False);
        XtSetSensitive(w, False);
        }

   /*
    *   Initiate the filtering process.
    */

    filter2d(&input, &output, &filter, type, sonar, addBack, progressGC);

   /*
    *   Write each row of the filtered data if the user has not cancelled
    *   the filtering operation.
    */

    cancelButton = XtNameToWidget(sonar->toplevel, "*CancelButton");
    XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Writing Data ...",
              XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"), args, n);

    if(!cancelCheck)
        {
        for(i = 0 ; i < (int)output.rows ; i++)
            {
            percentDone = ((float)i / (float)output.rows) / 3 + 0.666;
            drawAmount = (Dimension) (percentDone * (float)progressWidth);

            XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                   progressGC, drawAmount, 0, drawAmount, progressHeight);

            XmUpdateDisplay(sonar->toplevel);

            if((inbytes = read(infd, &sonarHeader, HEADSIZE)) != HEADSIZE)
                fprintf(stderr, "Error reading sonar header # %d\n", i);

            position = lseek(infd, (long) input.columns, SEEK_CUR);

            if((inbytes = write(outfd, &sonarHeader, HEADSIZE)) != HEADSIZE)
                fprintf(stderr, "Error writing sonar header # %d\n", i);

            if((outbytes = write(outfd, output.elements[i],
                               output.columns)) != (int)output.columns)
                fprintf(stderr, "Error writing filtered record # %d\n", i);
            }
        }

    /*
     *  Free up the XmString buffers allocated, deallocate the input and
     *  output matrices, popdown the progress monitor, and close the output 
     *  file.
     */

    XFreeGC(XtDisplay(sonar->toplevel), progressGC);

    XtPopdown(XtNameToWidget(sonar->toplevel, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(sonar->toplevel, "*StatusDialog"));

    XmStringFree(compareName);
    XmStringFree(toggleName);

    deAllocateMatrix(&input);
    deAllocateMatrix(&output);
    deAllocateMatrix(&filter);

    close(infd);
    close(outfd);

    /*
     *  Reset the done button to sensitive.
     */

    XtSetSensitive(doneButton, True);
    XtSetSensitive(w, True);
    cancelCheck = False;
    XtVaSetValues(cancelButton, XmNuserData, cancelCheck, NULL);

    return;


}


/*void closeFilterDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeFilterDialog(Widget w,
		        UNUSED  XtPointer client_data,
		        UNUSED  XtPointer callback_data)
{

    int answer;

    answer = 1;
    XtVaSetValues(w, XmNuserData, answer, NULL);

    return;
}

int outputFile(MainSonar *sonar, char *filterType)
{

    char *buffer;
    char *stringPtr;
    char warningMessage[100];

    int outfd;

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

    if((outfd = open(buffer, O_RDWR | O_CREAT | O_TRUNC, PMODE)) < 0)
        {
        sprintf(warningMessage, "Error opening output file");
        messages(sonar, warningMessage);
        XtFree(buffer);
        return(outfd);
        }

    XtFree(buffer);

    return(outfd);
}

void initializeFilter( UNUSED  MainSonar *sonar,
		       Matrix *filter,
		        UNUSED  char *type,
		       int kernelNumber)
{
    int i, j;

    filter->elementSize = sizeof(short);

    allocateMatrix(filter);

    for(i = 0; i < (int)filter->rows; i++)
         for(j = 0; j < (int)filter->columns; j++)
              filter->elements[i][j] = kernelNumber;

    return;

}
