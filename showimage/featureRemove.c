#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
 
#include "showimage2.h"
#include "externals.h"

void featureRemove(w, clientData, callData)
Widget       w;
XtPointer    clientData;
XtPointer    callData;
{


    struct draw *drawarea = (struct draw * ) clientData;
 
    Arg args[40];
    Cardinal n;

    /*XColor grayScale[256];*/

    /*Colormap defaultColormap;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    Widget featureRemoveDialog;
    /*Widget featureRemovebutton;*/
    Widget featureRemoveForm;
    Widget featureRemoveRowColumn1;
    Widget buttonRowColumn;
    Widget filterWidthLabel;
    Widget filterWidthText;
    Widget filterHeightLabel;
    Widget filterHeightText;
    Widget filterLabel;
    Widget pixelReplaceLabel;
    Widget pixelReplaceText;
    Widget doneButton;
    Widget cancelButton;
    Widget separator1;

    Widget zoomDialog;

    int answer;
    /*int pixelLocation;*/
    /*int i;*/


    /*char *temp;*/

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    void closeFeatureRemoveInput();
    void checkEvents();

    void message_display();

    answer = 0;

    if(fp1 == -1 || fp1 == EOF)
        {
        message_display(drawarea, fp1);
        return;
        }

    XmUpdateDisplay(drawarea->shell);

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    featureRemoveDialog = XtCreatePopupShell("FeatureRemoveDialog",
            transientShellWidgetClass, zoomDialog, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    featureRemoveForm = XtCreateManagedWidget("FeatureRemoveForm",
                xmFormWidgetClass, featureRemoveDialog, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Pixel Replacement Parameters:",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    filterLabel = XtCreateWidget("FilterLabel",
        xmLabelWidgetClass, featureRemoveForm, args, n);
    XtManageChild(filterLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, filterLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 4); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    featureRemoveRowColumn1 = XtCreateWidget("FeatureRemoveRowColumn1",
                xmRowColumnWidgetClass, featureRemoveForm, args, n);
    XtManageChild(featureRemoveRowColumn1);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Width: ",
        XmSTRING_DEFAULT_CHARSET)); n++;
    filterWidthLabel = XtCreateWidget("FilterWidthLabel",
        xmLabelWidgetClass, featureRemoveRowColumn1, args, n);
    XtManageChild(filterWidthLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    filterWidthText = XtCreateWidget("FilterWidthText", xmTextFieldWidgetClass,
                                             featureRemoveRowColumn1, args, n);
    XtManageChild(filterWidthText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Height: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    filterHeightLabel = XtCreateWidget("FilterHeightLabel",
         xmLabelWidgetClass, featureRemoveRowColumn1, args, n);
    XtManageChild(filterHeightLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    filterHeightText = XtCreateWidget("FilterHeightText",xmTextFieldWidgetClass,
                                       featureRemoveRowColumn1, args, n);
    XtManageChild(filterHeightText);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Number of pixels to replace: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    pixelReplaceLabel = XtCreateWidget("PixelReplaceLabel",
         xmLabelWidgetClass, featureRemoveRowColumn1, args, n);
    XtManageChild(pixelReplaceLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    pixelReplaceText = XtCreateWidget("PixelReplaceText",
                     xmTextFieldWidgetClass, featureRemoveRowColumn1, args, n);
    XtManageChild(pixelReplaceText);
    XmTextFieldSetString(pixelReplaceText, "1");

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, featureRemoveRowColumn1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                    xmSeparatorWidgetClass, featureRemoveForm, args, n);
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
                xmRowColumnWidgetClass, featureRemoveForm, args, n);
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
                                checkEvents, drawarea);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                closeFeatureRemoveInput, drawarea);

    XtRealizeWidget(featureRemoveDialog);

    XtVaGetValues(zoomDialog, XmNx, &shellx, NULL);
    XtVaGetValues(zoomDialog, XmNy, &shelly, NULL);
    XtVaGetValues(zoomDialog, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(zoomDialog, XmNheight, &shellHeight, NULL);
    XtVaGetValues(featureRemoveDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(featureRemoveDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx; 
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(featureRemoveDialog, args, n);

   /*
    *    Create a colormap entry for this drawarea->shell to match the grayscale
    *    colormap of the main drawarea->shell.  This will keep the display from
    *    blinking if the lower entries of the colormap are filled by
    *    another application.
    */

    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(featureRemoveDialog),
                                      main_window_valuemask, &main_window_attr);

    XtPopup(featureRemoveDialog, XtGrabNone);

    XtVaSetValues(doneButton, XmNuserData, answer, NULL);

    while(answer == 0)
         {
         XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);
         XtVaGetValues(doneButton, XmNuserData, &answer, NULL);
         }
/*
    if(answer == 1)
        {
        XtVaGetValues(drawarea->graphics, XmNuserData, &pixelLocation, NULL);
 fprintf(stdout, "button 1 clicked on pixel # %d\n", pixelLocation);
        }
*/

    XtDestroyWidget(featureRemoveDialog);

    return;


}                

void closeFeatureRemoveInput(w,client_data,callback_data)
Widget         w;
XtPointer      client_data;
XtPointer      callback_data;
{

    struct draw *drawarea = (struct draw * ) client_data;
    Widget doneButton;

    doneButton = XtNameToWidget(drawarea->shell, "*ZoomDialog*DoneButton");

    XtVaSetValues(doneButton, XmNuserData, 1, NULL);

    XtPopdown(XtNameToWidget(drawarea->shell, "*FeatureRemoveDialog"));

    return;

}

void checkEvents(w, clientData, callData)
Widget     w;
XtPointer  clientData;
XtPointer  callData;
{

    struct draw *drawarea = (struct draw * ) clientData;
 
    XEvent next_event;

    GC rubberGC;               /* the graphics context for rubber banding*/
    XGCValues the_GC_values;   /* for handling the GC values */

    Arg args[40];
    Cardinal n;

    XFontStruct *label_font;

    Widget zoomArea;
    Widget zoomDialog;

    char messageBuf[100];

    int status;
    int checkingEvents;
    int pixelLocation;
    int pixelValue;
    int width, height;
    int pixelsToReplace;
    int windowWidth, windowHeight;
    int lastx;
    int displayDataPad;

    float metersPerPix;

    void featureDelete();
    void message_display();
    int  checkRemovalStatus();

    width = atoi(XmTextFieldGetString(XtNameToWidget(drawarea->shell,
                                 "*FeatureRemoveDialog*FilterWidthText")));
    height = atoi(XmTextFieldGetString(XtNameToWidget(drawarea->shell,
                                 "*FeatureRemoveDialog*FilterHeightText")));

    pixelsToReplace = atoi(XmTextFieldGetString(XtNameToWidget(drawarea->shell,
                                 "*FeatureRemoveDialog*PixelReplaceText")));

    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");
    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");

    /*
     *    Pop the dialog down as the user has entered the width and
     *    height of the filter.  Refresh the screen as popping down
     *    the window does not generate an expose event.
     */

    XtPopdown(XtNameToWidget(drawarea->shell, "*FeatureRemoveDialog"));

    status = XGrabPointer(XtDisplay(drawarea->shell), XtWindow(zoomArea),FALSE,
        ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
        GrabModeAsync, GrabModeAsync, XtWindow(zoomArea),
        XCreateFontCursor(XtDisplay(zoomArea), XC_dotbox),
        CurrentTime);

    checkingEvents = True;

    windowWidth = ((drawarea->zoom_x - drawarea->base_x) + 1) * 
                                     data_reducer * drawarea->magnify;
    windowHeight = ((drawarea->zoom_y - drawarea->base_y) + 1) *
                                     data_reducer * drawarea->magnify;

    /*
     *    Create a rubber band gc to draw the line location in the main
     *    display of the pixel selected for replacement
     */

    n = 0;
    XtSetArg(args[n], XmNforeground, &the_GC_values.foreground); n++;
    XtSetArg(args[n], XmNbackground, &the_GC_values.background); n++;
    XtGetValues(w, args, n);
    the_GC_values.function = GXxor;

    rubberGC = XCreateGC(XtDisplay(drawarea->shell),XtWindow(drawarea->graphics),
                       GCFunction |GCForeground| GCBackground, &the_GC_values);

    XSetForeground(XtDisplay(drawarea->shell), rubberGC, 192);
    XSetBackground(XtDisplay(drawarea->shell), rubberGC, 255);

    XSetLineAttributes(XtDisplay(w),rubberGC, 2, 
                                 LineSolid, CapButt, JoinRound);

    label_font = XLoadQueryFont(XtDisplay(drawarea->graphics),
            "-adobe-helvetica-*-r-*-*-*-100-*-*-*-*-*-*");

    displayDataPad = (512 - datasize / data_reducer) / 2;

    while(checkingEvents)
        {
        XNextEvent(XtDisplay(zoomArea),&next_event);

        switch(next_event.type)
            {
            case  ButtonPress:
                lastx = next_event.xbutton.x / 
                                      (drawarea->magnify * data_reducer);
                if(drawarea->magnify < 2)
                    pixelValue = XGetPixel(subimage, next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;
                else
                    pixelValue = XGetPixel(magnified_image,
                                          next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;

                metersPerPix = (float) swath_width / datasize * data_reducer;

                sprintf(messageBuf,
                     "X = %.3d  Y = %.3d DN = %.3d Range = %5.1f",
                      (next_event.xbutton.x / (drawarea->magnify * data_reducer)
                      + drawarea->base_x - displayDataPad) * data_reducer,
                     next_event.xbutton.y,
                     (int) ((float)pixelValue / 63.0 * 255.0),
                     (float) abs(next_event.xbutton.x / 
                                    (drawarea->magnify * data_reducer) +
                                    drawarea->base_x - 256) * metersPerPix);

                XDrawImageString(XtDisplay(zoomArea),
                     XtWindow(zoomArea),drawarea->zoom_gc, 0, windowHeight - 1,
                     messageBuf, strlen(messageBuf));

                XDrawLine(XtDisplay(drawarea->shell),XtWindow(drawarea->graphics),
                   rubberGC,
                    next_event.xbutton.x / (drawarea->magnify *
                    data_reducer) + drawarea->base_x, 0,
                    next_event.xbutton.x / (drawarea->magnify *
                    data_reducer) + drawarea->base_x, 512);

/*
                if (next_event.xbutton.button == Button1)
                    {
                    sprintf(messageBuf, "FEATURE REMOVAL");

                    XDrawImageString(XtDisplay(zoomArea),
                         XtWindow(zoomArea), drawarea->zoom_gc, 0, 
                         label_font->ascent + label_font->descent, 
                         messageBuf, strlen(messageBuf));
                    }
*/


                break;

            case  ButtonRelease:

                XDrawLine(XtDisplay(drawarea->shell),XtWindow(drawarea->graphics),
                        rubberGC, 
                        lastx  + drawarea->base_x, 0,
                        lastx  + drawarea->base_x, 512);

                XCopyArea(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                        XtWindow(zoomArea), drawarea->zoom_gc,
                        0, 0, windowWidth, windowHeight, 0, 0);

                if (next_event.xbutton.button == Button1)
                    {

                    pixelLocation = (next_event.xbutton.x / 
                           (drawarea->magnify * data_reducer)
                           + drawarea->base_x - displayDataPad) * data_reducer;

                    XUngrabPointer(XtDisplay(zoomArea), CurrentTime);

                    if(drawarea->magnify < 2)
                        pixelValue = XGetPixel(subimage, next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;
                    else
                        pixelValue = XGetPixel(magnified_image,
                                          next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;

                    status = checkRemovalStatus(drawarea, pixelValue,
                                                             pixelLocation);

                    if(status == 1)
                        featureDelete(w, width, height, drawarea, 
                                      pixelLocation, pixelsToReplace);

                    XFreeGC(XtDisplay(drawarea->shell), rubberGC);

                    XmUpdateDisplay(drawarea->shell);
                    XmUpdateDisplay(zoomDialog);

                    checkingEvents = False;

                    }
                break;

            case  Expose:
                XCopyArea(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                        XtWindow(zoomArea), drawarea->zoom_gc,
                        0, 0, windowWidth, windowHeight, 0, 0);
                XmUpdateDisplay(drawarea->shell);
                XmUpdateDisplay(zoomDialog);
                break;

            case MotionNotify:

                if(drawarea->magnify < 2)
                    pixelValue = XGetPixel(subimage, next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;
                else
                    pixelValue = XGetPixel(magnified_image,
                                          next_event.xbutton.x,
                                          next_event.xbutton.y) - 192;

                metersPerPix = (float) swath_width / datasize * data_reducer;

                sprintf(messageBuf,
                     "X = %.3d  Y = %.3d DN = %.3d Range = %5.1f",
                      (next_event.xbutton.x / (drawarea->magnify * data_reducer)
                      + drawarea->base_x - displayDataPad) * data_reducer,
                     next_event.xbutton.y,
                     (int) ((float)pixelValue / 63.0 * 255.0),
                     (float) abs(next_event.xbutton.x / 
                                    (drawarea->magnify * data_reducer) + 
                                    drawarea->base_x - 256) * metersPerPix);

                XDrawImageString(XtDisplay(zoomArea),
                     XtWindow(zoomArea),drawarea->zoom_gc, 0, windowHeight - 1,
                     messageBuf, strlen(messageBuf));
/*
                if (next_event.xmotion.state == Button1Mask)
                    {
                    sprintf(messageBuf, "FEATURE REMOVAL");

                    XDrawImageString(XtDisplay(zoomArea),
                         XtWindow(zoomArea), drawarea->zoom_gc, 0, 
                         label_font->ascent + label_font->descent, 
                         messageBuf, strlen(messageBuf));
                    }
*/

                XDrawLine(XtDisplay(drawarea->shell),XtWindow(drawarea->graphics),
                    rubberGC, lastx + drawarea->base_x, 0, 
                              lastx + drawarea->base_x, 512);

                XDrawLine(XtDisplay(drawarea->shell),XtWindow(drawarea->graphics),
                    rubberGC, 
                    next_event.xbutton.x / (drawarea->magnify * 
                    data_reducer) + drawarea->base_x, 0,
                    next_event.xbutton.x / (drawarea->magnify * 
                    data_reducer) + drawarea->base_x, 512);

                lastx = next_event.xbutton.x / 
                                    (drawarea->magnify * data_reducer);

                break;

            default:
                break;
            }


        }

    XtVaSetValues(w, XmNuserData, 1, NULL);

    return;

}

void featureDelete(w, width, height, drawarea, pixelLocation, pixReplace)
Widget w;
int width;
int height;
struct draw *drawarea;
int pixelLocation;
int pixReplace;
{

    Widget progressWindow;
    GC progressGC;

    unsigned char *medianBuffer;
    unsigned char *medianValue;

    unsigned char *initialValue;
    unsigned char *endValue;

    long currentFilePosition;
    long initialPixel;
    long selectedPixel;
    long filterPixel;
    long replacePixel;

    int i, j;
    /*int currentPixel;*/
    int inbytes;
    int outbytes;
    int scan;
    int position;
    int fileSize;
    int fileStatus;


    float percentDone;
    float stepValue;

    Dimension progressWindowWidth;
    Dimension progressWindowHeight;
    Dimension drawAmount;

    Widget zoomDialog;

    char messageBuffer[40];

    unsigned char c_median();

    void showProgress();

    struct stat fileParams;

    /*
     *   Find file size for filling in progress monitor while processing
     *   the data.
     */


    if((fileStatus = fstat(fp1, &fileParams)) == -1)
         {
         fprintf(stdout, "Cannot read data file size");
         return;
         }

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");

    fileSize = fileParams.st_size;

    currentFilePosition = lseek(fp1, 
                           (long) (drawarea->top_ping * scansize), SEEK_SET);

    medianBuffer = (unsigned char *) XtCalloc(1, width * height);
    medianValue = (unsigned char *) XtCalloc(1, 1);
    initialValue = (unsigned char *) XtCalloc(1, 1);
    endValue = (unsigned char *) XtCalloc(1, 1);

    selectedPixel = pixelLocation + headsize;
    initialPixel = (long) (pixelLocation - ((pixReplace - 1) / 2) + headsize);
    position = lseek(fp1, (long) initialPixel, SEEK_SET);

    inbytes = 1;
    scan = 0;

    /*
     *   Fill in progress monitor while reading the data.
     *   Also tell showProgress() which drawarea->shell to pop up in.
     */

    sprintf(messageBuffer, "Filtering data...");
    showProgress(drawarea, zoomDialog, messageBuffer);

    progressWindow = XtNameToWidget(drawarea->shell, "*StatusDialog*DrawProgress");
    XtVaGetValues(progressWindow, XmNwidth, &progressWindowWidth, NULL);
    XtVaGetValues(progressWindow, XmNheight, &progressWindowHeight, NULL);

    progressGC = XCreateGC(XtDisplay(drawarea->shell),
                              XtWindow(progressWindow),None,NULL);

    XSetForeground(XtDisplay(drawarea->shell), progressGC,
           WhitePixelOfScreen(XtScreen(drawarea->shell)));

    XSetBackground(XtDisplay(drawarea->shell), progressGC,
           BlackPixelOfScreen(XtScreen(drawarea->shell)));

    XmUpdateDisplay(drawarea->shell);

    while(inbytes != 0)
        {

        filterPixel = initialPixel;

        for(i = 0; i < height; i++)
            {
            for(j = 0; j < width; j++)
                {
                if(j == (width - 1) / 2)
                    {
                    medianBuffer[j] = medianBuffer[0];
                    j++;
                    }

                position = lseek(fp1, (long) (filterPixel + j), SEEK_SET);

                if((inbytes = read(fp1, &medianBuffer[i * width + j], 1)) != 1)
                    {
                    i = height; 
                    j = width;
                    }
                }
            filterPixel += scansize;
            }

        position = lseek(fp1, (long) (filterPixel), SEEK_SET);
        if((inbytes = read(fp1, initialValue, 1)) == 1)
            {
            position = lseek(fp1, (long)
                                  (filterPixel + pixReplace - 1), SEEK_SET);
            inbytes = read(fp1, endValue, 1);

            *medianValue = c_median(medianBuffer, height * width);

            replacePixel = selectedPixel - pixReplace / 2;

            if(*endValue > *initialValue)
                stepValue = (float)(*endValue - *initialValue) / 
                                                            (float)pixReplace;
            else
                stepValue = (float)(*initialValue - *endValue) / 
                                                            (float)pixReplace;
/*
fprintf(stdout, "stepValue = %f\n", stepValue);
fprintf(stdout, "initialValue = %d\n", *initialValue);
fprintf(stdout, "endValue = %d\n", *endValue);
fprintf(stdout, "medianValue = %d\n", *medianValue);
*/

            for(i = 0; i < pixReplace; i++)
                {
                position = lseek(fp1, replacePixel + i, SEEK_SET);
/*
                if(*endValue > *initialValue)
                    *medianValue = (unsigned char) 
                                ((float) *initialValue + (i * stepValue) + 0.5);
                else
                    *medianValue = (unsigned char) 
                                ((float) *initialValue - (i * stepValue) + 0.5);
*/


                outbytes = write(fp1, medianValue, 1); 
                }


            initialPixel += scansize;
            selectedPixel += scansize;
    
            percentDone = (float)(scan * scansize) / (float)fileSize;
            drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

/*
fprintf(stdout, "percentDone = %f\n", percentDone);
fprintf(stdout, "draw amount = %d\n", drawAmount);
                fprintf(stdout, "medianBuffer[%d] = %d\n", i * width + j,medianBuffer[i * width + j]);
                if(scan > 300 && !(scan %100))
                fprintf(stdout, "medianvalue = %d\n",*medianValue);
*/

            XDrawLine(XtDisplay(progressWindow), XtWindow(progressWindow), 
                   progressGC, drawAmount, 0, drawAmount, progressWindowHeight);

            XmUpdateDisplay(drawarea->shell);
            }

        scan++;

        }

    position = lseek(fp1, currentFilePosition, SEEK_SET);

    XtFree((char *)medianBuffer);
    XtFree((char *)medianValue);
    XtFree((char *)initialValue);
    XtFree((char *)endValue);

    XFreeGC(XtDisplay(drawarea->shell), progressGC);

    XtPopdown(XtNameToWidget(drawarea->shell, "*StatusDialog"));
    XtDestroyWidget(XtNameToWidget(drawarea->shell, "*StatusDialog"));

    return;

}


int checkRemovalStatus(drawarea, pixVal, pixLocation)
struct draw *drawarea;
int pixVal;
int pixLocation;
{


    Widget checkDialog;
    Widget checkBox;
    Widget checkButton;
    Widget zoomDialog;

    int answer;
    int status;
    /*int i;*/

    Arg args[40];
    Cardinal n;

    /*XColor grayScale[256];*/

    /*Colormap defaultColormap;*/

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    char messageBuf[255];

    void closeCheck();

    answer = 0;

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");

    n = 0;
    XtSetArg(args[n], XmNtitle, "Feature Removal"); n++;
    checkDialog = XtCreatePopupShell("CheckDialog",
            xmDialogShellWidgetClass, zoomDialog, args, n);

    sprintf(messageBuf, "Pixel location is: %03d, DN value is: %03d",
                           pixLocation, (int) ((float)pixVal / 63.0 * 255.0));
    strcat(messageBuf, "\nOk to filter and replace selected pixels?");

    n = 0;
    XtSetArg(args[n], XmNmessageString, 
         XmStringCreateLtoR(messageBuf, XmSTRING_DEFAULT_CHARSET)); n++;
    checkBox = XtCreateWidget("CheckBox", xmMessageBoxWidgetClass, 
                                                     checkDialog, args, n);

    checkButton = XmMessageBoxGetChild(checkBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(checkButton);

    XtAddCallback(checkBox, XmNcancelCallback, 
                                         closeCheck, &answer);
    XtAddCallback(checkBox, XmNokCallback, 
                                         closeCheck, &answer);


    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    XChangeWindowAttributes(XtDisplay(drawarea->shell), XtWindow(checkDialog),
                                     main_window_valuemask, &main_window_attr);
    XtManageChild(checkBox);

    while(answer == 0)
        XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);

    XtPopdown(checkDialog);
    XtDestroyWidget(checkDialog);

    if(answer == 1)
        status = 0;
    if(answer == 2)
        status = 1;

    return(status);

}

void closeCheck(w,client_data,callback_data)
Widget        w;
XtPointer     client_data;
XtPointer     callback_data;
{

    XmFileSelectionBoxCallbackStruct *file_callback =
                (XmFileSelectionBoxCallbackStruct *) callback_data;

    int *answer = (int *) client_data;

    if(file_callback->reason == XmCR_CANCEL)
        *answer = 1;

    if(file_callback->reason == XmCR_OK)
        *answer = 2;

    return;
}
