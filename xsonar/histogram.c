/* 
 *    Callback to display histogram 
 *    of sonar data.
 *
 */

#include <unistd.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>

#include <map.h>
#include "xsonar.h"

#include     "sonar_struct.h"


#define     MAX_GRAY 256            /* range of pixel values */

typedef struct {
        Colormap colormap;    
        GC gc;
        Pixmap pixmap;
    } Hist;

Hist hist; /* This should be included in XmNuserData, but I haven't worked
            * that out yet... 2/19/93
            */

void calculate_hist(Widget w, XtPointer client_data, XtPointer call_data);
void close_hist(Widget w, XtPointer client_data, XtPointer call_data);
void redraw_hist(Widget w, XtPointer client_data, XtPointer call_data);
void draw_hist(MainSonar *sonar, Widget displayHist, int *histogram_dist, int max_hist_value, double *mean, int minPixel, int maxPixel);


void histogram(UNUSED Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget histWindow;
    Widget histDialog;
    Widget histTitle;
    Widget histShow;
    Widget histClose;
    Widget histFileFrame;
    Widget histDrawFrame;
    Widget histFileMenuBar;
    Widget histFileButton;
    Widget histFilePullDownMenu;
    Widget displayHist;
    Widget separator1;
    Widget separator2;
    Widget histButton;

    Arg args[10];
    Cardinal n;

    /*Colormap default_colormap;*/    /* colormap from server */
    /*XColor red_color;*/
    /*XColor colors[256];*/
    /*XColor color;*/
    /*XColor hardColor;*/


    /*Status result;*/
    XSetWindowAttributes main_window_attr;  /* window attribute struct */

    /*char warningmessage[100];*/

    unsigned long main_window_valuemask;
    /*unsigned long cell;*/           /* pixel number */
    /*unsigned short intensity;*/

    /*unsigned long pixels[255];*/         /* pixel values in colormap */
    /*unsigned long plane_masks[8];*/ /* number of planes in system */

    /*unsigned int width, height;*/

    /*int check_file_status;*/
    /*long file_size;*/


     /*
      *    Now we can desensitize the histButton on the main display 
      *    and show the histogram.
      */

    histButton = XtNameToWidget(sonar->toplevel, "*HistButton");
    XtSetSensitive(histButton, FALSE);

    /*
     *      Start creating widgets for this app.
     */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, True); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     XtSetArg(args[n], XmNx, 100); n++;
     XtSetArg(args[n], XmNy, 100); n++;
     histDialog = XtCreatePopupShell("HistDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     histWindow = XtCreateWidget("HistWindow", xmFormWidgetClass,
                      histDialog, args, n);
     XtManageChild(histWindow);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     histFileMenuBar = XmCreateMenuBar(histWindow, "HistFileMenuBar", args, n);
     XtManageChild(histFileMenuBar);

     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'F'); n++;
     histFileButton = XtCreateWidget("HistFileButton",
                    xmCascadeButtonWidgetClass, histFileMenuBar, args, n);
     XtManageChild(histFileButton);

     histFilePullDownMenu = XmCreatePulldownMenu(histFileMenuBar,
                                        "HistFilePullDownMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, histFilePullDownMenu); n++;
     XtSetValues(histFileButton, args, n);

     histShow = XtCreateWidget("HistShow",
                    xmPushButtonWidgetClass, histFilePullDownMenu, NULL, 0);
     XtManageChild(histShow);

     histClose = XtCreateWidget("HistClose",
                    xmPushButtonWidgetClass, histFilePullDownMenu, NULL, 0);
     XtManageChild(histClose);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, histFileMenuBar); n++;
     separator1 = XtCreateWidget("Separator1", xmSeparatorWidgetClass,
                                   histWindow, args, n);
     XtManageChild(separator1);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     histFileFrame = XtCreateWidget("HistFileFrame",
                         xmFrameWidgetClass, histWindow, args, n);
     XtManageChild(histFileFrame);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(sonar->infile,
          XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
     XtSetArg(args[n], XmNheight, 20); n++;
     XtSetArg(args[n], XmNwidth, 700); n++;
     XtSetArg(args[n], XmNrecomputeSize, False); n++;
     histTitle = XtCreateWidget("HistTitle",
                         xmLabelWidgetClass, histFileFrame, args, n);
     XtManageChild(histTitle);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, histFileFrame); n++;
     separator2 = XtCreateWidget("Separator2", xmSeparatorWidgetClass,
                                   histWindow, args, n);
     XtManageChild(separator2);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator2); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     histDrawFrame = XtCreateWidget("HistDrawFrame",
                         xmFrameWidgetClass, histWindow, args, n);
     XtManageChild(histDrawFrame);

    n = 0;
     XtSetArg(args[n], XmNheight,500); n++;
     XtSetArg(args[n], XmNwidth, 700); n++;
     displayHist = XtCreateWidget("DisplayHist",
                   xmDrawingAreaWidgetClass, histDrawFrame, args, n);
     XtManageChild(displayHist);

     XtRealizeWidget(histDialog);


     hist.gc = XCreateGC(XtDisplay(histDialog),
                         XtWindow(histDialog), None, NULL);

     /*   Get default colormap so it can be reset after the app finishes
     */

     /*
      *   Set screen gc and colormap
      */

     main_window_attr.colormap = sonar->colormap;
     main_window_valuemask = CWColormap;

     XChangeWindowAttributes(XtDisplay(histDialog),
                    XtWindow(histDialog), main_window_valuemask,
                    &main_window_attr);

     XChangeWindowAttributes(XtDisplay(histDialog),
                    XtWindow(displayHist), main_window_valuemask,
                    &main_window_attr);

     XSetBackground(XtDisplay(histWindow), hist.gc,
             BlackPixelOfScreen(XtScreen(histWindow)));

     XSetForeground(XtDisplay(histWindow), hist.gc,
             BlackPixelOfScreen(XtScreen(histWindow)));


     hist.pixmap = XCreatePixmap(XtDisplay(histWindow),
                              XtWindow(histWindow), 700, 500,
                              DefaultDepth(XtDisplay(histWindow),
                              DefaultScreen(XtDisplay(histWindow))));




     XFillRectangle(XtDisplay(histWindow), hist.pixmap,
                            hist.gc, 0, 0, 700, 500);

     XSetForeground(XtDisplay(histWindow), hist.gc,
             WhitePixelOfScreen(XtScreen(histWindow)));
/*
    XtVaSetValues(displayHist, XmNuserData, hist, NULL);
*/

     XtAddCallback(displayHist, XmNexposeCallback, 
                                         redraw_hist, NULL);
     XtAddCallback(histClose, XmNactivateCallback, 
                                         close_hist, sonar);
     XtAddCallback(histShow, XmNactivateCallback, 
                                         calculate_hist, sonar);

    XtPopup(histDialog, XtGrabNone);

    XSync(XtDisplay(histDialog), False);

    XmUpdateDisplay(sonar->toplevel);

    return;
}

void    draw_hist(MainSonar *sonar, Widget displayHist, int *histogram_dist, 
		  UNUSED int max_hist_value, double *mean, 
		  int minPixel, int maxPixel)
{

    int i, j;

    XFontStruct *label_font;

    char label_text[5];
    char mean_text[200];
    char warningmessage[100];

    int /*label_value,*/ text_offset; /* FIX: can be used uninitialized */
    int percentLabelValue;

    /*void messages();*/

    /*
     *    Clear the screen, and zero out the pixmap by filling it with
     *    black.
     */

     XSetBackground(XtDisplay(displayHist), hist.gc,
             BlackPixelOfScreen(XtScreen(displayHist)));

     XSetForeground(XtDisplay(displayHist), hist.gc,
             BlackPixelOfScreen(XtScreen(displayHist)));

     XFillRectangle(XtDisplay(displayHist), XtWindow(displayHist),
                            hist.gc, 0, 0, 700, 500);
     XFillRectangle(XtDisplay(displayHist), hist.pixmap,
                            hist.gc, 0, 0, 700, 500);
     XSetForeground(XtDisplay(displayHist), hist.gc,
             WhitePixelOfScreen(XtScreen(displayHist)));


     label_font = XLoadQueryFont(XtDisplay(displayHist),
            "-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*");

/*
            "-adobe-courier-bold-r-normal--14-140-75-75-m-90-iso8859-1");
*/

     if(label_font == 0)
          {
          sprintf(warningmessage,"Could not load special font.\nUsing 8X13");
          messages(sonar, warningmessage);
          label_font = XLoadQueryFont(XtDisplay(displayHist), "8x13");
          }

    XSetFont(XtDisplay(displayHist), hist.gc, label_font->fid);

    XDrawLine(XtDisplay(displayHist), XtWindow(displayHist),
                                hist.gc, 50, 10, 50, 450);
    XDrawLine(XtDisplay(displayHist), XtWindow(displayHist),
                                hist.gc, 50, 450, 560, 450);
    XDrawLine(XtDisplay(displayHist), hist.pixmap,
                                hist.gc, 50, 10, 50, 450);
    XDrawLine(XtDisplay(displayHist), hist.pixmap,
                                hist.gc, 50, 450, 560, 450);

    for(i = 0, j = 0; i < MAX_GRAY; i+=10, j += 44)
        {


        /*
         *     Draw label ticks and labels
         */

        XDrawLine(XtDisplay(displayHist), XtWindow(displayHist),
                hist.gc, 45, 450 - j, 50, 450 - j);
        XDrawLine(XtDisplay(displayHist), hist.pixmap,
                hist.gc, 45, 450 - j, 50, 450 - j);


        XDrawLine(XtDisplay(displayHist), XtWindow(displayHist),
                    hist.gc, (i * 2) + 50, 450, (i * 2) + 50, 455);
        XDrawLine(XtDisplay(displayHist), hist.pixmap,
                    hist.gc, (i * 2) + 50, 450, (i * 2) + 50, 455);

/*
        label_value = (int) (((double)j / 450.0) * (double)max_hist_value);
*/
        percentLabelValue = (int) (((double)j / 440.0) * 100.0);

        if(percentLabelValue < 110)
            {
            itoa(percentLabelValue, label_text);

            if(strlen(label_text) == 3)
                text_offset = 0;
            if(strlen(label_text) == 2)
                text_offset = 4;
            if(strlen(label_text) == 1)
                text_offset = 8;

            XDrawString(XtDisplay(displayHist), XtWindow(displayHist),
                hist.gc, 26 + text_offset, 453 - j, label_text,
                strlen(label_text));
            XDrawString(XtDisplay(displayHist), hist.pixmap,
                hist.gc, 26 + text_offset, 453 - j, label_text,
                strlen(label_text));
            }

        itoa(i, label_text);

        if(strlen(label_text) == 3)
            text_offset = 44;
        if(strlen(label_text) == 2)
            text_offset = 46;
        if(strlen(label_text) == 1)
            text_offset = 48;

          XDrawString(XtDisplay(displayHist), XtWindow(displayHist), hist.gc,
            (i * 2) + text_offset, 465, label_text, strlen(label_text));
          XDrawString(XtDisplay(displayHist), hist.pixmap, hist.gc,
            (i * 2) + text_offset, 465, label_text, strlen(label_text));

        }

     label_font = XLoadQueryFont(XtDisplay(displayHist),
            "-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*");

     if(label_font == 0)
          {
          sprintf(warningmessage,"Could not load special font.\nUsing 8X13");
          messages(sonar, warningmessage);
          label_font = XLoadQueryFont(XtDisplay(displayHist), "8x13");
          }

    XSetFont(XtDisplay(displayHist), hist.gc, label_font->fid);

    sprintf(mean_text, "Histogram mean = %.1f", *mean);
    XDrawString(XtDisplay(displayHist), XtWindow(displayHist), hist.gc,
                445, 100, mean_text, strlen(mean_text));
    XDrawString(XtDisplay(displayHist), hist.pixmap, hist.gc,
                445, 100, mean_text, strlen(mean_text));

    sprintf(mean_text, "Max pixel = %d, Min pixel = %d", 
                                                   maxPixel, minPixel);
    XDrawString(XtDisplay(displayHist), XtWindow(displayHist), hist.gc,
                445, 120, mean_text, strlen(mean_text));
    XDrawString(XtDisplay(displayHist), hist.pixmap, hist.gc,
                445, 120, mean_text, strlen(mean_text));

    XSetForeground(XtDisplay(displayHist), hist.gc, sonar->redPixel.pixel);

    for(i = 0, j = 0; i < MAX_GRAY; i++, j += 2)
        {
        if(histogram_dist[i] > 440)
            histogram_dist[i] = 440;

        if(histogram_dist[i] != 0)
            {
            XFillRectangle(XtDisplay(displayHist), XtWindow(displayHist),
            hist.gc, j + 50, 450 - histogram_dist[i], 2, histogram_dist[i]);
    
            XFillRectangle(XtDisplay(displayHist), hist.pixmap, hist.gc,
                j + 50, 450 - histogram_dist[i], 2, histogram_dist[i]);
            }
        }

    XSetForeground(XtDisplay(displayHist), hist.gc,
             WhitePixelOfScreen(XtScreen(displayHist)));



    return;
        
}

void calculate_hist(UNUSED Widget w,
		    XtPointer client_data,
		    UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Arg args[5];
    Cardinal n;

    long pixel_value, pixel_total = 0;

    int i;
    int scansize;
    int imagesize;
    int histogram_table[MAX_GRAY];
    int histogram_dist[MAX_GRAY];
    int max_hist_value = 0;
    int infd;

    int minPixel, maxPixel;             /*  min and max pixel values  */

    long position;

    double gray_level_fraction, pixel_sum;
    double temp_hist_value;
    double mean;

    char warningmessage[100];
    unsigned char *indata;

    /*void messages();*/

    /*XEvent event;*/

    Widget displayHist;

    /*void messags();*/

    minPixel = 255;
    maxPixel = 0;

    XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*HistDialog"));

     /*
      *    Check for the existence of a file.  If none has been
      *    selected, return.
      */

     infd = open (sonar->infile, O_RDONLY);  /* read only */
     if (infd == -1)
          {
          sprintf(warningmessage,
                  "Cannot open input file\nPlease select a data file in Setup");
          messages(sonar, warningmessage);
          return;
          }


     if (read (infd, &sonar->ping_header, 256) != 256)
          {
          sprintf(warningmessage,"Cannot read data header");
          messages(sonar, warningmessage);
          close(infd);
          return;
          }

    if(sonar->ping_header.fileType != XSONAR && sonar->fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }


     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(sonar->infile,
          XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetValues(XtNameToWidget(sonar->toplevel, "*HistTitle"), args, n);

     /*
      *    Make sure we can read the file header, and calloc space 
      *    for the data.
      */

     scansize = sonar->ping_header.sdatasize;
     imagesize = scansize - HEADSIZE;

     position = lseek (infd, 0L, SEEK_SET);

     indata = (unsigned char *) XtCalloc(imagesize, sizeof(unsigned char));
     if(indata == NULL)
          {
          sprintf(warningmessage, "Not enough memory for input sonar array");
          messages(sonar, warningmessage);
          return;
          }

    /*
     *    Initialize histogram and sums
     */

    for (i = 0; i < MAX_GRAY; i++)
        {
        histogram_table[i] = 0;
        histogram_dist[i] = 0;
        }

    gray_level_fraction = pixel_sum = 0.0;


    /*
     *    Read input image and compute histogram
     */

/*
     sprintf(warningmessage,"Reading data, please wait");
     messages(sonar, warningmessage);
*/
     while((read (infd, &sonar->ping_header, 256) == 256) &&
                         (read (infd, indata, imagesize) == imagesize))
        {

        for(i = 0; i < imagesize; i++)
            {
            pixel_value = indata[i];
            histogram_dist[pixel_value]++;
            pixel_sum += pixel_value;
            pixel_total++;

            if(histogram_dist[pixel_value] > max_hist_value && pixel_value > 5)
                max_hist_value = histogram_dist[pixel_value];

            }
        }

    mean =  pixel_sum / (double) pixel_total;
/*
fprintf(stdout, "pixel_sum = %f, pixel_total = %d, mean = %f\n", pixel_sum,
                            pixel_total, mean);
*/
    /*    Compute look-up table
     */


    for (i = 0; i < MAX_GRAY; i++)
        {

        gray_level_fraction += (double) (histogram_dist[i] * i) / pixel_sum;
        histogram_table[i] = (int)(0.5 + gray_level_fraction * 255.0);

        if(gray_level_fraction * 100.0 > 1.0 
            && gray_level_fraction * 100.0 < 1.5)
            minPixel = i;

        if(gray_level_fraction * 100.0 >  99.0
            && gray_level_fraction * 100.0 < 99.1)
            maxPixel = i;

        if (histogram_table[i] > 255)
            histogram_table[i] = 255;
        if(histogram_table[i] < 0)
            histogram_table[i] = 0;
        }

/*
fprintf(stdout, "level = %d\thist fraction = %f\n", i, gray_level_fraction);
fprintf(stdout, "hist max value = %d\n", *max_hist_value);
fprintf(stdout, "hist dist[%d] = %d\n", i, histogram_dist[i]);
fprintf(stdout, "temp hist = %lf\n", temp_hist_value); 
fprintf(stdout, "hist dist[%d] now = %d\n", i, histogram_dist[i]);
*/

    /*
     *    Scale the values to fit in a 500 pixel high window
     */

    for(i = 0; i < MAX_GRAY; i++)
        {
        temp_hist_value =  (((double)histogram_dist[i] /
                                (double) max_hist_value) * 440.0);
        histogram_dist[i] = (int) temp_hist_value;
        }

    displayHist = XtNameToWidget(sonar->toplevel, "*DisplayHist");
/*
fprintf(stderr, "getting hist from user data...\n");
    XtVaGetValues(displayHist, XmNuserData, &hist, NULL);
fprintf(stderr, "done.\n");
*/

    draw_hist(sonar, displayHist, histogram_dist, 
                                 max_hist_value, &mean, minPixel, maxPixel);    

    close(infd);

    return;
}


void redraw_hist(Widget w, UNUSED XtPointer client_data,
		 UNUSED XtPointer call_data)
{
/*
    Hist *hist = (Hist *) client_data;
*/

     XCopyArea(XtDisplay(w), hist.pixmap,
               XtWindow(w), hist.gc,
               0, 0, 700, 500, 0, 0);

     return;
}

void close_hist(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget parent_widget;
    Widget histButton;

    /*void messages();*/

    parent_widget = w;

    while(!XtIsTransientShell(parent_widget))
        parent_widget = XtParent(parent_widget);

    histButton = XtNameToWidget(sonar->toplevel, "*HistButton");
    XtSetSensitive(histButton, TRUE);

    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    return;

}

