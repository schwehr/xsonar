
#include "showimage2.h"

void set_file_type(w,client_data,callback_data)
Widget           w;
XtPointer          client_data;
XtPointer      callback_data; 
{

     Arg args[40];
     int n;

     struct draw *drawarea = (struct draw * ) client_data;

    /*
     *    Set the proper file type depending on who's calling here.
     * 
     *    Set various buttons to false or true with regard to data file type.
     *    (for now...)
     */


     if(w == XtNameToWidget(drawarea->shell, "*QmipsButton"))
         {
         drawarea->fileType = QMIPS;
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                                  "*BitShiftButton"), True);
         }
     else if(w == XtNameToWidget(drawarea->shell, "*UsgsButton"))
         {
         drawarea->fileType = USGS;

         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                         "*FeatureRemoveButton"), TRUE);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*PortAltButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*StbdAltButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomPortAltButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomStbdAltButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,"*ZoomWriteAlts"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                         "*ZoomCorrectAltButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                         "*TelemetryButton"), True);
         }
     else if(w == XtNameToWidget(drawarea->shell, "*RasterButton"))
          drawarea->fileType = RASTER;
     else
          drawarea->fileType = UNDEFINED;

     if(w == XtNameToWidget(drawarea->shell, "*UsgsButton") || 
         w == XtNameToWidget(drawarea->shell, "*UndefinedButton") ||
         w == XtNameToWidget(drawarea->shell, "*RasterButton"))
         {
         XtUnmanageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftFrame"));
         XtUnmanageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftScale"));
         XtUnmanageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftFrame"));
         XtUnmanageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftScale"));

         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                                 "*BitShiftButton"), False);
        }

     if(w == XtNameToWidget(drawarea->shell, "*QmipsButton") || 
         w == XtNameToWidget(drawarea->shell, "*UndefinedButton") ||
         w == XtNameToWidget(drawarea->shell, "*RasterButton"))
         {
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                         "*FeatureRemoveButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*PortAltButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*PortAltOnButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*PortAltOffButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*StbdAltButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*StbdAltOnButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell, 
                                               "*StbdAltOffButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomPortAltButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomPortAltOnButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomPortAltOffButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomStbdAltButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomStbdAltOnButton"), True);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomStbdAltOffButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomCorrectAltButton"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*ZoomWriteAlts"), False);
         XtSetSensitive(XtNameToWidget(drawarea->shell,
                                              "*TelemetryButton"), False);
         }


     return;

}

