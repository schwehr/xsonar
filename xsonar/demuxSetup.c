/*
 *    
 *
 *    Dialog for selecting demux options.
 *    Revised setup.c to reflect this.
 *
 *    Written for Motif by:
 *    William W. Danforth
 *    U.S. Geological Survey
 *    Woods Hole, MA 02543
 *
 *    Dec 27, 2001
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>

#include <map.h>
#include "xsonar.h"

#include "sonar_struct.h"

void closeDemuxSetupDialog(Widget w, XtPointer client_data,XtPointer callback_data);
void getDemuxToggleValues(Widget w, XtPointer client_data, XtPointer call_data);
void getDemuxScaleValues(Widget w, XtPointer client_data, XtPointer call_data);
void getDemuxTextValues(Widget w, XtPointer client_data, XtPointer call_data);
/*void tvgRampFileSelect();*/

void demuxSetup(Widget w, XtPointer client_data, UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;

    char temp_text_value[30];

    Widget demuxSetupDialog;
    Widget demuxSetupForm;
    /*Widget demuxSetupHolder; */

    Widget separator1;
    Widget separator2;
    Widget separator3;

    Widget demuxTextRowColumn;
    Widget bitAndNormalizeLabel;
    Widget bitAndNormalizeRowColumn1;
    Widget bitAndNormalizeRowColumn2;
    Widget removeRampToggleHolder;
    Widget removeRampOptionsHolder;
    Widget bitShiftScaleHolder;

    Widget alongTrackLabel;
    Widget alongTrackText;
    Widget acrossTrackLabel;
    Widget acrossTrackText;
    Widget portOffsetLabel;
    Widget portOffsetText;
    Widget stbdOffsetLabel;
    Widget stbdOffsetText;
    Widget rangeLabel;
    Widget rangeText;
    Widget scanSkipLabel;
    Widget scanSkipText;
    Widget portNormalizeLabel;
    Widget portNormalizeText;
    Widget portMaxNormalizeLabel;
    Widget portMaxNormalizeText;
    Widget stbdNormalizeLabel;
    Widget stbdNormalizeText;
    Widget stbdMaxNormalizeLabel;
    Widget stbdMaxNormalizeText;

    Widget removeRampLabel;
    Widget removeRampPortStartLabel;
    Widget removeRampPortStartText;
    Widget removeRampPortEndLabel;
    Widget removeRampPortEndText;
    Widget removeRampStbdStartLabel;
    Widget removeRampStbdStartText;
    Widget removeRampStbdEndLabel;
    Widget removeRampStbdEndText;
    Widget removeRampRangeStartPortLabel;
    Widget removeRampRangeStartPortText;
    Widget removeRampRangeStartStbdLabel;
    Widget removeRampRangeStartStbdText;
        
    Widget eightBitToggle;
    Widget sixteenBitToggle;
        
    Widget normalizeToggle;
    Widget bitShiftToggle;

    Widget removeRampPortToggle;
    Widget removeRampStbdToggle;
    Widget removeRampTextToggle;

    Widget removeRampStbdLabel;
    Widget removeRampStbdText;

    Widget removeRampPortLabel;
    Widget removeRampPortText;

    Widget stbdBitShiftScale;
    Widget portBitShiftScale;

    /*Widget demuxSetupRowColumn; */
    Widget demuxSetupLabel;

    Widget buttonRowColumn;
    /*Widget onToggle;*/
    /*Widget offToggle;*/

    Widget doneButton;
    Widget cancelButton;

    Arg args[30];
    /*Status status;*/
    Cardinal n;

    /*int cancelCheck;*/
    int doneAnswer;
    int cancelAnswer;

    /*Dimension shellx;*/
    /*Dimension shelly;*/
    /*Dimension shellWidth;*/
    /*Dimension shellHeight;*/
    /*Dimension dialogHeight;*/
    /*Dimension dialogWidth;*/

    /*void closeDemuxSetupDialog(Widget w, XtPointer client_data, XmAnyCallbackStruct callback_data);*/

    /*
     *    De-sensitize the calling widget's button and begin creating
     *    the dialog:  This dialog will contain a form for several toggles,
     *    an area to select the filter kernel size, and a push button
     *    row-column to apply or cancel the filtering function.

     */

    XtSetSensitive(w, FALSE);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
    demuxSetupDialog = XtCreatePopupShell("DemuxSetupDialog",
               transientShellWidgetClass, sonar->toplevel, args, n);

    n = 0;
    XtSetArg(args[n], XmNallowResize, True); n++;
    demuxSetupForm = XtCreateWidget("DemuxSetupForm", xmFormWidgetClass,
                  demuxSetupDialog, args, n);
    XtManageChild(demuxSetupForm);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("Demultiplexing Range and Filter Options",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    demuxSetupLabel = XtCreateWidget("DemuxSetupLabel",
                             xmLabelWidgetClass, demuxSetupForm, args, n);
    XtManageChild(demuxSetupLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, demuxSetupLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 5); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++;
    /*
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    */
    XtSetArg(args[n], XmNisAligned, True); n++;
    demuxTextRowColumn = XtCreateWidget("DemuxTextRowColumn",
                xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(demuxTextRowColumn);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    acrossTrackLabel = XtCreateWidget("AcrossTrackLabel",
                    xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(acrossTrackLabel);
    
    n = 0;
    acrossTrackText = XtCreateWidget("AcrossTrackText",
                        xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(acrossTrackText);

     itoa(sonar->across_track, temp_text_value);
     XmTextSetString(acrossTrackText, temp_text_value);

     n = 0;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     alongTrackLabel = XtCreateWidget("AlongTrackLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(alongTrackLabel);

     n = 0;
     alongTrackText = XtCreateWidget("AlongTrackText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(alongTrackText);
 
     itoa(sonar->along_track, temp_text_value);
     XmTextSetString(alongTrackText, temp_text_value);

     n = 0;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     stbdOffsetLabel = XtCreateWidget("StbdOffsetLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(stbdOffsetLabel);

    n = 0;
     stbdOffsetText = XtCreateWidget("StbdOffsetText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(stbdOffsetText);

     itoa(sonar->stbdOffset, temp_text_value);
     XmTextSetString(stbdOffsetText, temp_text_value);

     n = 0;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     portOffsetLabel = XtCreateWidget("PortOffsetLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(portOffsetLabel);

     n = 0;
     portOffsetText = XtCreateWidget("PortOffsetText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(portOffsetText);
     if(sonar->portOffset)

     itoa(sonar->portOffset, temp_text_value);
     XmTextSetString(portOffsetText, temp_text_value);

     n = 0;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
     rangeLabel = XtCreateWidget("RangeLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
     XtManageChild(rangeLabel);

    n = 0;
    rangeText = XtCreateWidget("RangeText",
                        xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(rangeText);

    itoa(sonar->meterRange, temp_text_value);
    XmTextSetString(rangeText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    scanSkipLabel = XtCreateWidget("ScanSkipLabel",
                        xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(scanSkipLabel);

    n = 0;
    XtSetArg(args[n], XmNwidth, 5); n++;
    scanSkipText = XtCreateWidget("ScanSkipText",
                        xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(scanSkipText);

    itoa(sonar->skipScans, temp_text_value);
    XmTextSetString(scanSkipText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    portNormalizeLabel = XtCreateWidget("PortNormalizeLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(portNormalizeLabel);

    n = 0;
    portNormalizeText = XtCreateWidget("PortNormalizeText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(portNormalizeText);

    itoa(sonar->portNormalizeValue, temp_text_value);
    XmTextSetString(portNormalizeText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    stbdNormalizeLabel = XtCreateWidget("StbdNormalizeLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(stbdNormalizeLabel);

    n = 0;
    stbdNormalizeText = XtCreateWidget("StbdNormalizeText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(stbdNormalizeText);

    itoa(sonar->stbdNormalizeValue, temp_text_value);
    XmTextSetString(stbdNormalizeText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    portMaxNormalizeLabel = XtCreateWidget("PortMaxNormalizeLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(portMaxNormalizeLabel);

    n = 0;
    portMaxNormalizeText = XtCreateWidget("PortMaxNormalizeText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(portMaxNormalizeText);

    itoa(sonar->portMaxNormalizeValue, temp_text_value);
    XmTextSetString(portMaxNormalizeText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    stbdMaxNormalizeLabel = XtCreateWidget("StbdMaxNormalizeLabel",
                         xmLabelWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(stbdMaxNormalizeLabel);

    n = 0;
    stbdMaxNormalizeText = XtCreateWidget("StbdMaxNormalizeText",
                         xmTextWidgetClass, demuxTextRowColumn, args, n);
    XtManageChild(stbdMaxNormalizeText);

    itoa(sonar->stbdMaxNormalizeValue, temp_text_value);
    XmTextSetString(stbdMaxNormalizeText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, demuxTextRowColumn); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator1 = XtCreateWidget("Separator1",
                   xmSeparatorWidgetClass, demuxSetupForm, args, n);
    XtManageChild(separator1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, 
         XmStringCreateLtoR("Input Bit Type and Normalization", 
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    bitAndNormalizeLabel = 
        XtCreateWidget("BitAndNormalizeLabel", xmLabelWidgetClass,
                                     demuxSetupForm, args, n);
    XtManageChild(bitAndNormalizeLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, bitAndNormalizeLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    bitAndNormalizeRowColumn1 = XtCreateWidget("bitAndNormalizeRowColumn1",
                xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(bitAndNormalizeRowColumn1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Input 8 bit",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
     eightBitToggle = XtCreateWidget("EightBitToggle",
          xmToggleButtonWidgetClass, bitAndNormalizeRowColumn1, args, n);
     XtManageChild(eightBitToggle);
     if(sonar->eightBit)
          XmToggleButtonSetState(eightBitToggle, True, False);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Input 16 bit",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
     sixteenBitToggle = XtCreateWidget("SixteenBitToggle",
          xmToggleButtonWidgetClass, bitAndNormalizeRowColumn1, args, n);
     XtManageChild(sixteenBitToggle);
     if(sonar->sixteenBit)
          XmToggleButtonSetState(sixteenBitToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, bitAndNormalizeLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, bitAndNormalizeRowColumn1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    bitAndNormalizeRowColumn2 = XtCreateWidget("bitAndNormalizeRowColumn2",
                xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(bitAndNormalizeRowColumn2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Normalize Image",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
     normalizeToggle = XtCreateWidget("NormalizeToggle",
           xmToggleButtonWidgetClass, bitAndNormalizeRowColumn2, args, n);
     XtManageChild(normalizeToggle);
     if(sonar->normalize)
          XmToggleButtonSetState(normalizeToggle, True, False);

     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Use bit shift",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
     bitShiftToggle = XtCreateWidget("BitShiftToggle",
          xmToggleButtonWidgetClass, bitAndNormalizeRowColumn2, args, n);
     XtManageChild(bitShiftToggle);
     if(sonar->bitShift)
          XmToggleButtonSetState(bitShiftToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, bitAndNormalizeRowColumn1); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNradioBehavior, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    bitShiftScaleHolder = XtCreateWidget("BitShiftScaleHolder",
                        xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(bitShiftScaleHolder);

    n = 0;
    XtSetArg(args[n], XmNtitleString, 
                 XmStringCreateLtoR("Port Bit Shifter",
                 XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 12); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleWidth, 150); n++;
    XtSetArg(args[n], XmNvalue, sonar->portBitShift); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    portBitShiftScale = XtCreateWidget("PortBitShiftScale",
                        xmScaleWidgetClass, bitShiftScaleHolder, args, n);
    XtManageChild(portBitShiftScale);

    n = 0;
    XtSetArg(args[n], XmNtitleString, 
                 XmStringCreateLtoR("Stbd Bit Shifter",
                 XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNmaximum, 12); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNscaleWidth, 150); n++;
    XtSetArg(args[n], XmNvalue, sonar->stbdBitShift); n++;
    XtSetArg(args[n], XmNscaleMultiple, 1); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    stbdBitShiftScale = XtCreateWidget("StbdBitShiftScale",
                         xmScaleWidgetClass, bitShiftScaleHolder, args, n);
    XtManageChild(stbdBitShiftScale);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, bitShiftScaleHolder); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator2 = XtCreateWidget("Separator2",
                   xmSeparatorWidgetClass, demuxSetupForm, args, n);
    XtManageChild(separator2);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
         XmStringCreateLtoR("TVG Removal Options:",
         XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    removeRampLabel = XtCreateWidget("RemoveRampLabel",
                             xmLabelWidgetClass, demuxSetupForm, args, n);
    XtManageChild(removeRampLabel);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, removeRampLabel); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNradioBehavior, False); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    removeRampToggleHolder = XtCreateWidget("RemoveRampToggleHolder",
                        xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(removeRampToggleHolder);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Remove TVG Ramp Port",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    removeRampPortToggle = XtCreateWidget("RemoveRampPortToggle",
          xmToggleButtonWidgetClass, removeRampToggleHolder, args, n);
    XtManageChild(removeRampPortToggle);
    if(sonar->removeRampPortFlag)
          XmToggleButtonSetState(removeRampPortToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Remove TVG Ramp Stbd",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    removeRampStbdToggle = XtCreateWidget("RemoveRampStbdToggle",
          xmToggleButtonWidgetClass, removeRampToggleHolder, args, n);
    XtManageChild(removeRampStbdToggle);
    if(sonar->removeRampStbdFlag)
          XmToggleButtonSetState(removeRampStbdToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Use Ramp Text Model",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    removeRampTextToggle = XtCreateWidget("RemoveRampTextToggle",
          xmToggleButtonWidgetClass, removeRampToggleHolder, args, n);
    XtManageChild(removeRampTextToggle);
    if(sonar->getCurveModelText)
          XmToggleButtonSetState(removeRampTextToggle, True, False);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, removeRampToggleHolder); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNradioBehavior, False); n++;
    XtSetArg(args[n], XmNnumColumns, 8); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    removeRampOptionsHolder = XtCreateWidget("RemoveRampOptionsHolder",
                        xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(removeRampOptionsHolder);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Port Start Scan",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampPortStartLabel = XtCreateWidget("RemoveRampPortStartLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortStartLabel);

    n = 0;
    XtSetArg(args[n], XmNwidth, 2); n++;
    XtSetArg(args[n], XmNcolumns, 2); n++;
    removeRampPortStartText = XtCreateWidget("RemoveRampPortStartText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortStartText);
    itoa(sonar->startScanForRemoveRampPort, temp_text_value);
    XmTextSetString(removeRampPortStartText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Port End Scan",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampPortEndLabel = XtCreateWidget("RemoveRampPortEndLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortEndLabel);

    n = 0;
    XtSetArg(args[n], XmNwidth, 2); n++;
    removeRampPortEndText = XtCreateWidget("RemoveRampPortEndText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortEndText);
    itoa(sonar->endScanForRemoveRampPort, temp_text_value);
    XmTextSetString(removeRampPortEndText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Stbd Start Scan",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampStbdStartLabel = XtCreateWidget("RemoveRampStbdStartLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdStartLabel);

    n = 0;
    removeRampStbdStartText = XtCreateWidget("RemoveRampStbdStartText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdStartText);
    itoa(sonar->startScanForRemoveRampStbd, temp_text_value);
    XmTextSetString(removeRampStbdStartText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Stbd End Scan",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampStbdEndLabel = XtCreateWidget("RemoveRampStbdEndLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdEndLabel);

    n = 0;
    removeRampStbdEndText = XtCreateWidget("RemoveRampStbdEndText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdEndText);
    itoa(sonar->endScanForRemoveRampStbd, temp_text_value);
    XmTextSetString(removeRampStbdEndText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Port Pivot (DN)",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampPortLabel = XtCreateWidget("RemoveRampPortLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortLabel);

    n = 0;
    removeRampPortText = XtCreateWidget("RemoveRampPortText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampPortText);
    itoa(sonar->removeRampPortPivot, temp_text_value);
    XmTextSetString(removeRampPortText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Ramp Stbd Pivot (DN)",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampStbdLabel = XtCreateWidget("RemoveRampStbdLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdLabel);

    n = 0;
    removeRampStbdText = XtCreateWidget("RemoveRampStbdText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampStbdText);
    itoa(sonar->removeRampStbdPivot, temp_text_value);
    XmTextSetString(removeRampStbdText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Start Range Port",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampRangeStartPortLabel = XtCreateWidget("RemoveRampRangeStartPortLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampRangeStartPortLabel);

    n = 0;
    removeRampRangeStartPortText = XtCreateWidget("RemoveRampRangeStartPortText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampRangeStartPortText);
    itoa(sonar->removeRampRangeStartPort, temp_text_value);
    XmTextSetString(removeRampRangeStartPortText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Start Range Stbd",
                                   XmSTRING_DEFAULT_CHARSET)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    removeRampRangeStartStbdLabel = XtCreateWidget("RemoveRampRangeStartStbdLabel",
                         xmLabelWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampRangeStartStbdLabel);

    n = 0;
    removeRampRangeStartStbdText = XtCreateWidget("RemoveRampRangeStartStbdText",
                         xmTextWidgetClass, removeRampOptionsHolder, args, n);
    XtManageChild(removeRampRangeStartStbdText);
    itoa(sonar->removeRampRangeStartStbd, temp_text_value);
    XmTextSetString(removeRampRangeStartStbdText, temp_text_value);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, removeRampOptionsHolder); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    separator3 = XtCreateWidget("Separator3",
                   xmSeparatorWidgetClass, demuxSetupForm, args, n);
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
    XtSetArg(args[n], XmNisAligned, True); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    buttonRowColumn = XtCreateWidget("ButtonRowColumn",
                xmRowColumnWidgetClass, demuxSetupForm, args, n);
    XtManageChild(buttonRowColumn);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("    Done    ", XmSTRING_DEFAULT_CHARSET)); n++;
    doneButton = XtCreateWidget("DoneButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(doneButton);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
        XmStringCreateLtoR("   Cancel   ", XmSTRING_DEFAULT_CHARSET)); n++;
    cancelButton = XtCreateWidget("CancelButton",
                xmPushButtonWidgetClass, buttonRowColumn, args, n);
    XtManageChild(cancelButton);

    /*
     *    Button Callbacks
     */

    XtAddCallback(doneButton, XmNactivateCallback, 
                                       closeDemuxSetupDialog, sonar);
    XtAddCallback(cancelButton, XmNactivateCallback, 
                                       closeDemuxSetupDialog, sonar);

    /*
     *    Text Callbacks
     */

    XtAddCallback(acrossTrackText, XmNvalueChangedCallback, 
                                       getDemuxTextValues, sonar);

    XtAddCallback(alongTrackText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(stbdOffsetText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(portOffsetText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(rangeText, XmNvalueChangedCallback, 
                                       getDemuxTextValues, sonar);

    XtAddCallback(scanSkipText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(portNormalizeText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(stbdNormalizeText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(portMaxNormalizeText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(stbdMaxNormalizeText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampPortText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampStbdText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampPortStartText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampPortEndText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampStbdStartText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampStbdEndText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampRangeStartStbdText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    XtAddCallback(removeRampRangeStartPortText, XmNvalueChangedCallback,
                                       getDemuxTextValues, sonar);

    /*
     *    Toggle Callbacks
     */

    XtAddCallback(eightBitToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(sixteenBitToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(normalizeToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(bitShiftToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(removeRampPortToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(removeRampStbdToggle, XmNvalueChangedCallback,
                                       getDemuxToggleValues, sonar);

    XtAddCallback(removeRampTextToggle, XmNvalueChangedCallback,
                                       tvgRampFileSelect, sonar);
    /*
     *    Scale Slider Callbacks
     */

    XtAddCallback(stbdBitShiftScale, XmNvalueChangedCallback,
                                     getDemuxScaleValues, sonar);

    XtAddCallback(portBitShiftScale, XmNvalueChangedCallback,
                                     getDemuxScaleValues, sonar);

    XtRealizeWidget(demuxSetupDialog);

   /*
    *  Get the dimensions of the top level shell widget in order to pop
    *  up this dialog in the center of the top level shell.


    XtVaGetValues(sonar->toplevel, XmNx, &shellx, NULL);
    XtVaGetValues(sonar->toplevel, XmNy, &shelly, NULL);
    XtVaGetValues(sonar->toplevel, XmNwidth, &shellWidth, NULL);
    XtVaGetValues(sonar->toplevel, XmNheight, &shellHeight, NULL);
    XtVaGetValues(demuxSetupDialog, XmNwidth, &dialogWidth, NULL);
    XtVaGetValues(demuxSetupDialog, XmNheight, &dialogHeight, NULL);


    shellx = (shellWidth / 2) + shellx;
    shellx -= (dialogWidth / 2);
    shelly = (shellHeight / 2) + shelly;
    shelly -= (dialogHeight / 2);

    n = 0;
    XtSetArg(args[n], XmNx, shellx); n++;
    XtSetArg(args[n], XmNy, shelly); n++;
    XtSetValues(demuxSetupDialog, args, n);

    */

    XtPopup(demuxSetupDialog, XtGrabNone);

    doneAnswer = 0;
    cancelAnswer = 0;

    while(doneAnswer == 0 && cancelAnswer == 0)
        {
        XtAppProcessEvent(sonar->toplevel_app, XtIMAll);
        XtVaGetValues(cancelButton, XmNuserData, &doneAnswer, NULL);
        XtVaGetValues(doneButton, XmNuserData, &cancelAnswer, NULL);
        }


    XtPopdown(demuxSetupDialog);
    XtDestroyWidget(demuxSetupDialog);

    XtSetSensitive(w, True);

    return;


}

void closeDemuxSetupDialog(Widget w, XtPointer client_data,
			   UNUSED  XtPointer callback_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    int answer;

    char message[255];
    char temp_text_value[30];
    /*void messages();*/

    if(sonar->portNormalizeValue > sonar->portMaxNormalizeValue)
        {
        sprintf(message,"Port normalize value is greater than max value\n");
        strcat(message, "Setting port max value to port normalize value.\n");
	sonar->portMaxNormalizeValue = sonar->portNormalizeValue;
        itoa(sonar->portMaxNormalizeValue, temp_text_value);
        XmTextSetString(XtNameToWidget(sonar->toplevel, 
            "*PortMaxNormalizeText"), temp_text_value);
        messages(sonar, message);
        }
    else if(sonar->stbdNormalizeValue > sonar->stbdMaxNormalizeValue)
        {
        sprintf(message,"Stbd normalize value is greater than max value\n");
        strcat(message, "Setting stbd max value to stbd normalize value.\n");
	sonar->stbdMaxNormalizeValue = sonar->stbdNormalizeValue;
        itoa(sonar->stbdMaxNormalizeValue, temp_text_value);
        XmTextSetString(XtNameToWidget(sonar->toplevel, 
            "*StbdMaxNormalizeText"), temp_text_value);
        messages(sonar, message);
        }
    else
        {
        answer = 1;
        XtVaSetValues(w, XmNuserData, answer, NULL);
        }

    return;
}

void getDemuxToggleValues(Widget w, XtPointer client_data, 
			  UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = w;
    XmString calling_name;
    XmString compare_name;

    /*int status;*/

    /*int Get_swath();*/

    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("EightBitToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->eightBit++;
        else
            sonar->eightBit--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("SixteenBitToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->sixteenBit++;
        else
            sonar->sixteenBit--;
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("NormalizeToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            {
            sonar->normalize = 1;
            sonar->bitShift = 0;
            }
        else
            {
            sonar->normalize = 0;
            sonar->bitShift = 1;
            }
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampTextToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->getCurveModelText = 1;
        else
            sonar->getCurveModelText = 0;
printf("ramp text flag is %d\n", sonar->getCurveModelText);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampPortToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->removeRampPortFlag = 1;
        else
            sonar->removeRampPortFlag = 0;
printf("ramp port flag is %d\n", sonar->removeRampPortFlag);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampStbdToggle",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        if(XmToggleButtonGetState(calling_widget))
            sonar->removeRampStbdFlag = 1;
        else
            sonar->removeRampStbdFlag = 0;
printf("ramp stbd flag is %d\n", sonar->removeRampStbdFlag);
        }
    XmStringFree(compare_name);

    XmStringFree(calling_name);

    return;

}
    
    

void getDemuxScaleValues(Widget w, XtPointer client_data, 
			 UNUSED  XtPointer call_data)
{

    
    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = w;
    XmString calling_name;
    XmString compare_name;
    int temp;

    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("PortBitShiftScale",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
          {
          XmScaleGetValue(calling_widget, &temp);
          sonar->portBitShift = (int) temp;
          }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("StbdBitShiftScale",
                                             XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
         {
         XmScaleGetValue(calling_widget, &temp);
         sonar->stbdBitShift = (int) temp;
         }

    XmStringFree(compare_name);

    XmStringFree(calling_name);
    return;
}


void getDemuxTextValues(Widget w, XtPointer client_data, 
			UNUSED  XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget calling_widget = (Widget) w;
    /*Widget parent_widget;*/

    XmString calling_name;
    XmString compare_name;

    char *text;

    calling_name = XmStringCreateLtoR(XtName(calling_widget),
                                    XmSTRING_DEFAULT_CHARSET);

    compare_name = XmStringCreateLtoR("RemoveRampRangeStartPortText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->removeRampRangeStartPort = atoi(text);
        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampRangeStartStbdText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->removeRampRangeStartStbd = atoi(text);
        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampPortStartText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->startScanForRemoveRampPort = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampPortEndText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->endScanForRemoveRampPort = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampStbdStartText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->startScanForRemoveRampStbd = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampStbdEndText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->endScanForRemoveRampStbd = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampPortText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->removeRampPortPivot = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RemoveRampStbdText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->removeRampStbdPivot = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("PortNormalizeText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->portNormalizeValue = atoi(text);

        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("PortMaxNormalizeText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->portMaxNormalizeValue = atoi(text);
        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("StbdNormalizeText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->stbdNormalizeValue = atoi(text);
        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("StbdMaxNormalizeText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->stbdMaxNormalizeValue = atoi(text);
        XtFree(text);
        }
    XmStringFree(compare_name);

    compare_name = XmStringCreateLtoR("RangeText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->meterRange = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("ScanSkipText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->skipScans = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("ScaleLineText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->scaleInt = atof(text);
        XtFree(text);
        }

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
    compare_name = XmStringCreateLtoR("PortOffsetText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->portOffset = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("StbdOffsetText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->stbdOffset = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("AlongTrackText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->along_track = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);
    compare_name = XmStringCreateLtoR("AcrossTrackText",
                                    XmSTRING_DEFAULT_CHARSET);
    if(XmStringByteCompare(calling_name, compare_name))
        {
        text = XmTextGetString(calling_widget);
        sonar->across_track = atoi(text);
        XtFree(text);
        }

    XmStringFree(compare_name);

    XmStringFree(calling_name);
}

