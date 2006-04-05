
#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

void showBeamCorrectionLimits(drawarea, displayFlag)
struct draw *drawarea;
int displayFlag;
{

    int number_of_scans;
    int i, j;
    int displayDataPad;

    Widget portShowBeamLimitOnButton;
    Widget stbdShowBeamLimitOnButton;

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

    XmUpdateDisplay(drawarea->shell);

    displayDataPad = (512 - datasize / data_reducer) / 2;

    alt_to_pixel = (float) (512 - displayDataPad * 2) / (float)swath_width; 

    number_of_scans = (inbytes / scansize);

    portShowBeamLimitOnButton = XtNameToWidget(drawarea->shell, "*PortShowBeamLimitOnButton");
    stbdShowBeamLimitOnButton = XtNameToWidget(drawarea->shell, "*StbdShowBeamLimitOnButton");

    zoomDialog = XtNameToWidget(drawarea->shell, "*ZoomDialog");
    zoomArea = XtNameToWidget(drawarea->shell, "*ZoomArea");

    if(drawarea->visual->class == PseudoColor)
        XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc, 191);

    if(drawarea->visual->class == TrueColor)
        XSetForeground(XtDisplay(drawarea->shell), drawarea->main_gc, 
                           drawarea->grayPixels[50]);

    if(displayFlag == PORTBEAMLIMIT || displayFlag == STBDBEAMLIMIT)
        {

        for(i = 0, j = 0; i < ((number_of_scans / data_reducer) - 1); i++, j+=data_reducer)
            {

            if(!XtIsSensitive(portShowBeamLimitOnButton))
                {
                pix_alt = 256 + (int)(scans[j]->portBeamLimit * alt_to_pixel);
                next_pix_alt = 256 + (int)(scans[j+data_reducer]->portBeamLimit
                                                * alt_to_pixel);

                XDrawLine(XtDisplay(drawarea->shell),
                    XtWindow(drawarea->graphics),
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));

                XDrawLine(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
                }

            if(!XtIsSensitive(stbdShowBeamLimitOnButton))
                {
                pix_alt = 256 - (int)(scans[j]->stbdBeamLimit * alt_to_pixel);
                next_pix_alt = 256 - (int)(scans[j+data_reducer]->stbdBeamLimit
                                                * alt_to_pixel);

                XDrawLine(XtDisplay(drawarea->shell),
                    XtWindow(drawarea->graphics),
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
    
                XDrawLine(XtDisplay(drawarea->shell), drawarea->imagePixmap,
                    drawarea->main_gc, pix_alt, i, next_pix_alt, (i + 1));
                }

            }

        }


    if(displayFlag == ZOOMPORTBEAMLIMIT || displayFlag == ZOOMSTBDBEAMLIMIT)
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
            if(displayFlag == ZOOMPORTBEAMLIMIT)
                {
                pix_alt = dist_to_nadir +
                        (int)(scans[j]->portBeamLimit * zoom_alt_to_pixel);
                next_pix_alt = dist_to_nadir +
                        (int)(scans[j + 1]->portBeamLimit * zoom_alt_to_pixel);

                XDrawLine(XtDisplay(zoomDialog),XtWindow(zoomArea),
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);

                XDrawLine(XtDisplay(zoomDialog), drawarea->zoomPixmap,
                    drawarea->main_gc, pix_alt, i,
                    next_pix_alt, i + drawarea->magnify);

                }

            if(displayFlag == ZOOMSTBDBEAMLIMIT)
                {
                pix_alt = dist_to_nadir -
                        (int)(scans[j]->stbdBeamLimit * zoom_alt_to_pixel);
                next_pix_alt = dist_to_nadir -
                        (int)(scans[j + 1]->stbdBeamLimit * zoom_alt_to_pixel);
/*
fprintf(stderr, "i = %d, scans[%d] = %f\n",i, j,  scans[j]->portBeamLimit);
fprintf(stderr, "i = %d, scans[%d] = %f\n", i + drawarea->magnify, j+1, scans[j+1]->portBeamLimit);
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
