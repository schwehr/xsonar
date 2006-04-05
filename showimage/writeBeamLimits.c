#include <unistd.h>

#include "showimage2.h"
#include "externals.h"

void writeBeamLimits(w, client_data, call_data)
Widget w;
XtPointer             client_data;
XmAnyCallbackStruct   *call_data;
{
    struct draw *drawarea = (struct draw * ) client_data;

    int i;
    int num_scans;

    off_t offset;
    off_t position;

    void message_display();

    XmUpdateDisplay(drawarea->shell);

    if(subimage == NULL || fp1 == -1)
        {
        message_display(drawarea, NOZOOM);
        return;
        }

    num_scans = inbytes / scansize;

    position = lseek(fp1, 0L, SEEK_CUR); 
    offset = position - inbytes;
    position = lseek(fp1, offset, SEEK_SET); 

    for(i = 0; i < num_scans; i++)
        {
        write(fp1, scans[i], 256);
        position = lseek(fp1, (off_t) datasize, SEEK_CUR);
        }

    return;

}
