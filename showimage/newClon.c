

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void newCentralLon(w,client_data,callback_data)
Widget                 w;
XtPointer              client_data;
XtPointer              callback_data; 
{

    struct draw *drawarea = (struct draw *) client_data;

    void getCentralLon();

    XtSetSensitive(w, False);

    getCentralLon(drawarea);

    XtSetSensitive(w, True);

    return;

}
