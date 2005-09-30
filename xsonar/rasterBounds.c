/*
 *    rasterBounds.c
 *
 *    Text board for selecting boundaries of an output mapped raster file.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    Sept 16, 1994
 */

#include <unistd.h>

#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/DialogS.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>

#include "xsonar.h"
#include "sonar_struct.h"

void closeBoundsDialog(Widget w, XtPointer client_data, XtPointer callback_data);

void rasterBounds(Widget w, XtPointer client_data,  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    char *tempText;

    Widget boundsDialog;
    Widget boundsForm;
    /*Widget boundsHolder;*/
    Widget separator1;
    Widget separator2; /* FIX: can be used uninitialized */

    Widget boundsRowColumn;
    Widget sizeRowColumn;

    Widget boundsLabel;
    Widget eastLabel;
    Widget eastText;
    Widget westLabel;
    Widget westText;
    Widget northLabel;
    Widget northText;
    Widget southLabel;
    Widget southText;
    Widget pixSizeLabel;
    Widget pixSizeText;
    Widget mosaicRangeLabel;
    Widget mosaicRangeText;
    Widget widthLabel;
    Widget widthText;
    Widget heightLabel;
    Widget heightText;

    Widget buttonRowColumn;
    Widget boundsDoneButton;
    Widget boundsCancelButton;

    ImageHeader boundaries;

    Arg args[40];
    /*Status status;*/
    Cardinal n;

    int cancelAnswer;
    int infd;
    int bytesRead;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    /*double imageSize;*/

    char warningmessage[100];

    /*void closeBoundsDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/
    /*void messages();*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the filter kernel size, and a push button
     *    row-column to apply or cancel the filtering function.
     */

    XtSetSensitive(w, FALSE);
    if(XtNameToWidget(sonar->toplevel, "*NavDialog*ExitButton") != NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*NavDialog*ExitButton"), FALSE);

    if(XtNameToWidget(sonar->toplevel, "*SetupDialog*ExitButton") != NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*SetupDialog*ExitButton"), FALSE);

    if(w == XtNameToWidget(sonar->toplevel, "*SetBoundsButton"))
        XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*NavDialog"));

    /*
     *    Check to see if the file type is RASTER.  If so, set the 
     *    boundaries used by the text widgets to the appropriate values
     *    so that the reader can check them.
     */

    if(sonar->fileType == RASTER)
        {
    /*
     *    open input file
     */

        if ((infd = open (sonar->infile, O_RDONLY)) == -1)
            {
            sprintf(warningmessage,
                      "Cannot open input file\nPlease select a file in Setup");
            messages(sonar, warningmessage);
            return;
            }

        if((bytesRead = read(infd, &boundaries, 
            sizeof(boundaries))) != sizeof(boundaries))
            {
            sprintf(warningmessage,
                    "Could not read image boundaries\nMay be an older file\n");
            strcat(warningmessage, "that needs to be remapped");
            messages(sonar, warningmessage);
            return;
            }

        sonar->rasterWest = boundaries.west;
        sonar->rasterEast = boundaries.east;
        sonar->rasterNorth = boundaries.north;
        sonar->rasterSouth = boundaries.south;
        sonar->rasterPixSize = boundaries.pix_size;


        close(infd);

        }

    /*
     *  Set up buffer to hold text strings
     */

    tempText = (char *) XtCalloc(10, sizeof(char));
 
    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    boundsDialog = XtCreatePopupShell("BoundsDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    boundsForm = XtCreateWidget("BoundsForm", xmFormWidgetClass,
                  boundsDialog, args, n);
    XtManageChild(boundsForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("UTM boundaries for raster map",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    boundsLabel = XtCreateWidget("BoundsLabel",
                             xmLabelWidgetClass, boundsForm, args, n);
    XtManageChild(boundsLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, boundsLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    boundsRowColumn = XtCreateWidget("BoundsRowColumn",
                xmRowColumnWidgetClass, boundsForm, args, n);
    XtManageChild(boundsRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("West: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    westLabel = XtCreateWidget("WestLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(westLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    westText = XtCreateWidget("WestText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(westText);
    sprintf(tempText, "%.2f", sonar->rasterWest);
    XmTextSetString(westText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("East: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    eastLabel = XtCreateWidget("EastLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(eastLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    eastText = XtCreateWidget("EastText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(eastText);
    sprintf(tempText, "%.2f", sonar->rasterEast);
    XmTextSetString(eastText, tempText);


    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("North: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    northLabel = XtCreateWidget("NorthLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(northLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    northText = XtCreateWidget("NorthText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(northText);
    sprintf(tempText, "%.2f", sonar->rasterNorth);
    XmTextSetString(northText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("South: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    southLabel = XtCreateWidget("SouthLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(southLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    southText = XtCreateWidget("SouthText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(southText);
    sprintf(tempText, "%.2f", sonar->rasterSouth);
    XmTextSetString(southText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Pixel Size: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    pixSizeLabel = XtCreateWidget("PixSizeLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(pixSizeLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    pixSizeText = XtCreateWidget("PixSizeText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(pixSizeText);
    sprintf(tempText, "%.4f", sonar->rasterPixSize);
    XmTextSetString(pixSizeText, tempText);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Mosaic Range: ",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    mosaicRangeLabel = XtCreateWidget("MosaicRangeLabel", xmLabelWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(mosaicRangeLabel);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    mosaicRangeText = XtCreateWidget("MosaicRangeText", xmTextWidgetClass,
                                     boundsRowColumn, args, n);
    XtManageChild(mosaicRangeText);
    sprintf(tempText, "%.1f", sonar->mosaicRange);
    XmTextSetString(mosaicRangeText, tempText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, boundsRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, boundsForm, args, n);
    XtManageChild(separator1);

    /*
     *  If the calling button was a request for image header params
     *  (imageHeaderButton, setup.c), then create some extra boxes
     *  to hold the data.
     */

    if(w == XtNameToWidget(sonar->toplevel, "*ImageHeaderButton"))
        {
        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, separator1); n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        XtSetArg(args[n], XmNnumColumns, 1); n++;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNisAligned, True); n++;
        sizeRowColumn = XtCreateWidget("SizeRowColumn",
                    xmRowColumnWidgetClass, boundsForm, args, n);
        XtManageChild(sizeRowColumn);

        n = 0;
        XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Width: ",
             XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNheight, 30); n++;
        widthLabel = XtCreateWidget("WidthLabel", xmLabelWidgetClass,
                                         sizeRowColumn, args, n);
        XtManageChild(widthLabel);

        n = 0;
        XtSetArg(args[n], XmNcolumns, 10); n++;
        XtSetArg(args[n], XmNheight, 30); n++;
        widthText = XtCreateWidget("WidthText", xmTextWidgetClass,
                                         sizeRowColumn, args, n);
        XtManageChild(widthText);
        sprintf(tempText, "%d", boundaries.width);
        XmTextSetString(widthText, tempText);

        n = 0;
        XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Height: ",
             XmSTRING_DEFAULT_CHARSET)); n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNheight, 30); n++;
        heightLabel = XtCreateWidget("HeightLabel", xmLabelWidgetClass,
                                         sizeRowColumn, args, n);
        XtManageChild(heightLabel);

        n = 0;
        XtSetArg(args[n], XmNcolumns, 10); n++;
        XtSetArg(args[n], XmNheight, 30); n++;
        heightText = XtCreateWidget("HeightText", xmTextWidgetClass,
                                         sizeRowColumn, args, n);
        XtManageChild(heightText);
        sprintf(tempText, "%d", boundaries.height);
        XmTextSetString(heightText, tempText);

        n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, sizeRowColumn); n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
        separator2 = XtCreateWidget("Separator2",
                       xmSeparatorWidgetClass, boundsForm, args, n);
        XtManageChild(separator2);
        }

    /*
     *   Create a menu for cancelling or ok'ing the filter selection.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    if(w == XtNameToWidget(sonar->toplevel, "*ImageHeaderButton"))
        XtSetArg(args[n], XmNtopWidget, separator2);
    else
        XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, boundsForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("      Done      ", XmSTRING_DEFAULT_CHARSET)); n++;
    boundsDoneButton = XtCreateWidget("BoundsDoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(boundsDoneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("     Cancel     ", XmSTRING_DEFAULT_CHARSET)); n++;
    boundsCancelButton = XtCreateWidget("BoundsCancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(boundsCancelButton);

    XtAddCallback(boundsDoneButton, XmNactivateCallback, 
                  closeBoundsDialog, sonar);
    XtAddCallback(boundsCancelButton, XmNactivateCallback, 
                                        (XtCallbackProc)closeBoundsDialog, sonar);

    XtRealizeWidget(boundsDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.
    */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(boundsDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(boundsDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(boundsDialog, args, n);

    XtPopup(boundsDialog, XtGrabNone);

    cancelAnswer = 0;

    while(cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(boundsCancelButton, XmNuserData, &cancelAnswer, NULL);
        }

    XtPopdown(boundsDialog);
    XtDestroyWidget(boundsDialog);

    XtSetSensitive(w, True);
    if(XtNameToWidget(sonar->toplevel, "*NavDialog*ExitButton") != NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*NavDialog*ExitButton"), True);

    if(XtNameToWidget(sonar->toplevel, "*SetupDialog*ExitButton") != NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*SetupDialog*ExitButton"), True);

    return;

}

/*void closeBoundsDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data)*/
void closeBoundsDialog(Widget w, XtPointer client_data,
		       UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    int boundsAnswer;
    int screenX, screenY;       /* x and y position of upper left corner for
                                   the raster box (if selected) */

    unsigned int width, height; /* width and height of raster box if selected */

    char message[500];
    char callingWidget[100]; 
#if 0
    void messages();
    void xy_raster();
#endif

    /*
     *   Only do this if the cancel button is selected: set answer
     *   to 1 and let the user cancel the operation.
     */

    strcpy(callingWidget, XtName(w));

    if(!strcmp(callingWidget, "BoundsCancelButton"))
        {
        boundsAnswer = 1;
        XtVaSetValues(w, XmNuserData, boundsAnswer, NULL);
        return;
        }

    if(sonar->rasterFlag)
        {


        sonar->rasterEast = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*EastText")));

        sonar->rasterWest = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*WestText")));

        sonar->rasterNorth = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*NorthText")));

        sonar->rasterSouth = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*SouthText")));

        sonar->rasterPixSize = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*PixSizeText")));

        sonar->mosaicRange = atof(XmTextGetString(
                         XtNameToWidget(sonar->toplevel, "*MosaicRangeText")));

        if(sonar->rasterPixSize != 0)
            {
            height =(unsigned int) ((sonar->rasterNorth - sonar->rasterSouth) / 
                                                         sonar->rasterPixSize);
            width = (unsigned int) ((sonar->rasterEast - sonar->rasterWest) / 
                                                         sonar->rasterPixSize);
            }

        sonar->utm_west[0] = sonar->rasterWest;
        sonar->utm_south[0] = sonar->rasterSouth;

        sonar->number_of_images = 1;


        if(sonar->rasterPixSize == 0)
            {
            sprintf(message, "Cannot have a pixel resolution of 0.\n");
            strcat(message, "Please re-enter a pixel size.");
            messages(sonar, message);
            }
        else if(width == 0 || height == 0)
            {
            sprintf(message, "Width and/or height of box is 0.\n"); 
            strcat(message, "Please re-enter UTM values.");
            messages(sonar, message);
            }
        else if(sonar->pix_s == 0)
            {
            sprintf(message, "Please display sonar swath.\n"); 
            messages(sonar, message);
            }
        else
            {

            sprintf(message, "Raster file is %d samples by %d lines.", 
                         width, height);
            messages(sonar, message);

            boundsAnswer = 1;
            XtVaSetValues(XtNameToWidget(sonar->toplevel, 
                    "BoundsDialog*BoundsCancelButton"), 
                     XmNuserData, boundsAnswer, NULL);

            sonar->pix_s = sonar->rasterPixSize;

            xy_raster(sonar, sonar->rasterWest, sonar->rasterNorth,
                                           &screenX, &screenY, SCREEN);

            width = (sonar->rasterEast - sonar->rasterWest) / 
                                               sonar->rasterPixSize;
            height = (sonar->rasterNorth - sonar->rasterSouth) / 
                                               sonar->rasterPixSize;

            if(XtNameToWidget(sonar->toplevel, "*NavDialog") != NULL)
                {
                XDrawRectangle(XtDisplay(sonar->nav_window),
                    XtWindow(sonar->nav_window), sonar->nav_gc,
                    screenX, screenY, width, height);

                XDrawRectangle(XtDisplay(sonar->nav_window),
                    sonar->nav_pixmap, sonar->nav_gc,
                    screenX, screenY, width, height);

                }
            }

        }
    else
        {
        sprintf(message, "Need to select the Raster button under the");
        strcat(message, "\nOptions menu for these boundaries to be saved.");
        strcat(message, "\nSelect cancel to pop down this window.");
        messages(sonar, message);
/*
        XtVaGetValues(w, XmNuserData, &answer, NULL);
*/
        }

    return;
}


