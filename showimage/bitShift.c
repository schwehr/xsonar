

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void displayBitShifter(w,client_data,callback_data)
Widget                 w;
XtPointer              client_data;
XtPointer              callback_data; 
{
    struct draw *drawarea = (struct draw *) client_data;

    if(w == XtNameToWidget(drawarea->shell, "*BitShiftShow"))
        {
        XtManageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftFrame"));
        XtManageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftScale"));
        XtManageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftFrame"));
        XtManageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftScale"));
        XtSetSensitive(XtNameToWidget(drawarea->shell, "*BitShiftShow"), False);
        XtSetSensitive(XtNameToWidget(drawarea->shell, "*BitShiftHide"), True);
        }
    else
        {
        XtUnmanageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftFrame"));
        XtUnmanageChild(XtNameToWidget(drawarea->shell, "*PortBitShiftScale"));
        XtUnmanageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftFrame"));
        XtUnmanageChild(XtNameToWidget(drawarea->shell, "*StbdBitShiftScale"));
        XtSetSensitive(XtNameToWidget(drawarea->shell, "*BitShiftShow"), True);
        XtSetSensitive(XtNameToWidget(drawarea->shell, "*BitShiftHide"), False);
        }

    return;
}
