/* 
    orient.c

    Plots the sonar data along a sinuous track for some given pixel size. 
    It takes as input a file merged with navigation. 

    NEW: fills a polygon in an image array, and then displays the whole
    image array. Supposed to produce a smoother image
    at small pix_size and to go faster at large pix_size.

    Original code written for the MASSCOMP by:
       Alberto Malinverno
       Last modification: Oct 20 1987 

    Rewritten and converted to Motif by:
    William W. Danforth
    U.S. Geological Survey

    Jul 11, 1992
 */

#include <unistd.h>

#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>

#include "sonar_struct.h"
#include "alden.h"

#include <map.h>

#include "xsonar.h"

typedef struct {                /* polygon vertex structure */
    int x;
    int y;
    unsigned char z;
    int x_max;
    int x_min;
    double a;
    double b;
} PixelVertex;

int back_color = 255;            /* black background */
Widget window;

void display(Widget w, XtPointer client_data, XtPointer call_data);
void orient_close(Widget w, XtPointer client_data, XtPointer call_data);
void disp_swath(MainSonar *sonar, unsigned char *current, unsigned char *next, ImageBounds *image_bounds, int blocky_flag, ImageHeader *image_params, FILE *fp1, int outfd, int writeToDisk);
void utm_grid(MainSonar *sonar, int grid_int_ew, int grid_int_ns, ImageHeader *image_params);
void mer_grid(MainSonar *sonar, int dgrid_sec, ImageHeader *image_params, ImageBounds *image_bounds);
Boolean check_event_queue(MainSonar *sonar);
int in_swath(double x0, double y0, double x1, double y1, ImageBounds *box);
int poly_fill(PixelVertex *v, int n, MainSonar *sonar, ImageBounds *image_bounds, int blocky_flag, ImageHeader *image_params, int outfd, int writeToDisk);
void xy_image(MainSonar *sonar, double east, double north, int *scr_x, int *scr_y);
int in_image(int x, int y, ImageBounds *bounds);



void orient_sonar(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;

    Widget orientDialog;
    Widget showProgress;
    /*Widget infoframe;*/
    /*Widget topmessage;*/
    Widget /*separator1,*/ separator2, separator3;
    Widget displaytime;
    /*Widget infomessage;*/
    Widget statusmessage;
    Widget drawframe;
    Widget runButton;
    Widget cancelButton;
    Widget closeButton;

    Dimension shellx;
    Dimension shelly;
    Dimension shellWidth;
    Dimension shellHeight;
    Dimension dialogHeight;
    Dimension dialogWidth;

    Arg args[10];
    Cardinal n;

    XFontStruct *label_font;

    char warningmessage[50];

    /*int i, j, c;*/

    /*void messages(); */

    /*
     *   Check that a map scale has been selected, and that the map
     *   area has been outlined.
     */

    if(!sonar->number_of_images)
        {
        sprintf(warningmessage, "Need to outline the map area in Navigation");
        strcat(warningmessage, "\nor select Raster option.");
        messages(sonar, warningmessage);
        return;
        }

    if(!sonar->scale && (sonar->tduFlag || sonar->aldenFlag))
        {
        sprintf(warningmessage, "No map scale for TDU or Alden output,\n");
        strcat(warningmessage, "select a map scale in Setup.");
        messages(sonar, warningmessage);
        return;
        }

    XtSetSensitive(w, FALSE);
	if(XtNameToWidget(sonar->toplevel, "*NavDialog") == NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, "*NavButton"), FALSE);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*Process"), FALSE);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*DemuxButton"), FALSE);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    orientDialog = XtCreatePopupShell("OrientDialog",
              transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    window = XtCreateWidget("Window", xmFormWidgetClass,
                  orientDialog, args, n);
    XtManageChild(window);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Initializing display",
        XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    statusmessage = XtCreateWidget("StatusMessage",
                    xmLabelWidgetClass, window, args, n);
    XtManageChild(statusmessage);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, statusmessage); n++;
    separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                            window, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("Julian Day 000 / 00:00:00",
        XmSTRING_DEFAULT_CHARSET)); n++;
    displaytime = XtCreateWidget("DisplayTime",
                xmLabelWidgetClass, window, args, n);
    XtManageChild(displaytime);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, displaytime); n++;
    separator3 = XtCreateWidget("Separator3", xmSeparatorWidgetClass,
                            window, args, n);
    XtManageChild(separator3);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    runButton = XtCreateWidget("RunButton",
                xmPushButtonWidgetClass, window, args, n);
    XtManageChild(runButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, runButton); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNwidth, 200); n++;
    drawframe = XtCreateWidget("drawFrame",
                xmFrameWidgetClass, window, args, n);
    XtManageChild(drawframe);

    showProgress = XtCreateWidget("ShowProgress", xmDrawingAreaWidgetClass,
                    drawframe, args, n);
    XtManageChild(showProgress);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, drawframe); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Stop",
                    XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, window, args, n);
    XtManageChild(cancelButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator3); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, cancelButton); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 25); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Close",
                    XmSTRING_DEFAULT_CHARSET)); n++;
    closeButton = XtCreateWidget("CloseButton",
                xmPushButtonWidgetClass, window, args, n);
    XtManageChild(closeButton);

    XtAddCallback(runButton, XmNactivateCallback,  display, sonar);
    XtAddCallback(closeButton, XmNactivateCallback, 
                                           orient_close, sonar);

    XtRealizeWidget(orientDialog);

    XSelectInput(XtDisplay(orientDialog),XtWindow(cancelButton),
                EnterWindowMask | LeaveWindowMask |
                FocusChangeMask | ExposureMask | 
                ButtonPressMask | ButtonReleaseMask);


    XSetForeground(XtDisplay(window), sonar->main_gc,
             WhitePixelOfScreen(XtScreen(window)));

    XSetBackground(XtDisplay(window), sonar->main_gc,
             BlackPixelOfScreen(XtScreen(window)));

    if(sonar->tduFlag || sonar->aldenFlag)
        {
        label_font = XLoadQueryFont(XtDisplay(window),
                "-adobe-courier-bold-r-*-*-*-120-*-*-*-*-*-*");
        if(label_font == 0)
            {
            sprintf(warningmessage, "No font for map, closing window.\n");
            messages(sonar, warningmessage);
            XtSetSensitive(w, True);
            return;
            }

        XSetFont(XtDisplay(window), sonar->main_gc, label_font->fid);

        }
    /*
     *   Center the dialog over the parent shell.
     */

    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(orientDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(orientDialog, XmNheight, &dialogHeight, NULL);

    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(orientDialog, args, n);



    XtPopup(orientDialog, XtGrabNone);


}

void display(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    FILE *fp1=0; /* FIX: can be used uninitialized */

    XmString time_string;
    XmString info_string;
    Arg args[20];
    Cardinal n;

    XEvent event;

    XImage *ximage1;

    Window root;

    char time[200];
    char temp[40];
    char information[300];
    char warningmessage[300];
    char *stringPtr;

    unsigned char *current_image;
    unsigned char *next_image;
    /*unsigned char *pad_image;*/
    unsigned char *diskImageFill;

    ImageBounds image_bounds;
    ImageHeader image_params;

    AldenHeader aldenHeader;

    int position;
    int scansize;
    int imagesize;
    int  mer_grid_int; /* FIX: mer_grid_int can be used uninitialized */
    int hour, min;
    int outbytes;
    int blocky_flag = 0;
    int i, j;
    int imageNum;

     /* FIX: image_h and image_w can be used uninitialized */
    int image_h, image_w;            /* height and width (pix.) of image */

    /* FIX: xl, yb, xr, yt can be used uninitialized */
    int xl, yb, xr, yt;                /* printer boundaries */
    int check;
    int intSec;
    /*int format, offset;*/
    /*int bitmapPad, bytesPerLine;*/
    int base_x, base_y;
    int utm_grid_int_north; /* FIX: can be used uninitialized */
    long utm_grid_int_west; /* FIX: can be used uninitialized */
    int infd;
    int outfd; /* FIX: can be used uninitialized */
    int eventCheck;
    int writeToDisk;

    int pingbytes, headerbytes;

    unsigned int width, height, depth, border_width;

    long filePosition;

    double sec;
    double oldsec;

    float testDouble;

    /*void messages();*/
    /*void writeAldenHeader();*/

    /*int getFileSize();*/                   /*  in utilities.c   */

    Boolean status;


    /*
     *   De-sensitize the Run button and Close button.
     */

    XtSetSensitive(w, False);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*OrientDialog*CloseButton"), False);
   
    /*
     *    Check to make sure an ellipsoid has been selected.
     */

    if (!sonar->ellipsoid)
        {
        sprintf(warningmessage, "Please select an ellipsoid in\n");
        strcat(warningmessage, "the navigation window (options).");
        messages(sonar, warningmessage);
        XtSetSensitive(w, True);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                  "*OrientDialog*CloseButton"), True);

        return;
        }
    /*    
     *    open input file
     */

    if ((infd = open (sonar->infile, 0)) == -1)
        {
        sprintf(warningmessage,
                      "Cannot open input file\nPlease select a file in Setup");
        messages(sonar, warningmessage);
        XtSetSensitive(w, True);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                  "*OrientDialog*CloseButton"), True);
        return;
        }

    filePosition = lseek(infd, 0L, SEEK_SET);

    if (read (infd, &sonar->ping_header, 256) != 256)
        {
        sprintf(warningmessage, "Cannot read data header");
        messages(sonar, warningmessage);
        close(infd);
        XtSetSensitive(w, True);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                  "*OrientDialog*CloseButton"), True);

        return;
        }

    if(sonar->ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        XtSetSensitive(w, True);
        XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                  "*OrientDialog*CloseButton"), True);
        return;
        }


    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - 256;
    filePosition = lseek(infd, 0L, SEEK_SET);

    /*
     *    Allocate storage for input sonar data.
     */

    current_image = (unsigned char *)
                    XtCalloc(imagesize, sizeof(unsigned char));
    if(current_image == NULL)
        fprintf(stderr,"Not enough memory for input sonar array\n");

    next_image = (unsigned char *)
                    XtCalloc(imagesize, sizeof(unsigned char));

    if(next_image == NULL)
        fprintf(stderr,"Not enough memory for next input sonar array\n");

    /*
     *    Initialize the image boundaries for each of the output types.
     */

    if(sonar->tduFlag)   /*   1728 pixels across the print head   */
        {
        xl = 0;
        xr = 1727;
        yb = 0;
        yt = 1727;

        image_w = xr - xl + 1;
        image_h = yt - yb + 1;
        }

    if(sonar->aldenFlag)   /*   2048 pixels across the print head   */
        {
        xl = 0;
        xr = 2047;
        yb = 0;
        yt = (int) ((float)sonar->tduPixelsNS * 10.08867 + 0.5) - 1;

        image_w = xr - xl + 1;
        image_h = yt - yb + 1;
        }

    /*
     *    Initialize this so that mosaicking is done in memory if the
     *    raster flag is set.  It gets set to true if the machine doesn't
     *    have enough memory to allow this and the data is written directly
     *    to disk (slow but doesn't use memory).
     */

    writeToDisk = False;

    if(sonar->rasterFlag)  /*  User defined in meters per pixel  */
        {

        if(sonar->rasterPixSize == 0 || sonar->rasterEast == 0 ||
            sonar->rasterWest == 0 || sonar->rasterNorth == 0 ||
            sonar->rasterSouth == 0)
            {
            sprintf(information, "Raster boundaries have not been set.");
            messages(sonar, information);
            XtSetSensitive(w, True);
            XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                     "*OrientDialog*CloseButton"), True);

            XtFree((char *)current_image);
            XtFree((char *)next_image);

            close(infd);
            return;
            }

        image_w = (sonar->rasterEast - sonar->rasterWest) /
                                                         sonar->rasterPixSize;
        image_h = (sonar->rasterNorth - sonar->rasterSouth) /
                                                         sonar->rasterPixSize;

        xl = 0;
        xr = image_w - 1;
        yb = 0;
        yt = image_h - 1;

        if(image_w * image_h < MAX_MEMORY)
            {
            /*
             *   Allocate storage for image array and initialize it
             */

            sprintf(information, "Initializing mosaic array");
            info_string = XmStringCreateLtoR(information,
                                              XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"),
                                                                  args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(sonar->toplevel);


            sonar->image = (unsigned char *)
                            XtCalloc(image_w*image_h, sizeof(unsigned char));

            if (sonar->image == NULL)
                {
                fprintf(stderr,"Not enough memory for image array\n");
                exit(-1);
                }

            sprintf(information, "Initialization completed");
            info_string = XmStringCreateLtoR(information,
                                      XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel,
                                            "*StatusMessage"), args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(sonar->toplevel);

            }
        else
            writeToDisk = True;


        }

    if(sonar->tduFlag || sonar->aldenFlag)
        {

        /*
         *    Initialize the pixmap used to annotate image with UTM and/or
         *    Mercator tick marks and lines.
         */

        depth = DefaultDepth(XtDisplay(sonar->toplevel), 
                             DefaultScreen(XtDisplay(sonar->toplevel)));

        sonar->orient_pixmap = XCreatePixmap(XtDisplay(sonar->toplevel),
                          RootWindow(XtDisplay(sonar->toplevel),
                          DefaultScreen(XtDisplay(sonar->toplevel))),
                          (unsigned) image_w, (unsigned) image_h, depth);

        status = XGetGeometry(XtDisplay(sonar->toplevel),
                        sonar->orient_pixmap, &root, &base_x, &base_y, &width,
                        &height, &border_width, &depth);

        /*
         *   Allocate storage for image array and initialize it
         */

        sprintf(information, "Initializing image array");
        info_string = XmStringCreateLtoR(information,
                                          XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"),
                                                                args, n);
        XmStringFree(info_string);
        XmUpdateDisplay(sonar->toplevel);

        sonar->image = (unsigned char *)
                        XtCalloc(image_w*image_h, sizeof(char));

        if (sonar->image == NULL)
            {
            fprintf(stderr,"Not enough memory for image array\n");
            exit(-1);
            }


        sprintf(information, "Initialization completed");
        info_string = XmStringCreateLtoR(information,
                                  XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel,
                                        "*StatusMessage"), args, n);
        XmStringFree(info_string);
        XmUpdateDisplay(sonar->toplevel);

        }

    /*
     *    Loop until the number of images requested is processed.
     */ 

    for(imageNum = 0; imageNum < sonar->number_of_images; imageNum++)
        {
        /*
         *    Open the text file if the user wants to write out nadir x,y
         *    locations.  This file pointer is passed to disp_swath().
         */

        if(sonar->nadirTextOut)
            {
            if((fp1 = fopen("asciiText", "wt")) == NULL)
                {
                sprintf(warningmessage, "Could not open text file.\n");
                strcat(warningmessage, 
                         "Check permissions in the directory your working in.");
                messages(sonar, warningmessage);
                XtSetSensitive(w, True);
                XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                        "*OrientDialog*CloseButton"), True);
                if(sonar->tduFlag || sonar->aldenFlag)
                    {
                    XtFree((char *)sonar->image);
                    sonar->image = NULL;
                    XFreePixmap(XtDisplay(sonar->toplevel), 
                                                       sonar->orient_pixmap);
                    }
                XtFree((char *)current_image);
                XtFree((char *)next_image);

                close(infd);
                return;
                }
            }

        sonar->outfile = (char *) XtCalloc(1, strlen(sonar->infile) + 20);

        if(sonar->tduFlag || sonar->aldenFlag)
            {
            strcpy(sonar->outfile, sonar->infile);

            for(i = 0; i < (int)strlen(sonar->infile); i++)  /* FIX: cast to int ok? */
                {
                if(sonar->outfile[i] == '.')
                    {
                    sonar->outfile[i + 1] = '\0';
                    i = strlen(sonar->infile);
                    }
                }

            sprintf(temp,"%c",imageNum + 97);

            strcat(sonar->outfile, temp);
            strcat(sonar->outfile,".img");

            if((outfd = open(sonar->outfile, 
                O_RDWR|O_CREAT|O_TRUNC, PMODE)) < 0)
                {
                sprintf(information, "Error opening output file.");
                messages(sonar, information);
                XtSetSensitive(w, True);
                XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                         "*OrientDialog*CloseButton"), True);
                XtFree((char *)sonar->image);
                sonar->image = NULL;

                XFreePixmap(XtDisplay(sonar->toplevel), sonar->orient_pixmap);
                XtFree((char *)current_image);
                XtFree((char *)next_image);

                XtFree((char *)sonar->outfile);

                close(infd);
                return;
                }

            }

        if(sonar->rasterFlag) 
            {

            stringPtr = (char *) XtCalloc(1, strlen(sonar->infile) + 20);
            strcpy(sonar->outfile, sonar->infile);

            if((stringPtr = strrchr(sonar->outfile, '/')) != NULL)
                *++stringPtr = '\0';
            else
                {
                stringPtr = sonar->outfile;
                strcpy(stringPtr, "./");
                }


            strcat(stringPtr, "mosaic.ras");

            if((outfd = open(sonar->outfile, O_RDWR|O_CREAT, PMODE)) < 0)
                {
                sprintf(information, "Error opening output raster file.");
                messages(sonar, information);
                XtSetSensitive(w, True);
                XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                         "*OrientDialog*CloseButton"), True);
                XtFree((char *)current_image);
                XtFree((char *)next_image);

                XtFree((char *)sonar->outfile);

                close(infd);
                return;
                }

            }


        if(!sonar->c_lon)
            {
            sprintf(warningmessage, 
                       "Please enter a central longitude in setup.");
            messages(sonar, warningmessage);
            XtSetSensitive(w, True);
            XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                     "*OrientDialog*CloseButton"), True);
            if(sonar->tduFlag || sonar->aldenFlag)
                {
                XtFree((char *)sonar->image);
                sonar->image = NULL;
                XFreePixmap(XtDisplay(sonar->toplevel), sonar->orient_pixmap);
                }
            XtFree((char *)current_image);
            XtFree((char *)next_image);

            XtFree((char *)sonar->outfile);

            close(infd);
            close(outfd);
            return;
            }

        /*
         *   Set up UTM map parameters to be passwd to disp_swath
         *   and the gridding routines.
         */

        if(!sonar->swath)
            sonar->swath = sonar->ping_header.swath_width;

        if(sonar->tduFlag || sonar->aldenFlag)
            {
            /*
             *    Compute UTM bounds of the screen
             */

            image_params.pix_size = sonar->pix_s = (sonar->scale / 203)* 0.0254;
            sonar->pix_ew =((double)sonar->scale / (double)203.0) *(double) 0.0254;
            sonar->pix_ns =((double)sonar->scale / sonar->tduPixelsNS) * 0.0254;

            /*
             *   For the raster image, utm_south & west are set in
             *   rasterBounds.c.  Otherwise, they are set in showNav
             *   by placing the box.
             */

            image_params.south = sonar->south = sonar->utm_south[imageNum];
            image_params.west = sonar->west = sonar->utm_west[imageNum];


            image_params.north = sonar->north = 
                                     sonar->south + (image_h * sonar->pix_ns);
            image_params.east = sonar->east = 
                                     sonar->west + (image_w * sonar->pix_ew);

            image_params.width = image_w;
            image_params.height = image_h;

/*
            utm_grid_int_west = (int)(((float)sonar->pix_ew * 203.0) + 0.5);
*/
            utm_grid_int_north = (int)((sonar->pix_ns * sonar->tduPixelsNS) + 0.5);

            testDouble =  (sonar->pix_ew * 203.0) + 0.5;
            utm_grid_int_west = (int) testDouble;
/*
fprintf(stdout, "pixel size e->w is %.9lf\n", sonar->pix_ew);
fprintf(stdout, "pixel size n->s is %.9lf\n", sonar->pix_ns);

fprintf(stdout, "grid interval e->w (before utm_grid) is %d\n", utm_grid_int_west);
fprintf(stdout, "grid interval n->s (before utm_grid) is %d\n", utm_grid_int_north);
*/

            if(sonar->mer_grid_int)
                mer_grid_int = sonar->mer_grid_int;
            else
                mer_grid_int = (int)(((sonar->pix_s * 200) * 2 / 1852.0) * 60);
            }

        if(sonar->rasterFlag)
            {
            image_params.type = RASTER;
            image_params.pix_size = sonar->rasterPixSize;
           
            image_params.width = image_w;
            image_params.height = image_h;

            sonar->north = sonar->rasterNorth;
            sonar->east = sonar->rasterEast;
            sonar->south = sonar->rasterSouth;
            sonar->west = sonar->rasterWest;
            image_params.west = sonar->rasterWest;
            image_params.east = sonar->rasterEast;
            image_params.north = sonar->rasterNorth;
            image_params.south = sonar->rasterSouth;
            image_params.centralLon = sonar->c_lon;
            if(INTEL)
                image_params.byteOrder = LSB;
            if(MOTOROLA)
                image_params.byteOrder = MSB;
            image_params.back_color = 255;
            }

        /*
         *    Set bounds of box for in_box()
         *    and of image for in_image()
         */

        image_bounds.west = sonar->west - sonar->swath/3.0;
        image_bounds.south = sonar->south - sonar->swath/3.0;
        image_bounds.east = sonar->east + sonar->swath/3.0;
        image_bounds.north = sonar->north + sonar->swath/3.0;
        image_bounds.x_min = xl;
        image_bounds.y_min = yb;
        image_bounds.x_max = xr;
        image_bounds.y_max = yt;


        /*
         *   Write out the raster image boundaries if the raster flag
         *   was selected and the raster file is a new file.
         */

        if(sonar->rasterFlag)
            if(getFileSize(sonar->outfile) == 0)
                {
                if((outbytes = write(outfd, &image_params,sizeof(image_params)))
                    !=  sizeof(image_params))
                    {
                    sprintf(warningmessage,
                           "Could not write out raster image boundaries.");
                    messages(sonar, warningmessage);
                    XtSetSensitive(w, True);
                    XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                         "*OrientDialog*CloseButton"), True);
                    XtFree((char *)current_image);
                    XtFree((char *)next_image);
    
                    XtFree((char *)sonar->outfile);

                    close(infd);
                    close(outfd);
                    return;
                    }
                }


        /*
         *    Zero out the image buffers.
         */

        if(sonar->tduFlag || sonar->aldenFlag || !writeToDisk)
            {
            for (i = 0; i < (image_w*image_h); i++)
                sonar->image[i] = back_color;
            }


        if(sonar->rasterFlag)
            {

            /*
             *    Check to see if the file has been created and
             *    the image header and data written to it.  If only
             *    the header has been written, zero out the disk file.
             *    Otherwise, read the data section into sonar-> image
             *    for mosaicking.
             */

            if(getFileSize(sonar->outfile) == sizeof(image_params))
                {
                sprintf(information, "Zeroing out the disk file");
                info_string = XmStringCreateLtoR(information,
                                          XmSTRING_DEFAULT_CHARSET);
                n = 0;
                XtSetArg(args[n], XmNlabelString, info_string); n++;
                XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"),
                                                                  args, n);
                XmStringFree(info_string);
                XmUpdateDisplay(sonar->toplevel);

                diskImageFill = (unsigned char *)
                                     XtCalloc(image_w, sizeof(unsigned char));

                for(i = 0; i < image_w; i++)
                    diskImageFill[i] = back_color;

                for(i = 0; i < image_h; i++)
                    if((outbytes = write(outfd, diskImageFill, image_w))
                        != image_w)
                        {
                        fprintf(stderr, "Error on output, record length: %d\n",
                                                                     outbytes);
                        fprintf(stderr, "Exiting ...\n");
                        exit(-1);
                        }

                XtFree((char *)diskImageFill);

                }

            if(!writeToDisk)
                {

                position = lseek(outfd, 
                             (off_t) (sizeof(image_params)), SEEK_SET); 

                if((outbytes = read(outfd, sonar->image, image_w * image_h))
                    != image_w * image_h)
                    {
                    fprintf(stderr, 
                         "Error reading sonar mosaic, bytes read: %d\n",
                                                                   outbytes);
                    fprintf(stderr, "Exiting ...\n");
                    exit(-1);
                    }
   
                /*
                 *   Rewind the file to just past the header.
                 */

                position = lseek(outfd, 
                             (off_t) (sizeof(image_params)), SEEK_SET); 

                }

            XtFree((char *)sonar->outfile);

            }


        /*
         *   Set the event checker to true so events become processed
         *   immediately.  Also set the time check so that the julian
         *   day display doesn't go crazy (blinking).
         */

        eventCheck = 1;

        oldsec = 0;

        /*
         *    Zero out the image buffers and Read first record to start
         */

        for(i = 0; i < imagesize; i++)
            current_image[i] = back_color;
        for(i = 0; i < imagesize; i++)
            next_image[i] = back_color;


        filePosition = lseek (infd, 0L, SEEK_SET);

        if ((headerbytes = read (infd, &sonar->ping_header, 256)) != 256 ||
            (pingbytes = read (infd, current_image, imagesize)) != imagesize)
            {
            fprintf (stderr, "cannot read first record\n");
            exit(-1);
            }


        /*
         *    Loop until end of input data file
         */

        sprintf(information, "Sonar mapping in progress");
        info_string = XmStringCreateLtoR(information,XmSTRING_DEFAULT_CHARSET);

        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"), args, n);        XmStringFree(info_string);


        while (read (infd, &sonar->next_ping_header, 256) == 256 &&
            (read (infd, next_image, imagesize) == imagesize))
            {

            check = 1;

            if(in_box(sonar->ping_header.utm_e, 
                sonar->ping_header.utm_n, &image_bounds))
                {

                djdtohm(sonar->ping_header.djday, &hour, &min, &sec);

                intSec = (int) sec;

                if(!(intSec % 5) && oldsec != sec)
                    {
                    sprintf(time, "Julian day %03d / %02d:%02d:%02d",
                            (int) sonar->ping_header.djday, hour, min, intSec);
                    time_string = XmStringCreateLtoR(time,
                                               XmSTRING_DEFAULT_CHARSET);

                    XtVaSetValues(XtNameToWidget(sonar->toplevel,
                            "*OrientDialog*DisplayTime"), XmNlabelString, 
                            time_string, NULL);

                    XmUpdateDisplay(XtNameToWidget(sonar->toplevel,
                                                  "*OrientDialog*DisplayTime"));

                    XmStringFree(time_string);

                    }

                oldsec = sec;


                disp_swath (sonar, current_image, next_image, &image_bounds, 
                          blocky_flag, &image_params, fp1, outfd, writeToDisk);

                }

            /*
             *    Update the display, check for events.
             */

            while(check == 1)
                {
                if(XtAppPending(sonar->toplevel_app))
                    {
                    if(XCheckWindowEvent(XtDisplay(sonar->toplevel),
                        XtWindow(XtNameToWidget(sonar->toplevel,
                        "*OrientDialog*CancelButton")),
                        ButtonPressMask | ButtonReleaseMask, &event))
                        {
                        XtFree((char *)sonar->image);
                        sonar->image = NULL;

                        if(sonar->tduFlag || sonar->aldenFlag)
                            {
                            XFreePixmap(XtDisplay(sonar->toplevel), 
                                                    sonar->orient_pixmap);
                            XtFree((char *)sonar->outfile);
                            }

                        XtFree((char *)current_image);
                        XtFree((char *)next_image);
                        close(infd);
                        close(outfd);
                        XtSetSensitive(w, True);
                        XtSetSensitive(XtNameToWidget(sonar->toplevel,
                                   "*OrientDialog*CloseButton"), True);


                        return;
                        }

                    XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
                    XmUpdateDisplay(sonar->toplevel);
                    }
                else
                    check = 0;
                }

            sonar->ping_header = sonar->next_ping_header;

            for(i = 0; i < imagesize; i++)
                current_image[i] = next_image[i];
            }

        /*
         *     Display image, grids and exit
         */


        if(sonar->tduFlag || sonar->aldenFlag)
            {
/*
            XSynchronize(XtDisplay(sonar->toplevel), True);
*/

            XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc,
                   BlackPixelOfScreen(XtScreen(sonar->toplevel)));

            XFillRectangle(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                 sonar->main_gc, 0, 0, (unsigned) image_w, (unsigned) image_h);

            XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc,
                   WhitePixelOfScreen(XtScreen(sonar->toplevel)));


            /*
             *    Copy the ximage to a pixmap for drawing purposes.
             */

            ximage1 = XCreateImage(XtDisplay(sonar->toplevel),
                DefaultVisualOfScreen(XtScreen(sonar->toplevel)),
                depth, ZPixmap, 0, (char *) NULL,
                (unsigned) image_w, (unsigned) image_h, 8, (unsigned) image_w);

			ximage1->bits_per_pixel = 8;
/*
fprintf(stderr, "Ximage field bitsperpix = %d\n", ximage1->bits_per_pixel);
fprintf(stderr, "Ximage field bytesperline = %d\n", ximage1->bytes_per_line);
fprintf(stderr, "Ximage field depth = %d\n", ximage1->depth);
*/
            sprintf(information, "copy ximage to pixmap ...");
            info_string = XmStringCreateLtoR(information, 
                                           XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel, 
                                      "*OrientDialog*StatusMessage"), args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*OrientDialog"));

            ximage1->data = (char *) sonar->image;

            XPutImage(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                               sonar->main_gc, ximage1, 0, 0, 0, 0,
                               (unsigned) image_w, (unsigned) image_h);

            sprintf(information, "copy ximage to pixmap ... done");
            info_string = XmStringCreateLtoR(information, 
                                           XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel, 
                                      "*OrientDialog*StatusMessage"), args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*OrientDialog"));

            /*
             *    Draw the mercator and/or UTM grids if requested in setup.
             */

            if(sonar->utm_flag)
                utm_grid(sonar, utm_grid_int_west,
                             utm_grid_int_north, &image_params);

            if(sonar->mercator_flag)
                mer_grid(sonar, mer_grid_int, &image_params, &image_bounds);

            /*
             *    Copy the pixmap back to an ximage structure.
             */

            sprintf(information, "copying pixmap back to ximage ...");
            info_string = XmStringCreateLtoR(information, 
                                               XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel, 
                                    "*OrientDialog*StatusMessage"), args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*OrientDialog"));


            XGetSubImage(XtDisplay(sonar->toplevel), 
                               sonar->orient_pixmap, 0, 0,
                               (unsigned) image_w,
                               (unsigned) image_h,
                               AllPlanes, ZPixmap, ximage1, 0, 0);

            /*
             *    Write out the new data which includes the UTM and/or
             *    mercator grids.
             */

            sprintf(information, "Processing completed ... Writing image");
            info_string = XmStringCreateLtoR(information, 
                                               XmSTRING_DEFAULT_CHARSET);
            n = 0;
            XtSetArg(args[n], XmNlabelString, info_string); n++;
            XtSetValues(XtNameToWidget(sonar->toplevel, 
                                     "*OrientDialog*StatusMessage"), args, n);
            XmStringFree(info_string);
            XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*OrientDialog"));

            for(j = 0; j < image_h; j++)
                for(i = 0; i < image_w; i++)
                {

                sonar->image[i + j * image_w] = 
                                    (unsigned char) XGetPixel(ximage1, i, j);

            /*
             *    Reverse the image if the user requested it.
             */

                if((sonar->setReturnBlack && sonar->aldenFlag) || 
                    (sonar->setReturnWhite && sonar->tduFlag))
                    sonar->image[i + j * image_w] = 
                                         ~sonar->image[i + j * image_w];

                }

            if(sonar->aldenFlag)
                 writeAldenHeader(outfd, &aldenHeader, 
                                      image_w, image_h, sonar->outfile);

            if((outbytes = write(outfd, sonar->image,
                                image_w * image_h)) != image_w * image_h)
                {
                fprintf(stderr,"Error on output, record length: %d\n",outbytes);
                fprintf(stderr, "Exiting ...\n");
                exit(-1);
                }

            XtFree((char *)sonar->outfile);

            ximage1->data = (char *) NULL;
            XDestroyImage(ximage1);
			}

        if(sonar->rasterFlag && !writeToDisk)
            {
            if((outbytes = write(outfd, sonar->image, image_w * image_h))
                != image_w * image_h)
                {
                fprintf(stderr, 
                       "Error writing sonar mosaic, bytes written: %d\n",
                                                                   outbytes);
                fprintf(stderr, "Exiting ...\n");
                exit(-1);
                }
            }


        close(outfd);

        } /* end of for loop at beginning of function */

    if(sonar->tduFlag || sonar->aldenFlag || !writeToDisk)
        {
        XtFree((char *)sonar->image);
        sonar->image = NULL;
        if(!sonar->rasterFlag)
            XFreePixmap(XtDisplay(sonar->toplevel), sonar->orient_pixmap);
        }

    XtFree((char *)current_image);
    XtFree((char *)next_image);

    sprintf(information, "Exiting");
    info_string = XmStringCreateLtoR(information, XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNlabelString, info_string); n++;
    XtSetValues(XtNameToWidget(sonar->toplevel, 
                                     "*OrientDialog*StatusMessage"), args, n);
    XmStringFree(info_string);
    XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*OrientDialog"));

    close(infd);

    XtSetSensitive(w, True);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, 
                                    "*OrientDialog*CloseButton"), True);

    if(sonar->tduFlag || sonar->aldenFlag)
        sonar->number_of_images = 0;

    return;

}

Boolean check_event_queue(MainSonar *sonar)
{

    XEvent event;
    Boolean status;
    Boolean gotEvent;
    Widget cancelButton;
    Widget orientDialog;
    char information[40];
    Arg args[5];

    int n;

    XmString info_string;

    String widget_string;

    status = False;

/*
        fprintf(stdout, "button sub window = %d\n", event.xbutton.subwindow);
        XtDispatchEvent(&event);
        cancelButton = XtNameToWidget(sonar->popup_widget, "*CancelButton");

*/

    orientDialog = XtNameToWidget(sonar->toplevel, "*OrientDialog");

    cancelButton = XtNameToWidget(sonar->toplevel, 
                                             "*OrientDialog*CancelButton");

    widget_string = XtName(cancelButton);

    gotEvent = XCheckWindowEvent(XtDisplay(orientDialog),XtWindow(cancelButton),
                         ButtonPressMask | ButtonReleaseMask, &event);

    if(gotEvent)
        fprintf(stderr, "got an event\n");


    if(event.type == ButtonRelease || event.type == ButtonPress)
        {
        fprintf(stdout, "button window = %d\n", (int)(event.xbutton.window));
        fprintf(stdout, "event.type = %d\n", event.type);
        }


    if(event.type == ButtonPress)
        {
        status = True;

        sprintf(information, "Mapping Halted");
        info_string = XmStringCreateLtoR(information,
                                XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg(args[n], XmNlabelString, info_string); n++;
        XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusMessage"), args, n);
        XmStringFree(info_string);

        XSendEvent(XtDisplay(orientDialog), XtWindow(cancelButton),
                      True, ButtonReleaseMask, &event);

        XmUpdateDisplay(orientDialog);
        }

    return(status);

}    

/*
void set_calling_widget(w, client_data, call_data)
Widget     w;
XtPointer    client_data;
XtPointer    call_data;
{

    Widget calling_widget = (Widget) client_data;

    XtSetSensitive(calling_widget, True);
    return;
}
*/

void orient_close(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{
    MainSonar *sonar = (MainSonar *) client_data;
    Widget parent_widget;

    parent_widget = w;

    parent_widget = XtNameToWidget(sonar->toplevel, "*OrientDialog");

    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*OrientButton"), True);
	if(XtNameToWidget(sonar->toplevel, "*NavDialog") == NULL)
        XtSetSensitive(XtNameToWidget(sonar->toplevel, "*NavButton"), True);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*Process"), True);
    XtSetSensitive(XtNameToWidget(sonar->toplevel, "*DemuxButton"), True);

    return;
}

/*****************************************************************************
 *
 *    disp_swath()
 *
 *    Plot on screen data between the present swath and the next one
 */
void disp_swath (MainSonar *sonar, unsigned char *current, unsigned char *next, ImageBounds *image_bounds, int blocky_flag, ImageHeader *image_params, FILE *fp1, int outfd, int writeToDisk)
{

    int n = 4;
    int i;

    int scanwidth;
    int pixelRange;

    double far_dist, near_dist, pix_dist;
    double outpix_dist, inpix_dist;
    double east, north, port_east, port_north, stbd_east, stbd_north;
    double theta, next_theta;
    double de, dn, next_de, next_dn;

    double slantRange, beamAngle;

    PixelVertex v[4]/*, w[4]*/;


    scanwidth = sonar->ping_header.sdatasize - HEADSIZE;

    /*    Start by computing direction parameters for the 
     *    swath plot
     */

    pix_dist = ((double)sonar->swath/2.0) / ((double)scanwidth / 2.0);

    if(sonar->mosaicRange)
        {
        far_dist = sonar->mosaicRange; /* distance vehicle-end of swath (m) */
        pixelRange = sonar->mosaicRange / pix_dist;
        }
    else
        {
        far_dist = (double)sonar->swath/2.0; /* distance vehicle-end of swath (m) */
        pixelRange = scanwidth / 2.0;
        }
    near_dist = 0.0;            /* distance vehicle-beginning swath */


    /*
     * Add PI/2 to the fish azimuth to calculate the 
     * delta easting and delta northing between current
     * and next ping.
     *
     */

    theta = sonar->ping_header.utm_azi + M_PI/2.0;
    de = sin(theta);
    dn = cos(theta);
    next_theta = sonar->next_ping_header.utm_azi + M_PI/2.0;    /* next ping */
    next_de = sin(next_theta);
    next_dn = cos(next_theta);

    /*    Check if at least part of the swath is 
     *    inside the screen; if not, return.
     */
    stbd_east = sonar->ping_header.utm_e + far_dist * de; 
    stbd_north = sonar->ping_header.utm_n + far_dist * dn; 
    port_east = sonar->ping_header.utm_e - far_dist * de; 
    port_north = sonar->ping_header.utm_n - far_dist * dn; 

    if (!(in_swath(stbd_east, stbd_north, port_east,
                            port_north, image_bounds)))
        return;

    /*
     *   If the user has selected to print out the x,y positions of nadir
     *   as mapped in image space, do it here, then proceed with mapping.
     */

    if(sonar->nadirTextOut)
        {
        east = sonar->ping_header.utm_e;
        north = sonar->ping_header.utm_n;
        xy_image (sonar, east, north, &v[0].x, &v[0].y);

        fprintf(fp1, "%d  %d  %.2f  %.2f  %f\n",
                          v[0].x, v[0].y, sonar->ping_header.utm_e,
                          sonar->ping_header.utm_n, sonar->ping_header.alt);
        }


    /*    Plot the starboard half, starting from the
     *    starboardmost pixel
     */
    east = stbd_east;
    north = stbd_north;

    xy_image (sonar, east, north, &v[0].x, &v[0].y);
    v[0].z = current[0];
    if (v[0].z < 1) v[0].z = 1;
    /*if (v[0].z > 255) v[0].z = 255; FIX: always true*/

    east = sonar->next_ping_header.utm_e + far_dist * next_de; 
    north = sonar->next_ping_header.utm_n + far_dist * next_dn; 

    xy_image (sonar, east, north, &v[3].x, &v[3].y);

    v[3].z = next[0];
    if (v[3].z < 1) v[3].z = 1;
    /*if (v[3].z > 255) v[3].z = 255; FIX: ALWAYS TRUE */


    for (i = scanwidth / 2 - pixelRange, inpix_dist = far_dist - pix_dist; 
    i < scanwidth / 2; i++, inpix_dist -= pix_dist)
        { 
        east = sonar->ping_header.utm_e + inpix_dist * de; 
        north = sonar->ping_header.utm_n + inpix_dist * dn; 
        xy_image (sonar, east, north, &v[1].x, &v[1].y);

        v[1].z = current[i];

        if (v[1].z < 1) v[1].z = 1;
        /* if (v[1].z > 255) v[1].z = 255;  FIX: ALWAYS TRUE */

        east = sonar->next_ping_header.utm_e + inpix_dist * next_de; 
        north = sonar->next_ping_header.utm_n + inpix_dist * next_dn; 
        xy_image (sonar, east, north, &v[2].x, &v[2].y);
        v[2].z = next[i];

        if (v[2].z < 1) v[2].z = 1;
        /* if (v[2].z > 255) v[2].z = 255; FIX: ALWAYS TRUE */
       
        if(inpix_dist > 4 && sonar->ping_header.alt > 0.0)
            {
            beamAngle = atan((double) inpix_dist / 
                                    (double) sonar->ping_header.alt);
            slantRange = (double) inpix_dist / sin(beamAngle);
            }
        else
            slantRange = sonar->ping_header.alt;
        
        if((slantRange / ((double)sonar->swath / 2.0)) < 0.97)
            poly_fill (v, n, sonar, image_bounds, 
                             blocky_flag, image_params, outfd, writeToDisk);

        v[0] = v[1];
        v[3] = v[2];
        }    


    /*     Do the port side, starting from the innermost port 
     *    pixel
     */
    inpix_dist = - near_dist;
    east = sonar->ping_header.utm_e + inpix_dist * de; 
    north = sonar->ping_header.utm_n + inpix_dist * dn; 
    xy_image (sonar, east, north, &v[0].x, &v[0].y);
    v[0].z = current[scanwidth / 2];
    if (v[0].z < 1) v[0].z = 1;
    /* if (v[0].z > 255) v[0].z = 255; FIX: ALWAYS TRUE */
    east = sonar->next_ping_header.utm_e + inpix_dist * next_de; 
    north = sonar->next_ping_header.utm_n + inpix_dist * next_dn; 
    xy_image (sonar, east, north, &v[3].x, &v[3].y);
    v[3].z = next[scanwidth / 2];
    if (v[3].z < 1) v[3].z = 1;
    /*if (v[3].z > 255) v[3].z = 255; FIX: ALWAYS TRUE */
/*
    for (i = scanwidth / 2, outpix_dist = -(near_dist + pix_dist); i < scanwidth; i++, 
*/
    for (i = scanwidth / 2, outpix_dist = 0; i < scanwidth / 2 + pixelRange; 
    i++, outpix_dist -= pix_dist) { 
        east = sonar->ping_header.utm_e + outpix_dist * de; 
        north = sonar->ping_header.utm_n + outpix_dist * dn; 
        xy_image (sonar, east, north, &v[1].x, &v[1].y);

        v[1].z = current[i];

        if (v[1].z < 1) v[1].z = 1;
        /*if (v[1].z > 255) v[1].z = 255;  FIX: ALWAYS TRUE */

        east = sonar->next_ping_header.utm_e + outpix_dist * next_de; 
        north = sonar->next_ping_header.utm_n + outpix_dist * next_dn; 
        xy_image (sonar, east, north, &v[2].x, &v[2].y);

        v[2].z = next[i];
        if (v[2].z < 1) v[2].z = 1;
        /*if (v[2].z > 255) v[2].z = 255; FIX: ALWAYS TRUE */

        if(outpix_dist < -4 && sonar->ping_header.alt > 0.0)
            {
            beamAngle = atan((double)abs(outpix_dist) / 
                                    (double) sonar->ping_header.alt);
            slantRange = (double) abs(outpix_dist) / sin(beamAngle);
            }
        else
            slantRange = sonar->ping_header.alt;
        
        if((slantRange / ((double)sonar->swath / 2.0)) < 0.97)
            poly_fill (v, n, sonar, image_bounds, blocky_flag, 
                                           image_params, outfd, writeToDisk);

        v[0] = v[1];
        v[3] = v[2];
    }

    return;
}

/*****************************************************************************
 *
 *    poly_fill()
 *
 *    Fills a polygon of n vertexes. v is a vertex structure
 */

#ifdef __APPLE__
/* #define MAXINT INT_MAX */
#include <stdint.h>
#define MAXINT INT32_MAX
#endif
int poly_fill (PixelVertex *v, int n, MainSonar *sonar, ImageBounds *image_bounds, int blocky_flag, ImageHeader *image_params, int outfd, int writeToDisk)
{
    unsigned char diskImageFill;
    /*unsigned char pixelValue;*/

    int inters;
    int i, j, x, y, dx, dy, dist_2;
    int min_dist_2, i_min_dist;  /* FIX: i_min_dist can be used uninitialized */
    int max_x = -MAXINT;
    int max_y = -MAXINT;
    int min_x = MAXINT;
    int min_y = MAXINT;
    int outbytes, inbytes;


    long position;
    long filePosition;

    double fz, tot_coeff, coeff;

    /*    Find min_x, min_y, max_x, max_y
     */
    for (i = 0; i < n; i++) {
        if (v[i].x > max_x)
            max_x = v[i].x;
        if (v[i].x < min_x)
            min_x = v[i].x;
        if (v[i].y > max_y)
            max_y = v[i].y;
        if (v[i].y < min_y)
            min_y = v[i].y;
    }

    /*     If the whole polygon is out of image
     *    return.
     */
    if ((!(in_image(max_x, max_y, image_bounds))) &&
    (!(in_image(min_x, min_y, image_bounds))))
        return (0);

    /*    Check if all vertexes fall in a pixel;
     *    if so, fill the pixel with the value of
     *    vertex[0] and return
     */
    if ((max_x == min_x) && (max_y == min_y))
        {
        if(sonar->tduFlag || sonar->aldenFlag || !writeToDisk)
            {
/*
fprintf(stdout, "values in 1 pixel... image pixel: x = %d\ty = %d\tvalue = %d\n", max_x, max_y, sonar->image[xy_to_i(max_x, max_y, image_params)]);
*/

            if(sonar->image[xy_to_i(max_x, max_y, image_params)] != back_color) 
                sonar->image[xy_to_i(max_x, max_y, image_params)] = (v[0].z +
                    sonar->image[xy_to_i(max_x,max_y,image_params)]) / 2;
            else
                sonar->image[xy_to_i(max_x, max_y, image_params)] = v[0].z;
            }
        else
            {
            position = (long) xy_to_i(max_x, max_y, image_params) ;
            filePosition = lseek(outfd, 72L, SEEK_SET);
            filePosition = lseek(outfd, position, SEEK_CUR);
            diskImageFill = v[0].z;
            outbytes = write(outfd, &diskImageFill, 1);
            }
        return(0);
        }

    /*    Check if all vertexes fall in 2x2 pixels;
     *    if so, fill the pixels with the value of the 
     *    closest vertex (or vertex[0], if blocky_flag
     *    is set) and return
     */
    if (((max_x - min_x) == 1) && ((max_y - min_y) == 1)) {
        for (y = min_y; y <= max_y; y++) {
            for (x = min_x; x <= max_x; x++) {
                if (in_image (x, y, image_bounds)) {
                    if (blocky_flag)
                        sonar->image[xy_to_i(x,y,image_params)] = v[0].z;
                    else {
                    min_dist_2 = MAXINT;
                    for (i = 0; i < n; i++) {
                        dx = x - v[i].x;
                        dy = y - v[i].y;
                        dist_2 = dx*dx + dy*dy;
                        if (dist_2) {
                            if(dist_2 < min_dist_2)
                                i_min_dist = i;
                        }
                        else {
                            i_min_dist = i;
                            break;
                        }
                    }

                    if(sonar->tduFlag || sonar->aldenFlag || !writeToDisk)
                       {
/*
fprintf(stdout, "values in 2x2 pixel... image pixel: x = %d\ty = %d\tvalue = %d\n", x, y, sonar->image[xy_to_i(x, y, image_params)]);
*/
                       if(sonar->image[xy_to_i(x,y,image_params)] != back_color)
                           sonar->image[xy_to_i(x,y,image_params)] =
                               (v[i_min_dist].z +
                               sonar->image[xy_to_i(x,y,image_params)]) / 2;
                       else
                           sonar->image[xy_to_i(x,y,image_params)] =
                                            v[i_min_dist].z;
                       }
                    else
                        {
                        position = (long) xy_to_i(x, y, image_params);
                        filePosition = lseek(outfd, 72L, SEEK_SET);
                        filePosition = lseek(outfd, position, SEEK_CUR);
                        inbytes = read(outfd, &diskImageFill, 1);
                        if(diskImageFill != back_color)
                            diskImageFill = (v[i_min_dist].z+diskImageFill) / 2;
                        else
                            diskImageFill = v[i_min_dist].z;
                        filePosition = lseek(outfd, 72L, SEEK_SET);
                        filePosition = lseek(outfd, position, SEEK_CUR);
                        outbytes = write(outfd, &diskImageFill, 1);
                        }

                    }
                }
            }
        }
        return (0);
    }

    /*    Fill in the vertex structures
     */
    for (i = 0, j = 1; i < n; i++, j++) {
        if (j == n)
            j -= n;
        dx = v[j].x - v[i].x;
        if (dx) {
            dy = v[j].y - v[i].y;
            v[i].b = (double)dy / (double)dx;
            v[i].a = v[i].y - v[i].b * v[i].x;
        }
        v[i].x_max = MAX(v[i].x, v[j].x);
        v[i].x_min = MIN(v[i].x, v[j].x);
    }

    /*    Cycle in the bounds and check if the point (x, y) is
     *    (1) inside the image bounds;
     *    (2) inside the polygon;
     *    if both are true:
     *    - if blocky_flag is not set, compute a value proportional to
     *      the squared distance from the vertexes;
     *    - if blocky_flag is set, assign a value = vertex[0].
     */
    for (y = min_y; y <= max_y; y++) {
        for (x = min_x; x <= max_x; x++) {
            if (in_image (x, y, image_bounds)) {
                inters = 0;
                for (i = 0; i < n; i++) {
                    if ((x < v[i].x_max) && 
                    (x >= v[i].x_min) && 
                    ((double)y <= (v[i].a+v[i].b*(double)x)))
                        inters++;
                }
                if (inters % 2) {        /* inside */
                    if (blocky_flag)
                        sonar->image[xy_to_i(x,y,image_params)] = v[0].z;
                    else {
                    fz = tot_coeff = 0.0;
                    for (i = 0; i < n; i++) {
                        dx = x - v[i].x;
                        dy = y - v[i].y;
                        dist_2 = dx*dx + dy*dy;

                        if (!dist_2) {
                            if(sonar->tduFlag || sonar->aldenFlag || 
                                !writeToDisk)
                                {
/*
fprintf(stdout, "filling in vertices... image pixel: x = %d\ty = %d\tvalue = %d\n",x, y, sonar->image[xy_to_i(x, y, image_params)]);
*/
                                if(sonar->image[xy_to_i(x,y,image_params)] != 
                                                                    back_color) 
                                   sonar->image[xy_to_i(x,y,image_params)] = 
                                   (v[i].z + 
                                   sonar->image[xy_to_i(x,y,image_params)]) / 2;
                                else
                                    sonar->image[xy_to_i(x,y,image_params)] =
                                                                    v[i].z;
                                }
                            else
                                {
                                position = (long) xy_to_i(x, y, image_params);
                                filePosition = lseek(outfd, 72L, SEEK_SET);
                                filePosition = lseek(outfd, position, SEEK_CUR);
                                inbytes = read(outfd,&diskImageFill,1);
                                if(diskImageFill != back_color)
                                    diskImageFill = (v[i].z + diskImageFill)/ 2;
                                else
                                    diskImageFill = v[i].z;
                                filePosition = lseek(outfd, 72L, SEEK_SET);
                                filePosition = lseek(outfd, position, SEEK_CUR);
                                outbytes = write(outfd,&diskImageFill,1);
                                }

                            break;
                        }
                        coeff = 1.0 / (double)dist_2;
                        fz += ((double)v[i].z * coeff);
                        tot_coeff += coeff;
                    }
                    if (dist_2)
                        {
                        if(sonar->tduFlag || sonar->aldenFlag || !writeToDisk)
                           {
/*
fprintf(stdout, "filling in vertices... image pixel: x = %d\ty = %d\tvalue = %d\n",x, y, sonar->image[xy_to_i(x, y, image_params)]);
*/
                           if(sonar->image[xy_to_i(x,y,image_params)] != 
                                                                  back_color) 
                               sonar->image[xy_to_i(x,y,image_params)] =
                                  (int)((0.5 + fz/tot_coeff) +
                                  sonar->image[xy_to_i(x,y,image_params)]) / 2;
                           else
                               sonar->image[xy_to_i(x,y,image_params)] =
                                  (int)(0.5 + fz/tot_coeff);
                           }
                        else
                           {
                           position = (long) xy_to_i(x, y, image_params);
                           filePosition = lseek(outfd, 72L, SEEK_SET);
                           filePosition = lseek(outfd, position, SEEK_CUR);
                           inbytes = read(outfd,&diskImageFill,1);
                           if(diskImageFill != back_color)
                               diskImageFill =  (int) (((0.5 + fz/tot_coeff)
                                                          + diskImageFill) / 2);
                           else
                               diskImageFill =  (int)(0.5 + fz/tot_coeff);
                           filePosition = lseek(outfd, 72L, SEEK_SET);
                           filePosition = lseek(outfd, position, SEEK_CUR);
                           outbytes = write(outfd, &diskImageFill, 1);
                           }
                        }

                    }
                }
            }
        }
    }
    return(1);
}

/*****************************************************************************
 *
 *    xy_image()
 *
 *    Return the integer screen coordinates of the point with coordinates
 *    east and north
 *    NOTE: these coordinates are according to the mgiimage() world,
 *    where the UPPER left corner has coordinates (0,0).
 */
void xy_image (MainSonar *sonar, double east, double north, int *scr_x, int *scr_y)
{

    double tdu850_conv_ew = (sonar->scale / 203.0) * 0.0254;
    double tdu850_conv_ns = (sonar->scale / sonar->tduPixelsNS) * 0.0254;

    if(sonar->tduFlag || sonar->aldenFlag)
        {
        *scr_x = (int)(((east - sonar->west) / tdu850_conv_ew) + 0.5);
        *scr_y = (int)(((sonar->north - north) / tdu850_conv_ns) + 0.5);
        }
    else
        {
        *scr_x = (int)(((east - sonar->west) / sonar->rasterPixSize) + 0.5);
        *scr_y = (int)(((sonar->north - north) / sonar->rasterPixSize) + 0.5);
        }

}

/*****************************************************************************
 *
 *    in_swath()
 *
 *    returns 1 if at least part of the swath of extremes (x0, y0)
 *    and (x1, y1) is inside box
 */
int in_swath(double x0, double y0, double x1, double y1, ImageBounds *box)
{
    double dx, dy, a, b, y_test, x_test;

    if ((in_box(x0, y0, box)) || (in_box(x1, y1, box)))
        return (1);
    dx = x1 - x0;
    if (dx == 0.0){
        if ((x0 <= box->east) && (x0 >= box->west))
            return (1);
        else
            return (0);
    }
    dy = y1 - y0;
    if (dy == 0.0) {
        if ((y0 <= box->north) && (y0 >= box->south))
            return (1);
        else
            return (0);
    }
    b = dy / dx;
    a = y0 - b * x0;
    y_test = a + b * box->east;
    if ((y_test >= box->south) && (y_test <= box->north))
        return (1);
    y_test = a + b * box->west;
    if ((y_test >= box->south) && (y_test <= box->north))
        return (1);
    x_test = (box->south - a) / b;
    if ((x_test >= box->west) && (x_test <= box->east))
        return (1);
    x_test = (box->north - a) / b;
    if ((x_test >= box->west) && (x_test <= box->east))
        return (1);

    return (0);
}

/*****************************************************************************
 *
 *    in_image()
 *
 *    returns 1 if the point (x,y) is inside an image, 0 otherwise.
 */
int in_image (int x, int y, ImageBounds *bounds)
{
    if ((x >= bounds->x_min) && (x <= bounds->x_max) && 
    (y >= bounds->y_min) && (y <= bounds->y_max))
        return (1);
    else
        return (0);
}

/*****************************************************************************
 *
 *    utm_grid()
 *
 *    Plots and labels a UTM grid on the screen
 */
void utm_grid (MainSonar *sonar, int grid_int_ew, int grid_int_ns, ImageHeader *image_params)
{
    char grid_label [20];
    int lab_off = 8;    /* offset of the grid labels */
    int e, w, s, n;
    int rastx, rasty;
    double x, y;
#if 0
    void test_xy_raster();
    void xy_raster();
#endif
    if(sonar->rasterFlag)
        return;


    XSetLineAttributes(XtDisplay(sonar->toplevel), sonar->main_gc, 1,
                                        LineSolid, CapButt, JoinRound);


    /*
     *    Draw vertical (north - south) grid lines
     */


    x = (1 + (int)(sonar->west) / grid_int_ew) * grid_int_ew;

/*
    XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc,
                   WhitePixelOfScreen(XtScreen(sonar->toplevel)));
*/

    XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc, 
					                            sonar->redPixel.pixel);

    while (x < sonar->east)
        {    

        xy_raster (sonar, x, sonar->south, &rastx, &s, TDU);
        xy_raster (sonar, x, sonar->north, &rastx, &n, TDU);

        XDrawLine(XtDisplay(sonar->toplevel), sonar->orient_pixmap, 
                                 sonar->main_gc, rastx, 0, rastx, 
                                 image_params->height);

        if((sonar->image[xy_to_i(rastx + lab_off, 
            image_params->height - lab_off, image_params)] == back_color)) 
            {
            sprintf (grid_label, "%d", (int)x);


            XDrawString(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                        sonar->main_gc, rastx+lab_off, 
                        image_params->height - lab_off,
                        grid_label, strlen(grid_label));

            }
        x += grid_int_ew;

        }

    /*
     *    Draw horizontal (east - west) grid lines
     */

    y = (1 + (int)(sonar->south) / grid_int_ns) * grid_int_ns;



    while (y < sonar->north)
        {    
        xy_raster (sonar, sonar->west, y, &w, &rasty, TDU);
        xy_raster (sonar, sonar->east, y, &e, &rasty, TDU);

        if((sonar->image[xy_to_i(0 + lab_off,rasty - lab_off, image_params)]
                                                == back_color))
            {
            sprintf (grid_label, "%d", (int)y);

            XDrawString(XtDisplay(sonar->toplevel), sonar->orient_pixmap,
                        sonar->main_gc, 0 + lab_off, rasty - lab_off,
                        grid_label, strlen(grid_label));

            }

        XDrawLine(XtDisplay(sonar->toplevel), sonar->orient_pixmap, 
                                   sonar->main_gc, w, rasty, e, rasty);

        y += grid_int_ns;

        }


    XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc,
                   WhitePixelOfScreen(XtScreen(sonar->toplevel)));

    return;

}

/*****************************************************************************
 *
 *    mer_grid()
 *
 *    Plots and labels a Mercator grid on the screen
 */
void mer_grid (MainSonar *sonar, int dgrid_sec, ImageHeader *image_params, ImageBounds *image_bounds)
{
    char grid_label[10];

    int crossw;
    int lon_startdeg, lat_startdeg, lat_startmin, lon_startmin;
    int x, y;
    int lat_deg, lat_minutes, lat_seconds, lon_deg, lon_minutes, lon_seconds;
    int pixel;
    int index;

    double lat_min, lon_min, lat_max, lon_max;    
    double lat_startsec, lon_startsec;
    double lat_start, lon_start;
    double lat, lon, lat_sec, lon_sec;
    double north, east;    
    double delta_grid;
#if 0
    void xy_raster();
    void geoutm(double, double, double, double *, double *, int);
    void ddtodegminsec(double, int *, int *, double *);
#endif

    if(sonar->rasterFlag)
        return;

    /*    Compute lat. and long. to start and end
     */

    delta_grid = (double)dgrid_sec / 3600.0;

    utmgeo (&lat_min, &lon_min, sonar->c_lon, sonar->west, 
                                   sonar->south, 'n', sonar->ellipsoid);
    utmgeo (&lat_max, &lon_max, sonar->c_lon, sonar->east, 
                                   sonar->north, 'n', sonar->ellipsoid);

    ddtodegminsec (lat_min, &lat_startdeg, &lat_startmin, &lat_startsec);
    ddtodegminsec (lon_min, &lon_startdeg, &lon_startmin, &lon_startsec);

    lat_startsec = (int)lat_startsec + 1.0;

    while ((int)lat_startsec % dgrid_sec)
        lat_startsec++;

    if (lat_startsec >= 60.0)
        {
        lat_startsec -= 60.0;
        lat_startmin++;

        if(lat_startmin > 60)
            {
            lat_startmin -= 60;

            if (lat_startdeg >= 0)
                lat_startdeg++;
            else 
                lat_startdeg--;
            }
        }

    lon_startsec = (int)lon_startsec + 1.0;
    while ((int)lon_startsec % dgrid_sec)
        lon_startsec++;

    if (lon_startsec >= 60.0)
        {
        lon_startsec -= 60.0;
        lon_startmin++;

        if(lon_startmin > 60)
            {
            lon_startmin -= 60;

            if (lon_startdeg >= 0)
                lon_startdeg++;
            else 
                lon_startdeg--;
            }
        }

    degmintodd (&lat_start, lat_startdeg,
                    (double)lat_startmin + (lat_startsec / 60));
    degmintodd (&lon_start, lon_startdeg,
                    (double)lon_startmin + (lon_startsec / 60));
    lat = lat_start;
    lon = lon_start;

    XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc, 
					                            sonar->redPixel.pixel);
    /*   
     *    Loop on lat and lon drawing crosses and labels.
     *    Each line of cross is 10 pixels.
     */

    crossw = 10;

    for (lat = lat_start; lat < lat_max; lat += delta_grid)
        {
        for (lon = lon_start; lon < lon_max; lon += delta_grid)
            {
            geoutm (lat, lon, sonar->c_lon, &east, &north, sonar->ellipsoid);
            if (in_box(east, north, image_bounds))
                {
                xy_raster (sonar, east, north, &x, &y, TDU);

                index = xy_to_i(x,y,image_params);
                if(index > 0 && 
                    index < image_params->width * image_params->height)
                    {
                    pixel = sonar->image[index]; 
                    if(pixel == back_color)
                        {

                        XDrawLine(XtDisplay(sonar->toplevel),
                                  sonar->orient_pixmap, sonar->main_gc, 
                                  x - crossw, y, x + crossw, y);
                        XDrawLine(XtDisplay(sonar->toplevel),
                                  sonar->orient_pixmap, sonar->main_gc,  
                                  x, y - crossw, x, y + crossw);

                        /* lat */
                        ddtodegminsec (lat, &lat_deg, &lat_minutes, &lat_sec);
                        lat_seconds = (int)(lat_sec + 0.5);
                        if (lat_seconds >= 60)
                            {
                            lat_seconds -= 60;
                            lat_minutes++;

                            if(lat_minutes > 60)
                                {
                                lat_minutes -= 60;

                                if (lat_startdeg >= 0)
                                    lat_deg++;
                                else 
                                    lat_deg--;
                                }
                            }

                        sprintf (grid_label, "%2d %.2d %.2d",
                                lat_deg, lat_minutes, lat_seconds);

                        XDrawString(XtDisplay(sonar->toplevel),
                                sonar->orient_pixmap,
                                sonar->main_gc, x + crossw + 5, y + 5,
                                grid_label, strlen(grid_label));

                        /* lon */

                        ddtodegminsec (lon, &lon_deg, &lon_minutes, &lon_sec);
                        lon_seconds = (int)(lon_sec + 0.5);

                        if (lon_seconds >= 60)
                            {
                            lon_seconds -= 60;
                            lon_minutes++;

                            if(lon_minutes > 60)
                                {
                                lon_minutes -= 60;

                                if (lon_startdeg >= 0)
                                    lon_deg++;
                                else 
                                lon_deg--;
                                }
                            }

                           sprintf (grid_label, "%2d %.2d %.2d",
                                    lon_deg, lon_minutes, lon_seconds);

                        XDrawString(XtDisplay(sonar->toplevel),
                            sonar->orient_pixmap,
                            sonar->main_gc, x - 3 * crossw, y - (crossw + 5),
                            grid_label, strlen(grid_label));

                        } /* end of pixel == backcolor if */

                    } /* end of image index check */

                } /* end of inbox check */

            } /* end of inner for loop */

        } /* end of outer for loop */

    XSetForeground(XtDisplay(sonar->toplevel), sonar->main_gc,
                   WhitePixelOfScreen(XtScreen(sonar->toplevel)));

    return;

}
