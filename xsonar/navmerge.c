/*
     navmerge.c

        merges a sonar data file with the corresponding navigation.
        The data file must have been already compacted (e.g. by demux).
        The navigation file should contain lats and longs every 1 minute.

        Libraries needed:     
          lmap          for geoutm() and spline()

        Alberto Malinverno 

     September 30 1987 

     Ported to Motif 1991:

     William W. Danforth, U.S. Geological Survey
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Xm/DialogS.h>

#include <map.h>
#include "xsonar.h"
#include "sonar_struct.h"

int getnav(MainSonar *sonar, FILE *navfp, double *djday, double *lon, double *lat, double *east, double *north);
int checkCentLon (MainSonar *sonar, char *text);
/*int getScanSize(MainSonar *, int *, int *);*/

/*float getcentlon();*/

void computm(double *lat, double *lon, double *easting, double *northing, double cent_lon, int n, MainSonar *sonar);
void getutmazi(double *east, double *north, double *utmazi, int n);
void smoothazi(double *azi, int n);
void close_navmerge(MainSonar *sonar, Widget w, int error);
/*void messages();*/

void close_checkLon_display(Widget w, XtPointer client_data, XmAnyCallbackStruct *call_data);


#define MAX_NNODES          5000          /* max number of ascii navigation points */
#define MAX_NSPL          45000     /* max number of interpolations */

void mergenav(UNUSED Widget w,
	      XtPointer client_data,
	      UNUSED XtPointer call_data)
{

    MainSonar *sonar = (MainSonar *) client_data;
    Widget navDialog;
    Widget statusWindow;
    Widget statusMessage;
    Widget separator1;
    Widget displayPings;

    Arg args[10];
    Cardinal n;

    /*unsigned char *outdata, *indata;*/
    char pingmessage[100];
    char statusmessage[100];
    char warningmessage[100];
    /*char outputfile[100];*/    /* output file name */


    struct azi {
             float azi_temp;
             } temp;           /* temp space for reading in fish azimuths */

    int inbytes;

    struct stat file_params;

    FILE *navfp;
    /*char *datafile;*/          /* navigation and data files */
    char navfile[100];
    char *string_ptr;
    char *buffer;

    int buffer_len;
    int imagesize;
    int scansize;
    int headsize;
    int outbytes;
    int check_file_status;
    int status;
    int updateScanSize;

    int n_nodes;          /* number of navigation points in navfile */
    int n_spl;          /* number of points interpolated by spline */
    int n_inter;          /* number of interpolations in cubic spline */
    /*int input_inter;*/
    int i; /*, j, c, byte_off; */
    /*int spline(double *, double *, int, double *, double *, int);*/
    int recnum;
    int infd;
    /*int merged;*/

    long azi_offset;
    long file_size;
    long position;

    /*float ping_int = 0.13333333;*/
    double n_min;          /* number of minutes in navigation file  */
    double lat[MAX_NNODES], lon[MAX_NNODES];
    double djday[MAX_NNODES];
    double north[MAX_NNODES], east[MAX_NNODES];
    double s_north[MAX_NSPL], s_east[MAX_NSPL];
    double s_djday[MAX_NSPL];
    double s_azi[MAX_NSPL];
    /*double outbuf[4];*/
    double cent_lon;

     /*
      *   If navmerge flag was not set in setup, return.
      */

     if(!sonar->navmerge_flag)
          return;

     updateScanSize = 0;


     /*
      *   Start creating widgets for this app.
      */

     n = 0;
     XtSetArg(args[n], XmNallowShellResize, False); n++;
     XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     XtSetArg(args[n], XmNx, 400); n++;
     XtSetArg(args[n], XmNy, 400); n++;
     navDialog = XtCreatePopupShell("NavDialog",
                   transientShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNallowResize, True); n++;
     statusWindow = XtCreateWidget("StatusWindow", xmFormWidgetClass,
                      navDialog, args, n);
     XtManageChild(statusWindow);

     n = 0;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Merging Navigation",
          XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNheight, 20); n++;
     XtSetArg(args[n], XmNwidth, 500); n++;
     statusMessage = XtCreateWidget("StatusMessage",
                         xmLabelWidgetClass, statusWindow, args, n);
     XtManageChild(statusMessage);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, statusMessage); n++;
     separator1 = XtCreateWidget("Separator1", xmSeparatorWidgetClass,
                                   statusWindow, args, n);
     XtManageChild(separator1);


     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, separator1); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNheight, 20); n++;
     XtSetArg(args[n], XmNwidth, 500); n++;
     XtSetArg(args[n], XmNlabelString,
          XmStringCreateLtoR("Processing Ping # 0",
          XmSTRING_DEFAULT_CHARSET)); n++;
     displayPings = XtCreateWidget("DisplayPings",
                    xmLabelWidgetClass, statusWindow, args, n);
     XtManageChild(displayPings);

     XtRealizeWidget(navDialog);
     XtPopup(navDialog, XtGrabNone);
     /*
      *   Open input and output files.
      */

     if((infd = open(sonar->infile, O_RDWR)) == -1)
          {
          sprintf(warningmessage, 
                "Error opening input file\nPlease select a data file in Setup");
          messages(sonar, warningmessage);
          close_navmerge(sonar, navDialog, True);
          return;
          }

     buffer = (char *) XtCalloc(100, sizeof(char));

     /*
      *    Added by Steve Dzurenko (URI Ocean Mapping Group) April 1995.
      *    Searches the file name buffer backwards looking for the first
      *    "." just in case some component of the filename directory
      *    hierarchy has a period in it.
      */

     string_ptr = sonar->infile;
     strcpy(buffer, string_ptr);
     buffer_len = (int) strlen(buffer);
     while(*(buffer + --buffer_len) != '.')
          strncpy((buffer + buffer_len),"\0",1);

     strncat(buffer, "nav", 3);
     sprintf(navfile, "%s", buffer);

     if((navfp = fopen(navfile, "r")) == NULL)
          {
          sprintf(warningmessage, "Error opening navigation file");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }


     /*     Check to make sure a navigation format has been chosen.
      */
     if(!sonar->latlon_flag || sonar->eastnorth_flag)
          {
          sprintf(warningmessage, "Chose setup and select input nav format");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }

     /*
      *   Make sure the data file can be read.
      */

     if (read (infd, &sonar->ping_header, 256) != 256)
          {
          sprintf(warningmessage, "Cannot read first header");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }

    if(sonar->ping_header.fileType != XSONAR)
        {
        sprintf(warningmessage, "Input file is not of type XSONAR\n");
        strcat(warningmessage, "or you need to select the XSONAR file type\n");
        strcat(warningmessage, "in the SETUP window under FILE");
        messages(sonar, warningmessage);
        close(infd);
        close_navmerge(sonar, navDialog, True);
        return;
        }


     position = lseek(infd, 0L, SEEK_SET);
     
     scansize = sonar->ping_header.sdatasize;
     if(scansize == (int) NULL)
/*
 || (scansize - 256) % 512)
*/
         {
         status = getScanSize(sonar, &scansize, &headsize);
         if(status == 2)
             {
             close_navmerge(sonar, navDialog, True);
             return;
             }
         
         updateScanSize++;
         }

     imagesize = scansize - HEADSIZE;

     if(updateScanSize)
         {
         sprintf(statusmessage, "Updating scan size in header");
         n = 0;
         XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                         XmSTRING_DEFAULT_CHARSET)); n++;
         XtSetValues(statusMessage, args, n);
         XmUpdateDisplay(navDialog);

         i = 0;

         while(read (infd, &sonar->ping_header, 256) == 256)
             {
             i++;
             sonar->ping_header.sdatasize = scansize;
             if(!sonar->ping_header.swath_width)
                 sonar->ping_header.swath_width = sonar->swath;
             position = lseek(infd, -256L, SEEK_CUR);

             write(infd, &sonar->ping_header, 256);
             position = lseek(infd, (long) imagesize, SEEK_CUR);

             if(!(i % 50))
                 {
                 sprintf(pingmessage, "Processing Ping # %d", i);
                 n = 0;
                 XtSetArg(args[n], XmNlabelString,
                     XmStringCreateLtoR(pingmessage,
                     XmSTRING_DEFAULT_CHARSET)); n++;
                 XtSetValues(displayPings, args, n);
                 XmUpdateDisplay(navDialog);
                 }

             }

        }

    position = lseek(infd, 0L, SEEK_SET);

     /*
      *     Read navigation file and compute UTM coordinates
      *     of navigation data (spline nodes)
      */

     n_nodes = getnav (sonar, navfp, djday, lon, lat, east, north);

     if(sonar->latlon_flag)
          {
          if(!sonar->c_lon)
               {
               sonar->ping_header.clon = getcentlon (lon[2]);
               sonar->c_lon = (double) sonar->ping_header.clon;

               sprintf(warningmessage, "Using central lon from file: %f",
                                   getcentlon (lon[2]));
               status = checkCentLon(sonar, warningmessage);
               n = 0;

               if(XtNameToWidget(sonar->toplevel, "*SetupDialog") != NULL)
                   {
                   XtSetArg(args[n], XmNvalue, sonar->c_lon); n++;
                   XtSetValues(XtNameToWidget(sonar->toplevel, 
                                               "*CentralLonScale"), args, n);
                   }

               }
          else
               {
               sonar->ping_header.clon = sonar->c_lon;
               sprintf(warningmessage, "using central lon from setup: %f",
                                                  sonar->c_lon);
               status = checkCentLon(sonar, warningmessage);
               }


        switch(status)
            {
            case 1:      /* everthing is ok, continue */
                break;
            case 2:      /* user wants to reset the central longitude */
                close(infd);
                close_navmerge(sonar, navDialog, True);
                return;
            default:
                break;
            }
                

          cent_lon = sonar->c_lon;
          computm (lat, lon, east, north, cent_lon, n_nodes, sonar);
          }

     /*
      *     Find file size, and compute number of pings.
      *     Then compute total number of minutes (time) for file.
      */


     if((check_file_status = stat(sonar->infile, &file_params)) == -1)
          {
          sprintf(warningmessage, "Cannot read data file size");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }

     file_size = file_params.st_size;


     n_min = djday [n_nodes-1] * 1440.0 - djday [0] * 1440.0;
     
     n_inter = file_size / scansize;


     /*      call spline with input of north[], djday[] 
      *      and then with input of east[], djday[] 
      */
     

     n_spl = spline(djday, north, n_nodes, s_djday, s_north, n_inter);
     if (n_spl != (n_inter + 1))
          {
          sprintf(warningmessage, "Error in the spline routine for north");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }


     n_spl = spline(djday, east, n_nodes, s_djday, s_east, n_inter);
     if (n_spl != (n_inter + 1))
          {
          sprintf(warningmessage, "Error in the spline routine for east");
          messages(sonar, warningmessage);
          close(infd);
          close_navmerge(sonar, navDialog, True);
          return;
          }


     if(sonar->fish_azi_flag) /*  Fish sent compass telemetry -- use it */
          {
          sprintf(statusmessage, "Using Fish telemetry for azimuth data");
          n = 0;
          XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetValues(statusMessage, args, n);
          XmUpdateDisplay(navDialog);

          for(i = 0; i < n_inter; i++)
               {
               if(!(i % 200))
                   {
                   sprintf(pingmessage, "Processing Ping # %d", i);
                   n = 0;
                   XtSetArg(args[n], XmNlabelString,
                              XmStringCreateLtoR(pingmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
                   XtSetValues(displayPings, args, n);
                   XmUpdateDisplay(navDialog);
                   }

               azi_offset = (long)(i * scansize + 32);
               position = lseek(infd, azi_offset, SEEK_SET);
               inbytes = read(infd, &temp, 4);
               s_azi[i] = (double) temp.azi_temp + sonar->magnetic_declination / RAD_TO_DEG;
               if(s_azi[i] > M_PI * 2.0)
                   s_azi[i] -= M_PI * 2.0;
               }
          }
     else     /*     No telemetry -- compute utm azimuth for each
           *     spline point and smooth it
           */
          {
          sprintf(statusmessage, "Getting UTM azimuths for datafile");
          n = 0;
          XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                              XmSTRING_DEFAULT_CHARSET)); n++;
          XtSetValues(statusMessage, args, n);
          XmUpdateDisplay(navDialog);

            getutmazi (s_north, s_east, s_azi, n_spl);

          }

     sprintf(statusmessage, "Smoothing azimuth points, 1st pass ...");
     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                    XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetValues(statusMessage, args, n);
     XmUpdateDisplay(navDialog);

     smoothazi (s_azi, n_inter);

     sprintf(statusmessage, "Smoothing azimuth points, 2nd pass ...");
     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                    XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetValues(statusMessage, args, n);
     XmUpdateDisplay(navDialog);

     smoothazi (s_azi, n_inter);

     /*     move to the appropriate location in the data file
      *     and write the time, north, east, utm azimuth of the sweep
      */

     sprintf(statusmessage, "Writing nav to file %s", sonar->infile);
     n = 0;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(statusmessage,
                         XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetValues(statusMessage, args, n);
     XmUpdateDisplay(navDialog);

     /*
      *   Set the file pointer back to the beginning just in case the
      *   fish azimuth was used for heading: see the routine above
      *   which reads the fish telem from the input file.
      */

     position = lseek(infd, 0L, SEEK_SET);

     recnum = 0;
     for (i = 0; i < n_inter; i++)
         {
         recnum++;
         if(!(recnum % 100))
             {
             sprintf(pingmessage, "Processing Ping # %d", recnum);

             n = 0;
             XtSetArg(args[n], XmNlabelString,
                    XmStringCreateLtoR(pingmessage,
                    XmSTRING_DEFAULT_CHARSET)); n++;
             XtSetValues(displayPings, args, n);
             XmUpdateDisplay(navDialog);
             }

         if((inbytes = read (infd, &sonar->ping_header, 256)) != 256)
             {
             sprintf(warningmessage, 
                          "Error reading ping header at ping %d", recnum);
             messages(sonar, warningmessage);
             close(infd);
             close_navmerge(sonar, navDialog, True);
             return;
             }
         else
             {
             sonar->ping_header.djday = s_djday[i];
             sonar->ping_header.utm_n = s_north[i];
             sonar->ping_header.utm_e = s_east[i];
             sonar->ping_header.utm_azi = s_azi[i];
             sonar->ping_header.merged = True;
             sonar->ping_header.clon = (float) sonar->c_lon;

             position = lseek(infd, -256L, SEEK_CUR);
             }

         if((outbytes = write(infd, &sonar->ping_header, 256)) != 256)
             {
             sprintf(warningmessage, "Error writing nav buffer, bytes = %d",
                                                       outbytes);
             messages(sonar, warningmessage);
             close(infd);
             close_navmerge(sonar, navDialog, True);
             return;
             }
         else
             position = lseek(infd, (long) imagesize, SEEK_CUR);

          }

     close(infd);
     close_navmerge(sonar, navDialog, False);
}

/*     getnav()
 *
 *     reads the navigation data in the navigation file into the
 *     arrays djday[], lat[], lon[]; returns the total number of 
 *     navigation nodes
 */
int getnav (MainSonar *sonar, FILE *navfp, double *djday, double *lon, double *lat, double *east, double *north)
{
     int jday, hour, min, sec;
     float fsec, sectenths;
     int i = 0;
     int latdeg, londeg;
     float latmin, lonmin;
     float tempeast, tempnorth;
     /*double tempday;*/
     /*double hmtodjd(int, int, int, double);*/
     
     if(sonar->eastnorth_flag)
          {
          while (fscanf (navfp,"%d %d %d %d %f %f %f", 
          &jday, &hour, &min, &sec, &sectenths, &tempeast, &tempnorth) != EOF)
               {
               *east++ = (double)tempeast;
               *north++ = (double)tempnorth;
               fsec = (float) sec + sectenths;
                         *djday++ = hmtodjd (jday, hour, min, fsec);

               i++;
               }
          }

     if(sonar->latlon_flag)
          {
          while (fscanf (navfp,"%d %d %d %d %d %f %d %f", &jday, &hour, &min,
                    &sec, &latdeg, &latmin, &londeg, &lonmin) != EOF)     
               {
               fsec = (float) sec;
                  *djday++ = hmtodjd (jday, hour, min, fsec);
               degmintodd (lat++, latdeg, (double)latmin);
               degmintodd (lon++, londeg, (double)lonmin);
               i++;
               }
          }

     fclose(navfp);
     return (i);
}

/*     computm()
 *
 *     computes UTM coordinates for each pair of lat and lon 
 */

void computm (double *lat, double *lon, double *easting, double *northing, double cent_lon, int n, MainSonar *sonar)
{
     int i;
     double north, east;

     for (i = 0; i < n; i++)
          {
          geoutm (lat[i], lon[i], cent_lon, &east, &north, sonar->ellipsoid);
          northing[i] = north;
          easting[i] = east;
          }

     return;
}

/*     getutmazi ()
 *
 *     computes the grid azimuth between successive points in the arrays 
 *     of easting and northing. 
 */
void getutmazi (double *east, double *north, double *utmazi, int n)
{
     int i;
     int numazi;
     double deltaeast, deltanorth, arctan;

     numazi= n - 1;
     for (i = 1; i < numazi; i++)
          {
          deltaeast = (east[i+1] - east[i-1]) / 2.0;     
          deltanorth = (north[i+1] - north[i-1]) / 2.0;
          arctan = atan2 (deltanorth, deltaeast);
          if (arctan < 0.0) arctan += (2.0 * M_PI);
          utmazi[i] = arctan;
          }
     utmazi[0] = utmazi[1];
     utmazi[numazi] = utmazi[numazi - 1];

     return;
}

/*     smoothazi()
 *
 *     smooths the azimuth array by a moving average. Checks if there is a
 *     0-359 jump in the 5-point smoothing window; if so, does not smooth.
 */

void smoothazi (double *azi, int n)
{
     int i, j, jump;
     double deltazi;
     double oldazi[MAX_NSPL];

     for (i = 0; i < n; i++)
          oldazi[i] = azi[i];
/*
          if(i)
              {
              if(fabs(oldazi[i] - azi[i - 1]) > 0.1745)
                 {
                 oldazi[i] = azi[i - 1];
                 fprintf(stdout, "Bad azi = %f, ", azi[i]);
                 azi[i] = azi[i - 1];
                 fprintf(stdout, "corrected azi = %f\n", azi[i]);
                 }
              }

*/


     for (i = 2; i < n - 2; i++)
          {
          jump = 0;
          for (j = i-2; j < i+2; j++)
               {
               deltazi = fabs (oldazi[j] - oldazi[j+1]);
               if (deltazi > M_PI) 
                    jump++;
               }
          if (!jump)     /* avoids averaging over the 0-359 jump */
               {
               azi[i] = (oldazi[i+2] + oldazi[i+1] + 
               oldazi[i] + oldazi[i-1] + oldazi[i-2]) / 5.0;
               }
          }

     return;
}

void close_navmerge(MainSonar *sonar, Widget w, int error)
{


     Widget parent_widget;

     parent_widget = w;

     if(error)
         {
         sonar->navmerge_flag = 0;
         setToggle("*MergeNavToggle", sonar->toplevel, False);

         sonar->slant_flag = 0;
         setToggle("*SlantRangeToggle", sonar->toplevel, False);
 
         sonar->destripe_flag = 0;
         setToggle("*DestripeToggle", sonar->toplevel, False);
 
         sonar->beam_flag = 0;
         setToggle("*BeamPatternToggle", sonar->toplevel, False);
 
         sonar->linear_flag = 0;
         setToggle("*LinearStretchToggle", sonar->toplevel, False);
 
         sonar->equalize_flag = 0;
         setToggle("*EqualizationToggle", sonar->toplevel, False);
 
         }

/*
     while(!XtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);
*/

     XtPopdown(parent_widget);
     XtDestroyWidget(parent_widget);

     return;

}

/*
 *     Check the central longitude. If not what is desired, have navmerge
 *  return and let user select a new value from setup.
 */


int checkCentLon(MainSonar *sonar, char *text)
{
     Arg args[2];

     int n;
     int answer;

     Widget checkLonDialog;
     Widget checkLonBox;
     Widget checkLonButton;


     answer = 0;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
     checkLonDialog = XtCreatePopupShell("CheckLonDialog",
               xmDialogShellWidgetClass, sonar->toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XmNmessageString,
          XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING); n++;
     checkLonBox = XtCreateWidget("CheckLonBox",
          xmMessageBoxWidgetClass, checkLonDialog,
          args, n);

     checkLonButton = XmMessageBoxGetChild(checkLonBox,
                    XmDIALOG_HELP_BUTTON);
     XtUnmanageChild(checkLonButton);

     /* FIX: arg 3 close_checkLon_display is of wrong type!?! */
     XtAddCallback(checkLonBox, XmNokCallback,
               (XtCallbackProc)close_checkLon_display, &answer);
     XtAddCallback(checkLonBox, XmNcancelCallback,
               (XtCallbackProc)close_checkLon_display, &answer);

     XtManageChild(checkLonBox);

     while(answer == 0)
          XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

     return(answer);

}

void close_checkLon_display(UNUSED Widget w,
			    XtPointer client_data, XmAnyCallbackStruct *call_data)
{

     int *answer = (int *) client_data;;

     switch(call_data->reason)
        {
        case XmCR_OK:
            *answer = 1;
            break;
        case XmCR_CANCEL: 
            *answer = 2;
            break;
          }

     return;
}
