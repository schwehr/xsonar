

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void testScale(w,client_data,callback_data)
Widget                 w;
XtPointer              client_data;
XtPointer              callback_data; 
{
    struct draw *drawarea = (struct draw *) client_data;

        XmScaleGetValue(w, &drawarea->bitShift);


    return;
}
