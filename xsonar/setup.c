/* 
 *  setup.c
 *
 *  Set up parameters for processing a sonar file.
 *  Modified December 2001:  Removed middle section
 *  of old setup dialog containing along and across
 *  track, range and scan skip info, sonar inpuyt
 *  bit type toggle, among others and put these into
 *  a separate dialog, demuxSetup.c.  Setup dialog
 *  was getting too big....
 *
 *  Written for Motif by:
 *  William W. Danforth
 *  U.S. Geological Survey
 *
 *  July 11, 1992
 *  Updated December 27, 2001
 */

#include <unistd.h>

#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/PushB.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>

#include <map.h>
#include "xsonar.h"

#include "sonar_struct.h"

void closeSwathDialog(Widget w, XtPointer client, XtPointer call);

void Close_setup(Widget w, XtPointer client_data, XtPointer call_data);
/*void Get_toggle_values(Widget w, XtPointer client_data, XtPointer call_data);*/
void Get_scale_values(Widget w, XtPointer client_data, XtPointer call_data);
void Get_text(Widget w, XtPointer client_data, XtPointer call_data);
void testFocus(Widget w, XtPointer client, XtPointer call);
void setFileType(Widget w, XtPointer client, XtPointer call);
int Get_swath(XtPointer client_data);

void setup(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget setupDialog;
    Widget mainForm;
    Widget scaleFrame;
    Widget scaleHolder;
    Widget swathBulletin;
    Widget swathHolder;
    Widget processBulletin;
    Widget processHolder;
    Widget secondToggleHolder;
    Widget secondToggleFrame;
    Widget separator1;
    Widget separator2;
    /*Widget separator3;*/
    Widget separator4;
    /*Widget separator5;*/
    Widget menuBarSeparator;

    Widget optionsPullDownMenu;
    Widget optionsButton;
    Widget beamButton;
    Widget demuxSetupButton;
    Widget swathButton;
    Widget updateHeaderButton;
    Widget imageHeaderButton;
    Widget altFindButton;

    Widget fishAziToggle;
    Widget centralLonScale;
    Widget navIntervalScale;
    Widget magneticDeclScale;

    Widget fileMenuBar;
    Widget fileButton;
    Widget filePullDownMenu;
    Widget fileRetrieveButton;
    Widget fileTypeButton;
    Widget fileNameLabel;
    Widget exitButton;

    Widget fileTypePullDownMenu;
    Widget usgsFileButton;
    Widget eggFileButton;
    Widget qmipsFileButton;
    Widget segyFileButton;
    Widget midasFileButton;
    Widget rasterFileButton;

    Widget stretchFrame;
    Widget stretchHolder;
    Widget stretchLabel;
    Widget lowStretchScale;
    Widget midStretchScale;
    Widget hiStretchScale;

    Widget swathWidthLabel;
    Widget specialToggle;
    Widget oneHundredMeterToggle;
    Widget twoHundredMeterToggle;
    Widget fiveHundredMeterToggle;
    Widget thousandMeterToggle;
    Widget twentyFiveHundredMeterToggle;
    Widget fiveThousandMeterToggle;

    Widget processingLabel;
    Widget demultiplexToggle;
    Widget mergeNavToggle;
    Widget slantRangeToggle;
    Widget destripeToggle;
    Widget beamPatternToggle;
    Widget linearStretchToggle;
    Widget equalizationToggle;

    Widget navUnitsLabel;
    Widget latLonToggle;
    Widget eastNorthToggle;
    Widget gridTypeLabel;
    Widget mercatorToggle;
    Widget utmToggle;
    Widget mercatorGridInt;

    Widget scaleLabel;
    Widget scaleText;

    /*Widget testShell;*/

    XmString file_message;
    Arg args[30];
    /*Status status;*/
    Cardinal n;

    char temp_text_value[30];
    char *filename;
    /*char warningmessage[100];*/

    Dimension height;

    /*int test;*/


#if 0
    void File_select();
    void BeamPatternSetup();
    void demuxSetup();
    void FindAltSetup();
    void updateSwath();
    void rasterBounds();
    
    void itoa();
    void messages();
#endif
    /*
     *  Set the calling widget (graphicsButton) to be insensitive.
     */

    XtSetSensitive(w, FALSE);

     /*
      *  Check to see if the setup shell has been realized.  If so,
      *  just pop it back up and return.

Took this out due to bugs in the SONY Motif.  If the menu bar is activated,
no text input will happen in the text widgets.  Only happens on the sony...

    testShell = XtNameToWidget(sonar->toplevel, "*SetupDialog");
    if(testShell != NULL)
        {
        XtPopup(testShell, XtGrabNone);
        return;
        }
      */

    /*
     *     Otherwise, create the shell...
     */

    XmUpdateDisplay(sonar->toplevel);


    n = 0;
    XtSetArg(args[n], XmNallowShellResize, False); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNmaxWidth, 487); n++;
    XtSetArg(args[n], XmNmaxHeight, 585); n++;
    XtSetArg(args[n], XmNminHeight, 585); n++;
    XtSetArg(args[n], XmNminWidth, 487); n++;
    setupDialog = XtCreatePopupShell("SetupDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    mainForm = XtCreateWidget("MainForm", xmFormWidgetClass,
                  setupDialog, args, n);
    XtManageChild(mainForm);


    /*
     *    Create a menu pulldown for getting the input file,
     *     storing the setup values, and exiting.
     */
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     fileMenuBar = XmCreateMenuBar(mainForm, "FileMenuBar", args, n);
     XtManageChild(fileMenuBar);


     n = 0;
     XtSetArg(args[n], XmNmnemonic, 'F'); n++;
     fileButton = XtCreateWidget("FileButton",
                    xmCascadeButtonWidgetClass, fileMenuBar, args, n);
     XtManageChild(fileButton);

     filePullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                "FilePullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, filePullDownMenu); n++;
    XtSetValues(fileButton, args, n);

    fileRetrieveButton = XtCreateWidget("FileRetrieveButton",
                   xmPushButtonWidgetClass, filePullDownMenu, NULL, 0);
    XtManageChild(fileRetrieveButton);

    fileTypeButton = XtCreateWidget("FileTypeButton",
                    xmCascadeButtonWidgetClass, filePullDownMenu, NULL, 0);
    XtManageChild(fileTypeButton);

    fileTypePullDownMenu = XmCreatePulldownMenu(filePullDownMenu,
                                "FileTypePullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, fileTypePullDownMenu); n++;
    XtSetValues(fileTypeButton, args, n);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("Xsonar", XmSTRING_DEFAULT_CHARSET)); n++;
    usgsFileButton = XtCreateWidget("UsgsFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(usgsFileButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("Raster", XmSTRING_DEFAULT_CHARSET)); n++;
    rasterFileButton = XtCreateWidget("RasterFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(rasterFileButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("EGG", XmSTRING_DEFAULT_CHARSET)); n++;
    eggFileButton = XtCreateWidget("EGGFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(eggFileButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("QMIPS", XmSTRING_DEFAULT_CHARSET)); n++;
    qmipsFileButton = XtCreateWidget("QmipsFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(qmipsFileButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("SEGY", XmSTRING_DEFAULT_CHARSET)); n++;
    segyFileButton = XtCreateWidget("SegyFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(segyFileButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
              XmStringCreateLtoR("EGG Midas", XmSTRING_DEFAULT_CHARSET)); n++;
    midasFileButton = XtCreateWidget("MidasFileButton",
                xmPushButtonWidgetClass, fileTypePullDownMenu, args, n);
    XtManageChild(midasFileButton);


    exitButton = XtCreateWidget("ExitButton",
                xmPushButtonWidgetClass, filePullDownMenu, NULL, 0);
    XtManageChild(exitButton);

    n = 0;
    XtSetArg(args[n], XmNmnemonic, 'O'); n++;
    optionsButton = XtCreateWidget("OptionsButton",
                xmCascadeButtonWidgetClass, fileMenuBar, args, n);
    XtManageChild(optionsButton);

    optionsPullDownMenu = XmCreatePulldownMenu(fileMenuBar,
                                "OptionsPullDownMenu", NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, optionsPullDownMenu); n++;
    XtSetValues(optionsButton, args, n);

    demuxSetupButton = XtCreateWidget("DemuxSetupButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(demuxSetupButton);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, optionsPullDownMenu); n++;
    XtSetValues(optionsButton, args, n);

    beamButton = XtCreateWidget("BeamButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, NULL, 0);
    XtManageChild(beamButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Update Swath",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    swathButton = XtCreateWidget("SwathButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, args, n);
    XtManageChild(swathButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Update Header",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    updateHeaderButton = XtCreateWidget("UpdateHeaderButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, args, n);
    XtManageChild(updateHeaderButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Image Header",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    imageHeaderButton = XtCreateWidget("ImageHeaderButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, args, n);
    XtManageChild(imageHeaderButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Find Altitude",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    altFindButton = XtCreateWidget("AltFindButton",
                xmPushButtonWidgetClass, optionsPullDownMenu, args, n);
    XtManageChild(altFindButton);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, fileMenuBar); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    menuBarSeparator = XtCreateWidget("MenuBarSeparator",
                xmSeparatorWidgetClass, mainForm, args, n);
    XtManageChild(menuBarSeparator);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, menuBarSeparator); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("File is: ",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    fileNameLabel = XtCreateWidget("FileNameLabel",
                    xmLabelWidgetClass, mainForm, args, n);
    XtManageChild(fileNameLabel);
    

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, fileNameLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                xmSeparatorWidgetClass, mainForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    scaleFrame = XtCreateWidget("ScaleFrame",
                    xmFrameWidgetClass, mainForm, args, n);
    XtManageChild(scaleFrame);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    scaleHolder = XtCreateWidget("ScaleHolder",
                              xmRowColumnWidgetClass, scaleFrame, args, n);
    XtManageChild(scaleHolder);


    n = 0;
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, scaleFrame); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    swathBulletin = XtCreateWidget("SwathBulletin",
                    xmBulletinBoardWidgetClass, mainForm, args, n);
    XtManageChild(swathBulletin);


    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, swathBulletin); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    processBulletin = XtCreateWidget("ProcessBulletin",
                    xmBulletinBoardWidgetClass, mainForm, args, n);
    XtManageChild(processBulletin);


    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, swathBulletin); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                xmSeparatorWidgetClass, mainForm, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    stretchFrame = XtCreateWidget("StretchFrame",
                        xmFrameWidgetClass, mainForm, args, n);
    XtManageChild(stretchFrame);

    n = 0;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    stretchHolder = XtCreateWidget("stretchHolder",
                        xmRowColumnWidgetClass, stretchFrame, args, n);
    XtManageChild(stretchHolder);

    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, stretchFrame); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    secondToggleFrame = XtCreateWidget("SecondToggleFrame",
                        xmFrameWidgetClass, mainForm, args, n);
    XtManageChild(secondToggleFrame);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    secondToggleHolder = XtCreateWidget("SecondToggleHolder",
                    xmRowColumnWidgetClass, secondToggleFrame, args, n);
    XtManageChild(secondToggleHolder);

    /*
     *    Create the various scales that will set the params
     *     in the MainSonar struct for other programs to use.
     */


    /*
     *    Swath width
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    swathWidthLabel = XtCreateWidget("SwathWidthLabel",
                    xmLabelWidgetClass, swathBulletin, NULL, 0);
    XtManageChild(swathWidthLabel);
    

    n = 0;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    swathHolder = XtCreateWidget("SwathHolder",
                        xmRowColumnWidgetClass, swathBulletin, args, n);
    XtManageChild(swathHolder);


    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("100 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    oneHundredMeterToggle = XtCreateWidget("OneHundredMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(oneHundredMeterToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("200 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    twoHundredMeterToggle = XtCreateWidget("TwoHundredMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(twoHundredMeterToggle);
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("500 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    fiveHundredMeterToggle = XtCreateWidget("FiveHundredMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(fiveHundredMeterToggle);
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("1000 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    thousandMeterToggle = XtCreateWidget("ThousandMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(thousandMeterToggle);
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("2500 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    twentyFiveHundredMeterToggle = 
                XtCreateWidget("TwentyFiveHundredMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(twentyFiveHundredMeterToggle);
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("5000 meter",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    fiveThousandMeterToggle = XtCreateWidget("FiveThousandMeterToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(fiveThousandMeterToggle);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("User Defined",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    specialToggle = XtCreateWidget("SpecialToggle",
                xmToggleButtonWidgetClass, swathHolder, args, n);
    XtManageChild(specialToggle);

    switch(sonar->swath)
        {
        case 100:
            XmToggleButtonSetState(oneHundredMeterToggle, True, False);
            break;

        case 200:
            XmToggleButtonSetState(twoHundredMeterToggle, True, False);
            break;
    
        case 500:
            XmToggleButtonSetState(fiveHundredMeterToggle, True, False);
            break;
    
        case 1000:
            XmToggleButtonSetState(thousandMeterToggle, True, False);
            break;
    
        case 2500:
            XmToggleButtonSetState(twentyFiveHundredMeterToggle, True, False);
            break;

        case 5000:
            XmToggleButtonSetState(fiveThousandMeterToggle, True, False);
            break;

        case 0:
            break;

        default:
            XmToggleButtonSetState(specialToggle, True, False);
            break;

        }
    

    /*
     *    Various input for Mr. demux.
     */

    /*
     *    Format of input navigation.
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    navUnitsLabel = XtCreateWidget("NavUnitsLabel",
                    xmLabelWidgetClass, scaleHolder, args, n);
    XtManageChild(navUnitsLabel);

    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Lat, Lon",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    latLonToggle = XtCreateWidget("LatLonToggle",
                xmToggleButtonWidgetClass, scaleHolder, args, n);
    XtManageChild(latLonToggle);
    
    if(sonar->latlon_flag)
        XmToggleButtonSetState(latLonToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString,
                        XmStringCreateLtoR("Eastings, Northings",
                        XmSTRING_DEFAULT_CHARSET)); n++;
    eastNorthToggle = XtCreateWidget("EastNorthToggle",
                xmToggleButtonWidgetClass, scaleHolder, args, n);
    XtManageChild(eastNorthToggle);
    if(sonar->eastnorth_flag)
        XmToggleButtonSetState(eastNorthToggle, True, False);

    /*
     *    Extract fish azimuth.
     */

    separator4 = XtCreateWidget("Separator4",
                xmSeparatorWidgetClass, scaleHolder, NULL, 0);
    XtManageChild(separator4);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
                    XmStringCreateLtoR("Use Fish Azimuth",
                    XmSTRING_DEFAULT_CHARSET)); n++;
    fishAziToggle = XtCreateWidget("FishAziToggle",
                xmToggleButtonWidgetClass, scaleHolder, args, n);
    XtManageChild(fishAziToggle);
    if(sonar->fish_azi_flag)
        XmToggleButtonSetState(fishAziToggle, True, False);

    /*
     *    Magnetic Declination and    Nav Interval
     */

    n = 0;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Navigation Interval (minutes)", 
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 1500); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,     (int)(sonar->navInterval * 100.0)); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNdecimalPoints, 2); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    navIntervalScale = XtCreateWidget("NavIntervalScale",
                    xmScaleWidgetClass, scaleHolder, args, n);
    XtManageChild(navIntervalScale);

    n = 0;
    XtSetArg(args[n], XmNtitleString,
                    XmStringCreateLtoR("Magnetic Declination", 
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 90); n++;
    XtSetArg(args[n], XmNminimum, -90); n++;
    XtSetArg(args[n], XmNvalue,     sonar->magnetic_declination); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    magneticDeclScale = XtCreateWidget("MagneticDeclScale",
                    xmScaleWidgetClass, scaleHolder, args, n);
    XtManageChild(magneticDeclScale);

    /*
     *    Stretch parameters.
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    stretchLabel = XtCreateWidget("StretchLabel",
                    xmLabelWidgetClass, stretchHolder, args, n);
    XtManageChild(stretchLabel);
    
    n = 0;
    XtSetArg(args[n], XmNtitleString, XmStringCreateLtoR("Low DN", 
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,     sonar->stretch_params[0]); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    lowStretchScale = XtCreateWidget("LowStretchScale",
                    xmScaleWidgetClass, stretchHolder, args, n);
    XtManageChild(lowStretchScale);

    n = 0;
    XtSetArg(args[n], XmNtitleString, XmStringCreateLtoR("Mean DN", 
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,     sonar->stretch_params[1]); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    midStretchScale = XtCreateWidget("MidStretchScale",
                    xmScaleWidgetClass, stretchHolder, args, n);
    XtManageChild(midStretchScale);

    n = 0;
    XtSetArg(args[n], XmNtitleString, XmStringCreateLtoR("High DN", 
                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 255); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue,     sonar->stretch_params[2]); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    hiStretchScale = XtCreateWidget("HiStretchScale",
                    xmScaleWidgetClass, stretchHolder, args, n);
    XtManageChild(hiStretchScale);

    /*
     *    Select processing functions, add to a callback list
     */

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    processingLabel = XtCreateWidget("ProcessingLabel",
                    xmLabelWidgetClass, processBulletin, args, n);
    XtManageChild(processingLabel);
    
    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, False); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    processHolder = XtCreateWidget("ProcessHolder",
                    xmRowColumnWidgetClass, processBulletin, args, n);
    XtManageChild(processHolder);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Demultiplex",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    demultiplexToggle = XtCreateWidget("DemultiplexToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(demultiplexToggle);
    if(sonar->demuxFlag)
        XmToggleButtonSetState(demultiplexToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Merge Navigation",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    mergeNavToggle = XtCreateWidget("MergeNavToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(mergeNavToggle);
    if(sonar->navmerge_flag)
        XmToggleButtonSetState(mergeNavToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Slant Range",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    slantRangeToggle = XtCreateWidget("SlantRangeToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(slantRangeToggle);
    if(sonar->slant_flag)
        XmToggleButtonSetState(slantRangeToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Destripe",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    destripeToggle = XtCreateWidget("DestripeToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(destripeToggle);
    if(sonar->destripe_flag)
        XmToggleButtonSetState(destripeToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Beam Pattern",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    beamPatternToggle = XtCreateWidget("BeamPatternToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(beamPatternToggle);
    if(sonar->beam_flag)
        XmToggleButtonSetState(beamPatternToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Linear Stretch",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    linearStretchToggle = XtCreateWidget("LinearStretchToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(linearStretchToggle);
    if(sonar->linear_flag)
        XmToggleButtonSetState(linearStretchToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Equalization",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    equalizationToggle = XtCreateWidget("EqualizationToggle",
                xmToggleButtonWidgetClass, processHolder, args, n);
    XtManageChild(equalizationToggle);
    if(sonar->equalize_flag)
        XmToggleButtonSetState(equalizationToggle, True, False);
    

    /*
     *    Select some map parameters for output.
     */
    
    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    gridTypeLabel = XtCreateWidget("GridTypeLabel",
                    xmLabelWidgetClass, secondToggleHolder, args, n);
    XtManageChild(gridTypeLabel);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Mercator",
                            XmSTRING_DEFAULT_CHARSET)); n++;
    mercatorToggle = XtCreateWidget("MercatorToggle",
                xmToggleButtonWidgetClass, secondToggleHolder, args, n);
    XtManageChild(mercatorToggle);
    if(sonar->mercator_flag)
        XmToggleButtonSetState(mercatorToggle, True, False);
    
    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("UTM",
                        XmSTRING_DEFAULT_CHARSET)); n++;
    utmToggle = XtCreateWidget("UtmToggle",
                xmToggleButtonWidgetClass, secondToggleHolder, args, n);
    XtManageChild(utmToggle);
    if(sonar->utm_flag)
        XmToggleButtonSetState(utmToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtitleString, 
               XmStringCreateLtoR("Mercator Grid Interval", 
               XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 60); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNvalue, sonar->mer_grid_int); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    mercatorGridInt = XtCreateWidget("MercatorGridInt",
                    xmScaleWidgetClass, secondToggleHolder, args, n);
    XtManageChild(mercatorGridInt);

    n = 0;
    XtSetArg(args[n], XmNtitleString, XmStringCreateLtoR("Central Longitude", 
                                     XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 180); n++;
    XtSetArg(args[n], XmNminimum, -180); n++;
    XtSetArg(args[n], XmNvalue, sonar->c_lon); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    centralLonScale = XtCreateWidget("CentralLonScale",
                    xmScaleWidgetClass, secondToggleHolder, args, n);
    XtManageChild(centralLonScale);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    scaleLabel = XtCreateWidget("ScaleLabel",
                    xmLabelWidgetClass, secondToggleHolder, args, n);
    XtManageChild(scaleLabel);
    
    n = 0;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
    XtSetArg(args[n], XmNwidth, 20); n++;
    scaleText = XtCreateWidget("ScaleText",
                    xmTextWidgetClass, secondToggleHolder, args, n);
    XtManageChild(scaleText);
/*
    XtAddCallback(scaleText, XmNfocusCallback, testFocus, NULL);
*/
    itoa((int)sonar->scale, temp_text_value);
    XmTextSetString(scaleText, temp_text_value);

    /*
     *    Setup callbacks
     */


/*
    XtAddCallback(runButton, XmNactivateCallback, display, sonar);
    XtAddCallback(fileRetrieveButton, XmNactivateCallback,
                                        File_update, sonar);
*/

    XtAddCallback(exitButton, XmNactivateCallback, 
                                         Close_setup, sonar);
    XtAddCallback(demuxSetupButton, XmNactivateCallback, 
                                        demuxSetup,  sonar);
    XtAddCallback(beamButton, XmNactivateCallback, 
                                        BeamPatternSetup,  sonar);
    XtAddCallback(swathButton, XmNactivateCallback, 
                                         updateSwath, sonar);
    XtAddCallback(updateHeaderButton, XmNactivateCallback, 
                                         updateSwath, sonar);
    XtAddCallback(imageHeaderButton, XmNactivateCallback, 
                                         rasterBounds, sonar);
    XtAddCallback(altFindButton, XmNactivateCallback, 
                                         FindAltSetup, sonar);
    XtAddCallback(fileRetrieveButton,XmNactivateCallback, 
                                         File_select, sonar);
    XtAddCallback(oneHundredMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(twoHundredMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(fiveHundredMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(thousandMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(twentyFiveHundredMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(fiveThousandMeterToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(specialToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(latLonToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(eastNorthToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(fishAziToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(navIntervalScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(magneticDeclScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(lowStretchScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(midStretchScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(hiStretchScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(demultiplexToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(mergeNavToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(slantRangeToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(destripeToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(beamPatternToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(linearStretchToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(equalizationToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(mercatorToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(utmToggle, XmNvalueChangedCallback,
                                        Get_toggle_values, sonar);
    XtAddCallback(mercatorGridInt, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);
    XtAddCallback(centralLonScale, XmNvalueChangedCallback,
                                        Get_scale_values, sonar);

    /*
     *    Text callbacks
     */

    XtAddCallback(scaleText, XmNvalueChangedCallback, Get_text, sonar);

    XtAddCallback(usgsFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtAddCallback(rasterFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtAddCallback(eggFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtAddCallback(qmipsFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtAddCallback(segyFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtAddCallback(midasFileButton, XmNactivateCallback,
                                     setFileType, sonar);
    XtRealizeWidget(setupDialog);

    XtVaGetValues(swathBulletin, XmNheight, &height, NULL);

    n = 0;
    XtSetArg(args[n], XmNheight, height); n++;
    XtSetValues(scaleFrame, args, n);

    if(sonar->infile != NULL)
        {
        filename = (char *) XtCalloc(1, strlen(sonar->infile) + 10);
        strcpy(filename, "File is: ");

        strncat(filename, sonar->infile, strlen(sonar->infile));
        file_message = XmStringCreateLtoR(filename,
                            XmSTRING_DEFAULT_CHARSET);

        n = 0;
        XtSetArg(args[n], XmNlabelString, file_message); n++;
        XtSetValues(fileNameLabel, args, n);

        XmStringFree(file_message);
        XtFree(filename);

        }

    XtPopup(setupDialog, XtGrabNone);

}

void Get_toggle_values(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = w;
    XmString calling_name;
    XmString compare_name;

    int status;  /* FIX: status can be used uninitialized */


    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("SpecialToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
/*
        XmToggleButtonSetState(calling_widget, True, False);
*/
        if(XmToggleButtonGetState(calling_widget))
            status = Get_swath(sonar);

        if(!status)
            XmToggleButtonSetState(calling_widget, False, False);
        
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("LimitBeamOnToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->useBeamLimits = 1;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("LimitBeamOffToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->useBeamLimits = 0;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("FindAltOnToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->findAltFlag = 1;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("FindAltOffToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->findAltFlag = 0;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("OnToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->adjustBeam = TRUE;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("OffToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        if(XmToggleButtonGetState(calling_widget))
            sonar->adjustBeam = FALSE;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("OneHundredMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 100;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("TwoHundredMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 200;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("FiveHundredMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 500;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("ThousandMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 1000;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("TwentyFiveHundredMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 2500;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("FiveThousandMeterToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        sonar->swath = 5000;
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("FishAziToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->fish_azi_flag++;
        else
            sonar->fish_azi_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("UtmToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->utm_flag++;
        else
            sonar->utm_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("MercatorToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->mercator_flag++;
        else
            sonar->mercator_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("LatLonToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->latlon_flag++;
        else
            sonar->latlon_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("EastNorthToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->eastnorth_flag++;
        else
            sonar->eastnorth_flag--;
        }

    compare_name = XmStringCreateLtoR("DemultiplexToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->demuxFlag++;
        else
            sonar->demuxFlag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("MergeNavToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->navmerge_flag++;
        else
            sonar->navmerge_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("SlantRangeToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->slant_flag++;
        else
            sonar->slant_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("DestripeToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->destripe_flag++;
        else
            sonar->destripe_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("BeamPatternToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->beam_flag++;
        else
            sonar->beam_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("LinearStretchToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->linear_flag++;
        else
            sonar->linear_flag--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("EqualizationToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->equalize_flag++;
        else
            sonar->equalize_flag--;
        }
    XmStringFree(compare_name);
    XmStringFree(calling_name);

    return;

}
    
    

void Close_setup(UNUSED Widget w,
		 XtPointer client_data,
		 UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    Widget parentWidget;
    Widget setupWidget;

    parentWidget = XtNameToWidget(sonar->toplevel, "*SetupDialog");
    setupWidget = XtNameToWidget(sonar->toplevel, "*SetupButton");

    XtSetSensitive(setupWidget, True);

    XtPopdown(parentWidget);
    XtDestroyWidget(parentWidget);

    return;
}

void Get_scale_values(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{
    
    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = w;
    XmString calling_name;
    XmString compare_name;
    int temp;

    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("MercatorGridInt",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        XmScaleGetValue(calling_widget, &sonar->mer_grid_int);
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("MagneticDeclScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        XmScaleGetValue(calling_widget, &sonar->magnetic_declination);
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("NavIntervalScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        XmScaleGetValue(calling_widget, &temp);
        sonar->navInterval = (float) temp / 100.0;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("LowStretchScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        XmScaleGetValue(calling_widget, &sonar->stretch_params[0]);
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("MidStretchScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        XmScaleGetValue(calling_widget, &sonar->stretch_params[1]);
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("HiStretchScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        XmScaleGetValue(calling_widget, &sonar->stretch_params[2]);
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("CentralLonScale",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        XmScaleGetValue(calling_widget, &temp);
        sonar->c_lon = (double) temp;
        }
    XmStringFree(compare_name);

    XmStringFree(calling_name);
    return;
}


void Get_text(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = (Widget) w;
    /*Widget parent_widget;*/

    XmString calling_name;
    XmString compare_name; /* FIX: compare_name can be used uninitialized */

    char *text;

    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("ScaleText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->scale = atof(text);
        XtFree(text);
        }

    XmStringFree(compare_name);

    XmStringFree(calling_name);
}

int Get_swath(XtPointer client_data)
{

    MainSonar *sonar= (MainSonar *) client_data;

    Arg args[15];
    Cardinal n;

    Widget swathDialog;
    Widget swathTextHolder;
    Widget swathLabel;
    Widget swathText;
    Widget parentDialog;
    Widget separator1;
    Widget swathForm;
    Widget doneButton, cancelButton;
    Widget swathRowColumn;

    char temp_text_value[30];
    char warningmessage[100];

    int headsize;
    int infd;


    /*void messages(); */
    /*void itoa();*/

    headsize = HEADSIZE;     /*   defined in sonar_struct.h  */

    if((infd = open(sonar->infile, O_RDONLY)) == -1)
        {
        sprintf(warningmessage, "Error opening input file.\nPlease reselect a file.");
        messages(sonar, warningmessage);
        return(0);
        }

    if(sonar->fileType == XSONAR)
        {
        if(read (infd, &sonar->ping_header, headsize) != headsize)
            {
            sprintf(warningmessage, "Error reading first header");
            messages(sonar, warningmessage);
            close(infd);
            return(0);
            }
        }

    parentDialog = XtNameToWidget(sonar->toplevel, "*SetupDialog");

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    XtSetArg(args[n], XmNx, 50); n++;
    XtSetArg(args[n], XmNy, 50); n++;
    XtSetArg(args[n], XmNtitle, "Set Swath"); n++;
    swathDialog = XtCreatePopupShell("SwathDialog",
            transientShellWidgetClass, parentDialog, args, n);

    swathForm = XtCreateWidget("SwathForm", xmFormWidgetClass, swathDialog,
                     NULL, 0);
    XtManageChild(swathForm);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    swathTextHolder = XtCreateWidget("SwathTextHolder", xmRowColumnWidgetClass,
                     swathForm, args, n);
    XtManageChild(swathTextHolder);

    swathLabel = XtCreateWidget("SwathLabel",
        xmLabelWidgetClass, swathTextHolder, NULL, 0);
    XtManageChild(swathLabel);

    swathText = XtCreateWidget("SwathText",
                    xmTextWidgetClass, swathTextHolder, NULL, 0);
    XtManageChild(swathText);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, swathTextHolder); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                xmSeparatorWidgetClass, swathForm, args, n);
    XtManageChild(separator1);

    /*
     *   Create a menu for cancelling or ok'ing the filter selection.
     */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    swathRowColumn = XtCreateWidget("SwathRowColumn",
                xmRowColumnWidgetClass, swathForm, args, n);
    XtManageChild(swathRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("    Done    ", XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, swathRowColumn, args, n);
    XtManageChild(doneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("   Cancel   ", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, swathRowColumn, args, n);
    XtManageChild(cancelButton);


    XtAddCallback(doneButton, XmNactivateCallback, closeSwathDialog, sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, closeSwathDialog, sonar);

    XtRealizeWidget(swathDialog);
    XtPopup(swathDialog, XtGrabNone);

    if(sonar->fileType == XSONAR)
        itoa((int)sonar->ping_header.swath_width, temp_text_value);
    else
        itoa((int) 0, temp_text_value);

    XmTextSetString(swathText, temp_text_value);

    close(infd);

    return(1);


}


void closeSwathDialog(Widget w, XtPointer client, UNUSED XtPointer call)
{

    MainSonar *sonar = (MainSonar *) client;

    Widget parent_widget;

    char *text;

    if(!strcmp(XtName(w), "DoneButton"))
        {
        text = XmTextGetString(XtNameToWidget(sonar->toplevel, "*SwathText"));
        sonar->swath = atoi(text);
        XtFree(text);
        }

    parent_widget = XtNameToWidget(sonar->toplevel, "*SwathDialog");
    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

    return;
}


void setFileType(Widget w, XtPointer client, UNUSED XtPointer call)
{
    MainSonar *sonar = (MainSonar *) client;

    /*Arg args[20];*/
    /*int n;*/

    XmString callingWidget;
    XmString compareWidget;


    callingWidget = XmStringCreateLtoR(XtName(w), XmSTRING_DEFAULT_CHARSET);

    compareWidget = XmStringCreateLtoR("EGGFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
        sonar->fileType = EGG;
    XmStringFree(compareWidget);

    compareWidget = XmStringCreateLtoR("QmipsFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
          sonar->fileType = QMIPS;
    XmStringFree(compareWidget);

    compareWidget = XmStringCreateLtoR("SegyFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
          sonar->fileType = SEGY;
    XmStringFree(compareWidget);

    compareWidget = XmStringCreateLtoR("MidasFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
          sonar->fileType = EGGMIDAS;
    XmStringFree(compareWidget);

    compareWidget = XmStringCreateLtoR("UsgsFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
          sonar->fileType = XSONAR;
    XmStringFree(compareWidget);

    compareWidget = XmStringCreateLtoR("RasterFileButton",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(callingWidget, compareWidget))
          sonar->fileType = RASTER;
    XmStringFree(compareWidget);

    return;

}



void testFocus(Widget w, UNUSED XtPointer client, UNUSED XtPointer call)
{

    Arg args[10];
    Cardinal n;

    n = 0;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNoverrideRedirect, True); n++;
    XtSetValues(w, args, n);

    return;

}
