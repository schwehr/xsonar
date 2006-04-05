/*    c_median()
 *
 *    sorts v[0]...v[n-1] in increasing order and returns the median.
 *    Modified from Kernighan & Ritchie, p. 58
 */

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"
#include <stdlib.h>
#include <sys/stat.h>          /*   For file attributes  */
#include <sys/types.h>

void setAltDrawMode(w,client_data,callback_data)
Widget                 w;
XtPointer             client_data;
XmAnyCallbackStruct     *callback_data;
{

    Widget portAltOnButton;
    Widget zoomPortAltOnButton;

    Widget portAltOffButton;
    Widget zoomPortAltOffButton;

    Widget stbdAltOnButton;
    Widget zoomStbdAltOnButton;

    Widget stbdAltOffButton;
    Widget zoomStbdAltOffButton;

    struct draw *drawarea = (struct draw * ) client_data;

    void showAltitude();

    portAltOnButton = XtNameToWidget(drawarea->shell, "*PortAltOnButton");
    zoomPortAltOnButton = XtNameToWidget(drawarea->shell, 
                                                    "*ZoomPortAltOnButton");

    portAltOffButton = XtNameToWidget(drawarea->shell, "*PortAltOffButton");
    zoomPortAltOffButton = XtNameToWidget(drawarea->shell, 
                                                    "*ZoomPortAltOffButton");

    stbdAltOnButton = XtNameToWidget(drawarea->shell, "*StbdAltOnButton");
    zoomStbdAltOnButton = XtNameToWidget(drawarea->shell, 
                                                    "*ZoomStbdAltOnButton");

    stbdAltOffButton = XtNameToWidget(drawarea->shell, "*StbdAltOffButton");
    zoomStbdAltOffButton = XtNameToWidget(drawarea->shell, 
                                                    "*ZoomStbdAltOffButton");


    if(w == portAltOnButton)
        {
        XtSetSensitive(portAltOffButton, True);
        XtSetSensitive(portAltOnButton, False);
        showAltitude(drawarea, PORTALT);
        }

    if(w == zoomPortAltOnButton)
        {
        XtSetSensitive(zoomPortAltOffButton, True);
        XtSetSensitive(zoomPortAltOnButton, False);
        showAltitude(drawarea, ZOOMPORTALT);
        }


    if(w == portAltOffButton)
        {
        XtSetSensitive(portAltOffButton, False);
        XtSetSensitive(portAltOnButton, True);
        }

    if(w == zoomPortAltOffButton)
        {
        XtSetSensitive(zoomPortAltOffButton, False);
        XtSetSensitive(zoomPortAltOnButton, True);
        }


    if(w == stbdAltOnButton)
        {
        XtSetSensitive(stbdAltOffButton, True);
        XtSetSensitive(stbdAltOnButton, False);
        showAltitude(drawarea, STBDALT);
        }

    if(w == zoomStbdAltOnButton)
        {
        XtSetSensitive(zoomStbdAltOffButton, True);
        XtSetSensitive(zoomStbdAltOnButton, False);
        showAltitude(drawarea, ZOOMSTBDALT);
        }

    if(w == stbdAltOffButton)
        {
        XtSetSensitive(stbdAltOffButton, False);
        XtSetSensitive(stbdAltOnButton, True);
        }

    if(w == zoomStbdAltOffButton)
        {
        XtSetSensitive(zoomStbdAltOffButton, False);
        XtSetSensitive(zoomStbdAltOnButton, True);
        }

    return;

}


void setBeamLimitDrawMode(w,client_data,callback_data)
Widget                 w;
XtPointer             client_data;
XmAnyCallbackStruct     *callback_data;
{

    Widget portShowBeamLimitOnButton;
    Widget zoomPortShowBeamLimitOnButton;

    Widget portShowBeamLimitOffButton;
    Widget zoomPortShowBeamLimitOffButton;

    Widget stbdShowBeamLimitOnButton;
    Widget zoomStbdShowBeamLimitOnButton;

    Widget stbdShowBeamLimitOffButton;
    Widget zoomStbdShowBeamLimitOffButton;

    struct draw *drawarea = (struct draw * ) client_data;

    void showBeamCorrectionLimits();

    portShowBeamLimitOnButton = XtNameToWidget(drawarea->shell, "*PortShowBeamLimitOnButton");
    zoomPortShowBeamLimitOnButton = XtNameToWidget(drawarea->shell,
                                                    "*ZoomPortShowBeamLimitOnButton");

    portShowBeamLimitOffButton = XtNameToWidget(drawarea->shell, 
                                                   "*PortShowBeamLimitOffButton");
    zoomPortShowBeamLimitOffButton = XtNameToWidget(drawarea->shell,
                                                   "*ZoomPortShowBeamLimitOffButton");

    stbdShowBeamLimitOnButton = XtNameToWidget(drawarea->shell, "*StbdShowBeamLimitOnButton");
    zoomStbdShowBeamLimitOnButton = XtNameToWidget(drawarea->shell,
                                                    "*ZoomStbdShowBeamLimitOnButton");

    stbdShowBeamLimitOffButton = XtNameToWidget(drawarea->shell, 
                                                    "*StbdShowBeamLimitOffButton");
    zoomStbdShowBeamLimitOffButton = XtNameToWidget(drawarea->shell,
                                                    "*ZoomStbdShowBeamLimitOffButton");

    if(w == portShowBeamLimitOnButton)
        {
        XtSetSensitive(portShowBeamLimitOffButton, True);
        XtSetSensitive(portShowBeamLimitOnButton, False);
        showBeamCorrectionLimits(drawarea, PORTBEAMLIMIT);
        }

    if(w == zoomPortShowBeamLimitOnButton)
        {
        XtSetSensitive(zoomPortShowBeamLimitOffButton, True);
        XtSetSensitive(zoomPortShowBeamLimitOnButton, False);
        showBeamCorrectionLimits(drawarea, ZOOMPORTBEAMLIMIT);
        }


    if(w == portShowBeamLimitOffButton)
        {
        XtSetSensitive(portShowBeamLimitOffButton, False);
        XtSetSensitive(portShowBeamLimitOnButton, True);
        }

    if(w == zoomPortShowBeamLimitOffButton)
        {
        XtSetSensitive(zoomPortShowBeamLimitOffButton, False);
        XtSetSensitive(zoomPortShowBeamLimitOnButton, True);
        }


    if(w == stbdShowBeamLimitOnButton)
        {
        XtSetSensitive(stbdShowBeamLimitOffButton, True);
        XtSetSensitive(stbdShowBeamLimitOnButton, False);
        showBeamCorrectionLimits(drawarea, STBDBEAMLIMIT);
        }

    if(w == zoomStbdShowBeamLimitOnButton)
        {
        XtSetSensitive(zoomStbdShowBeamLimitOffButton, True);
        XtSetSensitive(zoomStbdShowBeamLimitOnButton, False);
        showBeamCorrectionLimits(drawarea, ZOOMSTBDBEAMLIMIT);
        }

    if(w == stbdShowBeamLimitOffButton)
        {
        XtSetSensitive(stbdShowBeamLimitOffButton, False);
        XtSetSensitive(stbdShowBeamLimitOnButton, True);
        }

    if(w == zoomStbdShowBeamLimitOffButton)
        {
        XtSetSensitive(zoomStbdShowBeamLimitOffButton, False);
        XtSetSensitive(zoomStbdShowBeamLimitOnButton, True);
        }

    return;

}


unsigned char c_median (v, n)
unsigned char *v;
int n;
{
    int gap, i, j;
    unsigned char temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j=i-gap; j>=0 && v[j]>v[j+gap]; j-=gap)
                {
                temp = v[j];
                v[j] = v[j+gap];
                v[j+gap] = temp;
                }

    return (v[n/2]);
}

/*
 *    Return the size of the file indicated by descriptor fileID.
 */

int getFileSize(fileID)
int fileID;
{

    struct stat fileParams;

    if(fstat(fileID, &fileParams) == -1)
        return(0);

    return(fileParams.st_size);

}

void checkForNans(index)
int index;
{

    double testValue;

    testValue = (double)scans[index]->alt;
    if(isnan(testValue))
        scans[index]->alt = 0;

    testValue = (double)scans[index]->utm_azi;
    if(isnan(testValue))
        scans[index]->utm_azi = 0;

    return;

}

/*   get_short()
 *
 *   Subroutine to enable Intel based processors to
 *   read short integer numbers (2 bytes)
 *   in files transferred from a mc680xx machine.
 *
 */

unsigned short get_short(inbuf, location)
unsigned char *inbuf;
int location;
{
     int i;
     unsigned char *ptr1, *ptr3;
     unsigned short value;
     unsigned short *ptr2;

     ptr1 = inbuf;
     ptr3 = (unsigned char *) calloc (1, sizeof (short));

     if(INTEL)
          {
          ptr1 += location;
          for(i = 0; i < 2; i++)
               *ptr3++ = *ptr1++;
          }

     if(MOTOROLA)
          {
          ptr1 += (location + 1);
          for(i = 0; i < 2; i++)
               *ptr3++ = *ptr1--;
          }

     ptr3 -= 2;

     ptr2 = (unsigned short *) ptr3;

     value = *ptr2;

     free(ptr3);

     return(value);
}


/*    get_float()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE single precision
 *    floating point numbers (4 bytes) in files
 *    transferred from a mc680xx machine.
 */

float get_float(inbuf, location)
unsigned char *inbuf;
int location;
{
    int i;
    unsigned char *ptr1, *ptr3;
    float *ptr2;
    float value;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (float));

    if(MOTOROLA)
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 4;

    ptr2 = (float *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

