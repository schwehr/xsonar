

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void file_retriever(w,client_data,callback_data)
Widget                 w;
XtPointer              client_data;
XtPointer              callback_data; 
{


    XmFileSelectionBoxCallbackStruct *file_callback =
                (XmFileSelectionBoxCallbackStruct *) callback_data; 

    struct draw *drawarea = (struct draw *) client_data;

    char *file;
    char *filename;
    unsigned char *readBuffer;

    XmString file_message;

    long position;

    void message_display();
    void clearWindow();
    void saveFile();
    int getHeaderInfo();

    Arg args[40];
    Cardinal n;

    int fp2;

    if(file_callback->reason == XmCR_CANCEL)
        {
        /*
         *  Destroy the widget after unmanaging it as the dialog is
         *  recreated the next time a retrieve is requested.
         *  Unexpected behavior results from this if you don't.
         */
     
        XtUnmanageChild(w);
        XtDestroyWidget(w);
        if(w == XtNameToWidget(drawarea->shell, "*GetFileDialog"))
            {
            XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*FileRetrieveButton"), True);
            drawarea->top_ping = 0;
            }
        if(w == XtNameToWidget(drawarea->shell, "*MainSaveFileDialog"))
            XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*MainSaveImageButton"), True);
        if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveFileDialog"))
            XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*ZoomSaveImageButton"), True);
        return;
        }


    XmStringGetLtoR(file_callback->value, XmSTRING_DEFAULT_CHARSET, &file);

    filename = (char *) XtCalloc(1, strlen(file) + 20);

    if(w == XtNameToWidget(drawarea->shell, "*GetFileDialog"))
        strcpy(filename, "File is: ");
    if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveFileDialog")
          || w ==  XtNameToWidget(drawarea->shell, "*MainSaveFileDialog"))
        strcpy(filename, "Image saved in: ");

    strncat(filename, file, strlen(file));

    if(w == XtNameToWidget(drawarea->shell, "*GetFileDialog"))
        {

        clearWindow(w, drawarea);

        if(fp1 != -1)
            close(fp1);

        if((fp1 = open(file, O_RDWR)) == -1)
            {
            message_display(drawarea, BADFILE);
            fp1 = open(file, O_RDONLY);
            }


        drawarea->bitsPerPixel = 0;

        }

    if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveFileDialog")
        || w ==  XtNameToWidget(drawarea->shell, "*MainSaveFileDialog"))
        {
        if((fp2 = open(file, O_RDWR | O_CREAT | O_TRUNC, 0666)) == -1)
            {
            message_display(drawarea, BADFILE);
            strcpy(filename, "Could not save selected image");
            }
        else
            {
            saveFile(w, drawarea, fp2);
            close(fp2);
            }
      
        }
 
    file_message = XmStringCreateLtoR(filename, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, file_message); n++;
    XtSetValues(XtNameToWidget(drawarea->shell, "*FileName"), args, n);

    XmStringFree(file_message);
    XtFree((char *)file);
    XtFree((char *)filename);

    /*
     *  Destroy the widget after unmanaging it as the dialog is
     *  recreated the next time a retrieve is requested.
     *  Unexpected behavior results from this if you don't.
     */
     

    if(w == XtNameToWidget(drawarea->shell, "*GetFileDialog"))
        XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*FileRetrieveButton"), True);
    if(w == XtNameToWidget(drawarea->shell, "*MainSaveFileDialog"))
        XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*MainSaveImageButton"), True);
    if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveFileDialog"))
        XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                             "*ZoomSaveImageButton"), True);
    XtUnmanageChild(w);
    XtDestroyWidget(w);


    return;
}

void clearWindow(w, drawarea)
Widget w;
struct draw *drawarea;
{

    Arg args[20];
    Cardinal n;

    Widget zoomDialog;
    Widget zoomArea;

    Dimension width, height;

    /*
     *     Clear the display window and background pixmap to present a 
     *     clean slate to the user.
     */

    XClearWindow(XtDisplay(w), XtWindow(drawarea->graphics));
    XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc,
             BlackPixelOfScreen(XtScreen(drawarea->shell)));

    XFillRectangle(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, 0, 0, 512, 512);

    XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc,
             WhitePixelOfScreen(XtScreen(drawarea->shell)));

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    if(image != NULL)
        {
        XFree((char *)image);
        XtFree((char *)display_data);
        image = NULL;
        display_data = NULL;
        }

    if(magnified_image != NULL)
        {
        XFree((char *)magnified_image);
        XtFree((char *)zoom_data);
        magnified_image = NULL;
        zoom_data = NULL;
        }

    if(subimage != NULL)
        {
        XFree((char *)subimage);
        XtFree((char *)sub_sample_sonar);
        subimage = NULL;
        sub_sample_sonar = NULL;
        if(XtIsRealized(XtNameToWidget(drawarea->shell, "*ZoomDialog")))
            {
            XClearWindow(XtDisplay(zoomDialog), XtWindow(zoomArea));
            XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                    BlackPixelOfScreen(XtScreen(drawarea->shell)));

            n = 0;
            XtSetArg(args[n], XmNwidth, &width); n++;
            XtSetArg(args[n], XmNheight, &height); n++;
            XtGetValues(zoomArea, args, n);


            XFillRectangle(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                    drawarea->zoom_gc, 0, 0, width, height);

            XSetForeground(XtDisplay(drawarea->shell), drawarea->zoom_gc,
                    WhitePixelOfScreen(XtScreen(drawarea->shell)));

            }

        n = 0;
        XtSetArg(args[n], XmNwidth, 50); n++;
        XtSetArg(args[n], XmNheight, 50); n++;
        XtSetValues(zoomArea, args, n);
        }

    return;

}


void saveFile(w, drawarea, fp2)
Widget w;
struct draw *drawarea;
int fp2;
{


    unsigned int width;
    unsigned int magnified_width;
    unsigned int height;
    unsigned int magnified_height;

    int outbytes;
    int number_of_scans;
    int data_offset;
    int a, b, i, j, x, y, z;
    int displayDataPad;
    int height_offset, width_offset;
    int tempInbytes;

    long position, filePosition, currentPosition;

    unsigned char *zdata;
    unsigned char *zoom_sonar;
    char information[100];

    void saveMessage();
    void message_display();

    if(w == XtNameToWidget(drawarea->shell, "*ZoomSaveFileDialog"))
        {
        x = drawarea->base_x * data_reducer;
        y = drawarea->base_y * data_reducer;

        width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
        height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

        displayDataPad = ((512 * data_reducer) - datasize) / 2;

        height_offset = scansize * (drawarea->top_ping + y);
        width_offset = headsize + (x - displayDataPad);

        zoom_sonar = (unsigned char *) XtCalloc(width * height, 
                                                      sizeof(unsigned char));
        sonar_data = (unsigned char *) XtCalloc(width, sizeof(unsigned char));

        currentPosition = lseek(fp1, 0L, SEEK_CUR);

        for(i = 0; i < height; i++)
            {
            filePosition = (long) (height_offset + (scansize * i))
                                                             + width_offset;
            position = lseek(fp1, filePosition, SEEK_SET);
            tempInbytes = read(fp1, sonar_data, width);

            for(j = 0; j < width; j++)
               zoom_sonar[(i * width) + j] = sonar_data[j];

            }

        position = lseek(fp1, currentPosition, SEEK_SET);
        XtFree((char *)sonar_data);

        if(magnified_image != NULL)
            {

            magnified_height = height * drawarea->magnify;
            magnified_width = width * drawarea->magnify;

            zdata = (unsigned char *)
                      XtCalloc(1, magnified_width * magnified_height);

            for(i = 0, y = 0; i < height; i++, y += drawarea->magnify)
                for(a = 0; a < drawarea->magnify; a++)
                    for(j = 0, z = 0; j < width; j++, z+=drawarea->magnify)
                        for(b = 0; b < drawarea->magnify; b++)
                            {
                            zdata[((y + a) * magnified_width) + z + b] =
                              zoom_sonar[(i * width) + j];
                            }


            outbytes = write(fp2,zdata,magnified_height * magnified_width); 

            if(outbytes != magnified_height * magnified_width)
                message_display(drawarea, BADWRITE);                

            sprintf(information, "Width of saved image = %d pixels.\nHeight of saved image = %d pixels.", (int) magnified_width, (int) magnified_height);
            saveMessage(drawarea, information);

            XtFree((char *)zdata);

            }
        else
            {

            outbytes = write(fp2,zoom_sonar,height * width);

            if(outbytes != height * width)
                message_display(drawarea, BADWRITE);

            fprintf(stderr, "Width of saved image = %d\n", width);
            fprintf(stderr, "Height of saved image = %d\n", height);

            sprintf(information, "Width of saved image = %d pixels.\nHeight of saved image = %d pixels.\n", (int) width, (int) height);
            saveMessage(drawarea, information);

            }

        XtFree((char *)zoom_sonar);

        }

    
    if(w == XtNameToWidget(drawarea->shell, "*MainSaveFileDialog"))
        {
        if(headsize == 0)
            number_of_scans = scansize;
        else
            number_of_scans = 512 * data_reducer;

        data_offset = headsize;

        sonar_data = (unsigned char *) XtCalloc(scansize,sizeof(unsigned char));

        currentPosition = lseek(fp1, 0L, SEEK_CUR);
        position = lseek(fp1, (long) (scansize * drawarea->top_ping), SEEK_SET);

        for(i = 0; i < number_of_scans; i++)
            {
            tempInbytes = read(fp1, sonar_data, scansize);
            outbytes = write(fp2, &sonar_data[data_offset], datasize);

            if(outbytes != datasize)
                {
                message_display(drawarea, BADWRITE);
                i = number_of_scans;
                }

            }

        XtFree((char *)sonar_data);

        position = lseek(fp1, currentPosition, SEEK_SET);

        sprintf(information, "Width of saved image = %d pixels.\nHeight of saved image = %d pixels.\n", datasize, number_of_scans);
        saveMessage(drawarea, information);

        }

    return;

}

void saveMessage(drawarea, info)
struct draw *drawarea;
char *info;
{

    Arg args[20];
    Cardinal n;

    Widget saveDialog;
    Widget saveBox;
    Widget button;

    void closeSaveMessage();

    int answer;

    n = 0;
    XtSetArg(args[n], XmNtitle, "Saved File Dimensions"); n++;
    saveDialog = XtCreatePopupShell("SaveDialog",
              xmDialogShellWidgetClass, drawarea->shell, args, n);

    n = 0;
    XtSetArg(args[n], XmNmessageString, XmStringCreateLtoR(info,
               XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
    saveBox = XtCreateWidget("SaveBox",
         xmMessageBoxWidgetClass, saveDialog, args, n);

    button = XmMessageBoxGetChild(saveBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(button);

    button = XmMessageBoxGetChild(saveBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(button);

    XtAddCallback(saveBox, XmNokCallback, 
                                    closeSaveMessage, &answer);

    XtManageChild(saveBox);

    answer = 0;

    while(answer == 0)
         XtAppProcessEvent(drawarea->topLevelApp, XtIMAll);

    XtPopdown(saveDialog);
    XtDestroyWidget(saveDialog);


}

void closeSaveMessage(w,client_data,callback_data)
Widget        w;
XtPointer     client_data;
XtPointer     callback_data;
{

    int *answer = (int *) client_data;

    *answer = 1;

    return;
}
