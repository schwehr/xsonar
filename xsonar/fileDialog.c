/*
 *    fileDialog.c
 *
 *    Callback function for popping up a file selection dialog and retrieving
 *    the name of the selected file.
 *
 */

#include <unistd.h>

#include <Xm/ToggleB.h> /* XmToggleButtonGetState */
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>

#include "xsonar.h"

#include "sonar_struct.h"

void File_name(Widget w, XtPointer client_data, XtPointer callback_data);

void File_select(Widget w, XtPointer client_data,
		 UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    /*XmAnyCallbackStruct *callback = (XmAnyCallbackStruct *) callback_data;*/

    Arg args[5];
    Cardinal n;

    Widget filebox;
    Widget filedialog;

    /*XEvent event;*/

    char *inputFile;
    char *stringPtr;
    int answer;

    /*void runFunction();*/

    if(w == XtNameToWidget(sonar->toplevel, "*ComboDialog*ApplyButton"))
        {
        if(XmToggleButtonGetState(XtNameToWidget(sonar->toplevel, 
                                      "*ComboDialog*FlipToggle")))
            return;
        else
            {
            XtVaGetValues(w, XmNuserData, &inputFile, NULL);
            strcpy(inputFile, sonar->infile);
            }
        }

    XtSetSensitive(w, False);

    if(XtNameToWidget(sonar->toplevel, "*SetupDialog") != NULL)
        XmUpdateDisplay(XtNameToWidget(sonar->toplevel, "*SetupDialog"));

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    filedialog = XtCreatePopupShell("FileDialog",
                xmDialogShellWidgetClass, sonar->toplevel, args, n);

    if(w == XtNameToWidget(sonar->toplevel, "*ComboDialog*ApplyButton"))
        {
        n = 0;
        XtSetArg(args[n], XmNtitle, "Combine File"); n++;
        XtSetValues(filedialog, args, n);
        }

    stringPtr = strrchr(sonar->currentDirectory, '/');
    stringPtr++;
    if(*stringPtr != '*')
        strcat(sonar->currentDirectory, "*");
    n = 0;
    XtSetArg(args[n], XmNdirMask, XmStringCreateLtoR(sonar->currentDirectory, 
                           XmSTRING_DEFAULT_CHARSET)); n++;
    filebox = XtCreateWidget("FileBox", xmFileSelectionBoxWidgetClass,
                filedialog, args, n);

    XtAddCallback(filebox, XmNokCallback, File_name, sonar);
    XtAddCallback(filebox, XmNcancelCallback, File_name, sonar);
    XtManageChild(filebox);

    answer = 0;


    while(answer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(filebox, XmNuserData, &answer, NULL);
        }

    XtUnmanageChild(filebox);
    XtDestroyWidget(filedialog);

    if(XtNameToWidget(sonar->toplevel, "*SetupDialog") != NULL)
        XtSetSensitive(w, True);

    if(w == XtNameToWidget(sonar->toplevel, "*ComboDialog*ApplyButton"))
        {
        if(answer == 2)
            runFunction(w, sonar);
        else
            strcpy(sonar->infile, inputFile);
        }

    if(answer == 2)
        {
        strcpy(sonar->currentDirectory, sonar->infile);
        stringPtr = strrchr(sonar->currentDirectory, '/');
        stringPtr++;
        *stringPtr = '\0';
        }

    return;

}



void File_name( UNUSED  Widget w,
		XtPointer client_data, XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    XmFileSelectionBoxCallbackStruct *file_callback =
                (XmFileSelectionBoxCallbackStruct *) callback_data;

    XmString fileName;
    /*Widget parent_widget = w;*/
    Widget fileLabel;
    Widget dialog;
    Arg args[10];
    Cardinal n;

    int answer;
    int infd;
    int headsize;

    char *filename;
    char *tempFilePtr;
    char warningmessage[100];

    /*void messages();*/

    if(file_callback->reason == XmCR_CANCEL)
        {
        answer = 1;
        XtVaSetValues(XtNameToWidget(sonar->toplevel, 
                "*FileBox"), XmNuserData, answer, NULL);
        return;
        }
/*
    if(sonar->infile != NULL)
        {
        XtFree(sonar->infile);
        sonar->infile = NULL;
        }
*/

    XmStringGetLtoR(file_callback->value,
                    XmSTRING_DEFAULT_CHARSET, &tempFilePtr);

    strcpy(sonar->infile, tempFilePtr);
    XtFree(tempFilePtr);

    filename = (char *) XtCalloc(1, strlen(sonar->infile) + 10);
    strcpy(filename, "File is: ");
    strncat(filename, sonar->infile, strlen(sonar->infile));

    /*
     *   Check to make sure the input file can be opened for reading.
     */

    headsize = HEADSIZE;

    if((infd = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningmessage, 
                 "Error opening input file.\nPlease reselect a file.");
        messages(sonar, warningmessage);
        return;
        }

    if(read (infd, &sonar->ping_header, headsize) != headsize)
        {
        sprintf(warningmessage, "cannot read first header");
        messages(sonar, warningmessage);
        close(infd);
        return;
        }

    sonar->fileType = sonar->ping_header.fileType;


    close(infd);

    dialog = XtNameToWidget(sonar->toplevel, "*ComboDialog");

    /*
     *   If the combine files dialog initiated this selection, do not reset
     *   the filename in the setup dialog display.
     */

    if(dialog == NULL)
        {

        fileName = XmStringCreateLtoR(filename, XmSTRING_DEFAULT_CHARSET);
        fileLabel = XtNameToWidget(sonar->toplevel, "*FileNameLabel");
        if(fileLabel != NULL)
            {
            n = 0;
            XtSetArg(args[n], XmNlabelString, fileName); n++;
            XtSetValues(fileLabel, args, n);
            }

        XmStringFree(fileName);
        }
       

    answer = 2;
    XtVaSetValues(XtNameToWidget(sonar->toplevel, 
                "*FileBox"), XmNuserData, answer, NULL);

    return;
}

