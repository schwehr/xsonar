
#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void showAltitude(drawarea, displayFlag)
struct draw *drawarea;
int displayFlag;
{

    int number_of_scans;
    int i, j;
    int offset;
    int parse_return;
    int displayDataPad;
    int ping;

    Widget portAltOnButton;
    Widget portAltOffButton;

    Widget stbdAltOnButton;
    Widget stbdAltOffButton;

    Widget zoomPortAltButton;
    Widget zoomStbdAltButton;
    Widget zoomDialog;
    Widget zoomArea;

    unsigned int dist_to_nadir;

    int zoomheight; 
    int pix_alt, next_pix_alt;

    float alt_to_pixel;
    float zoom_alt_to_pixel;

    void message_display();

    if(fp1 == -1)
        {
        message_display(drawarea, fp1);
        return;
        }

    if(image == NULL)
        {
        message_display(drawarea, NOSONAR);
        return;
        }

    offset = 0;

    XmUpdateDisplay(drawarea->shell);

    displayDataPad = (512 - datasize / data_reducer) / 2;

    alt_to_pixel = (float) (512 - displayDataPad * 2) / (float)swath_width; 

    number_of_scans = (inbytes / scansize);

    ping = drawarea->base_y * data_reducer;

    portAltOnButton = XtNameToWidget(drawarea->shell, "*PortAltOnButton");
    stbdAltOnButton = XtNameToWidget(drawarea->shell, "*StbdAltOnButton");

    zoomPortAltButton = XtNameToWidget(drawarea->shell, "*ZoomPortAltButton");
    zoomStbdAltButton = XtNameToWidget(drawarea->shell, "*ZoomStbdAltButton");
    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    if(drawarea->visual->class == PseudoColor)
        XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc, 191);

    if(drawarea->visual->class == TrueColor)
        XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc, 
                           drawarea->grayPixels[50]);

    if(displayFlag == PORTALT || displayFlag == STBDALT)
        {

        for(i = 0, j = 0; i < ((number_of_scans / data_reducer) - 1); i++, j+=data_reducer)
            {

            if(!XtIsSensitive(portAltOnButton))
                {
                pix_alt = 256 + (int)(scans[j]->alt * alt_to_pixel);
                next_pix_alt = 256 + (int)(scans[j+data_reducer]->alt
                                                * alt_to_pixel);

                XDrawLine(XtDisplay(drawarea->shell),
                    XtWindow(drawarea->graphics),
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));

                XDrawLine(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
                }

            if(!XtIsSensitive(stbdAltOnButton))
                {
                pix_alt = 256 - (int)(scans[j]->alt * alt_to_pixel);
                next_pix_alt = 256 - (int)(scans[j+data_reducer]->alt
                                                * alt_to_pixel);

                XDrawLine(XtDisplay(drawarea->shell),
                    XtWindow(drawarea->graphics),
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
    
                XDrawLine(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
                }

            }

        }


    if(displayFlag == ZOOMPORTALT || displayFlag == ZOOMSTBDALT)
        {

        zoom_alt_to_pixel = 1 / ((float)swath_width / datasize);
        zoomheight = ((drawarea->zoom_y - drawarea->base_y) + 1) * data_reducer;

        dist_to_nadir = (256 - drawarea->base_x) * data_reducer;

/*
fprintf(stderr, "zoom alt to pixel = %f\n", zoom_alt_to_pixel);
fprintf(stderr, "distance to nadir = %d\n", dist_to_nadir);
fprintf(stderr, "zoomheight = %d\n", zoomheight);
fprintf(stderr, "drawarea->base_y = %d\n", drawarea->base_y);
fprintf(stderr, "drawarea->zoom_y = %d\n", drawarea->zoom_y);
fprintf(stderr, "widget = %s\n",XtName(w) );
*/

        if(drawarea->magnify > 1)
            {
            zoomheight *= drawarea->magnify;
            dist_to_nadir *= drawarea->magnify;
            zoom_alt_to_pixel *= drawarea->magnify;
            }

        for(i = 1, j = drawarea->base_y * data_reducer;
            i <= (zoomheight - drawarea->magnify) &&  j < number_of_scans - 1; 
            j++, i+=drawarea->magnify)
            {
            if(displayFlag == ZOOMPORTALT)
                {
                pix_alt = dist_to_nadir +
                        (int)(scans[j]->alt * zoom_alt_to_pixel);
                next_pix_alt = dist_to_nadir +
                        (int)(scans[j + 1]->alt * zoom_alt_to_pixel);

                XDrawLine(XtDisplay(zoomDialog),XtWindow(zoomArea),
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);

                XDrawLine(XtDisplay(zoomDialog), drawarea->zoomPixmap,
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);

                }

            if(displayFlag == ZOOMSTBDALT)
                {
                pix_alt = dist_to_nadir -
                        (int)(scans[j]->alt * zoom_alt_to_pixel);
                next_pix_alt = dist_to_nadir -
                        (int)(scans[j + 1]->alt * zoom_alt_to_pixel);
/*
fprintf(stderr, "i = %d, scans[%d] = %f\n",i, j,  scans[j]->alt);
fprintf(stderr, "i = %d, scans[%d] = %f\n", i + drawarea->magnify, j+1, scans[j+1]->alt);
*/

                XDrawLine(XtDisplay(zoomDialog),XtWindow(zoomArea),
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);

                XDrawLine(XtDisplay(zoomDialog), drawarea->zoomPixmap,
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);
                }

            }


        }

    XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc,
               WhitePixelOfScreen(XtScreen(drawarea->shell)));

    return;
}
