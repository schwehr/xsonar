/*
 *      Expose callback.
 */

#include "showimage2.h"
#include "externals.h"

void repaint_image(w,client_data,call_data)
Widget                 w;
XtPointer             client_data;
XtPointer                 call_data; 
{
    struct draw *drawarea = (struct draw * ) client_data;
    /*GC gc;*/
    unsigned int width, height;

    XSetWindowAttributes main_window_attr;  /* window attribute struct */
    unsigned long main_window_valuemask;    /* mask for attribute values */

    Widget zoomArea;
    Widget zoomDialog;

    width = ((drawarea->zoom_x - drawarea->base_x) + 1) * data_reducer;
    height = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");
    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");


    main_window_attr.colormap = drawarea->graphics_colormap;
    main_window_valuemask = CWColormap;

    if(w == XtNameToWidget(drawarea->shell, "*ReDisplayButton"))
        {
        XmUpdateDisplay(drawarea->shell);
        if(image != NULL)
            {

            XClearWindow(XtDisplay(drawarea->shell), 
                                          XtWindow(drawarea->graphics));

            XPutImage(XtDisplay(drawarea->shell), XtWindow(drawarea->graphics),
                    drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);
            XPutImage(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, image, 0, 0, 0, 0, 512, 512);

            }
        }
     else if(w == drawarea->graphics)
         {
         XmUpdateDisplay(drawarea->shell);


         XCopyArea(XtDisplay(drawarea->shell), drawarea->imagePixmap,
            XtWindow(drawarea->graphics), drawarea->main_gc,
            0, 0, 512, 512, 0, 0);
         }
    else if(w == zoomArea)
        {
        XmUpdateDisplay(XtNameToWidget(drawarea->shell, "*ZoomDialog"));
        if(subimage != NULL)
            {

            if(drawarea->magnify < 2)
                    XCopyArea(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                              XtWindow(zoomArea), drawarea->zoom_gc,
                              0, 0, width, height, 0, 0);
            else 
                if(magnified_image != NULL)
                    {
                    width *= drawarea->magnify;
                    height *= drawarea->magnify;

                    XCopyArea(XtDisplay(drawarea->shell), drawarea->zoomPixmap,
                              XtWindow(zoomArea), drawarea->zoom_gc,
                              0, 0, width, height, 0, 0);
                    }
            }
    
        }
    else
        ; /* do nothing */


    return;

}
