/* 
   fishnav.c

   program to compute a first estimate of the fish position from the ship
   navigation and the horizontal ranges. Takes input from stdin and sends
   output to stdout.
   Uses the technique of the midpoint between fish and ship forcing the
   line that connects fish and ship at time 1 to pass through the midpoint
   between fish and ship at time 0.
   The input navigation should be given every 15 minutes. The input should
   contain slant ranges in milliseconds except for the first few points.
   The format of the input navigation should be as follows (all int except
   for min_phi and min_lam, which are float):
    jday hour min deg_phi min_phi(f) deg_lam min_lam(f) alt depth sr
   alt(itude) and depth are in meters; s(lant)r(ange) in milliseconds.
   Needs the library lmap for the following routines:
    geodis(), degmintodd(), ddtodegmin().
   A. Malinverno
   Oct 30 1986 
*/

#include <unistd.h>

#include <map.h>

#include "xsonar.h"

#include "sonar_struct.h"

#define MAXHISTORY 12

double h_range(double s_range, double depth, MainSonar *sonar);
void fish_output(FILE *fishfp, int jday, int hour, int min, int sec, double fish_phi, double fish_lam);
int fish_position(double ship_phi, double ship_lam, double range, double mid_phi, double mid_lam, double mid_range, double *fish_phi, double *fish_lam, MainSonar *sonar);

int fishNav(Widget callingWidget, MainSonar *sonar, double *depth, double *sr, int readHeader)
{

    FILE *navfp;
    FILE *origfp;

    char *string_ptr, buffer[255], origBuffer[255];
    char warningmessage[100];
    char tempBuffer[50];

    char *navString[500];
    char *navStringPtr;

    double mid_phi, mid_lam;
    double range, /*past_range,*/ mid_range;

    double slantRange[1000];
    double fishDepth[1000];

    double ship_phi, ship_lam;
    double fish_phi, fish_lam;
    double past_phi[MAXHISTORY], past_lam[MAXHISTORY];
    double min_phi, min_lam;

    double shipEast, shipNorth;
    double de, dn;

    float azimuth;

    int pingHour, pingMin;
    double pingSec;

    double slope1, slope2;
    double rangeIncrement1, rangeIncrement2;
    double intercept1, intercept2;
    int halfNodes;

    long position;

    int jday, hour, min, sec, deg_phi, deg_lam/*, alt*/;
    int fileSize;
    int i, j, k, n;
    int navStringPtrPosition;
    int buffer_len;
    int number_of_nodes;
    int answer;
    int inbytes;
    int infd;
    int scansize, imagesize;

    char *tempString;

     /*
      *    Added by Steve Dzurenko (URI Ocean Mapping Group) April 1995.
      *    Searches the file name buffer backwards looking for the first
      *    "." just in case some component of the filename directory
      *    hierarchy has a period in it.
      */

    if(strlen(sonar->infile) < 1)
        {
        sprintf(warningmessage, "Please select a file in Setup");
        messages(sonar, warningmessage);
        return(0);
        }

    string_ptr = sonar->infile;
    strcpy(buffer, string_ptr);
    buffer_len = (int) strlen(buffer);
    while(*(buffer + --buffer_len) != '.')
        strncpy((buffer + buffer_len),"\0",1);

    strcpy(origBuffer, buffer);

    strncat(buffer, "nav", 3);
    strcat(origBuffer, "nav.orig");

    if((navfp = fopen(buffer, "r")) == NULL)
        {
        sprintf(warningmessage, "Error opening navigation file");
        messages(sonar, warningmessage);
        return(0);
        }

    if((origfp = fopen(origBuffer, "w")) == NULL)
        {
        sprintf(warningmessage, "Error opening fish navigation file");
        messages(sonar, warningmessage);
        fclose(navfp);
        return(0);
        }

    infd = open (sonar->infile, O_RDONLY);  /* read only */
    if (infd == -1)
        {
        sprintf(warningmessage,
             "Cannot open sonar file\nPlease select a data file in Setup");
        messages(sonar, warningmessage);
        fclose(navfp);
        fclose(origfp);
        return(0);
        }

    /*
     *   Make sure the data file can be read.
     */

    if ((inbytes = read (infd, &sonar->ping_header, 256)) != 256)
        {
        sprintf(warningmessage, "Cannot read first header");
        messages(sonar, warningmessage);
        fclose(navfp);
        fclose(origfp);
        close(infd);
        return(0);
        }

    scansize = sonar->ping_header.sdatasize;
    imagesize = scansize - 256;


    position = lseek(infd, 0L, SEEK_SET);

    /*  
     *    read the header to get the seed value for slant range  and
     *    depth.  Need this for a first estimate of mid point between fish
     *    and ship.
     *
     *    meters = (float)sr * 1.5;  (if slant range is in milliseconds)
     */

    i = 0;

    position = lseek(infd, 0L, SEEK_SET);
    inbytes = 0;

    pingHour = pingMin = 0;
    pingSec = 0.0;

    (void)fseek(navfp, 0L, SEEK_SET);


/*
fprintf(stdout, "jday = %d, hour = %d, min = %d, sec = %d\n", jday, hour, min, sec);
fprintf(stdout, "djday = %d, pinghour = %d, pingmin = %d, pingsec = %f\n", (int)sonar->ping_header.djday, pingHour, pingMin, pingSec);
fprintf(stdout, "slant range = %lf\tdepth = %lf\n", slantRange[number_of_nodes], fishDepth[number_of_nodes]);
fprintf(stdout, "read header, i is %d\n", i);
*/


    if(readHeader)   
        {

        while((fscanf (navfp,"%d %d %d %d %d %lf %d %lf", &jday, &hour, &min,
            &sec, &deg_phi, &min_phi, &deg_lam, &min_lam)) != EOF)
            {


            while(pingHour != hour || pingMin != min)
                {
                if((inbytes = read (infd, &sonar->ping_header, 256)) != 256)
                    break;
                position = lseek(infd, (long) imagesize, SEEK_CUR);
                djdtohm(sonar->ping_header.djday, &pingHour, &pingMin,&pingSec);
                slantRange[i] = sonar->ping_header.range;
                if(sonar->ping_header.depth == 0)
                    fishDepth[i] = sonar->ping_header.total_depth 
                                                   - sonar->ping_header.alt;
                else
                    fishDepth[i] = sonar->ping_header.depth;

                if(i == 0)  /*  always get the very first slant range */
                    break;
                }
            i++;
            }

        }
    else
        {
        while((fscanf (navfp,"%d %d %d %d %d %lf %d %lf", &jday, &hour, &min,
            &sec, &deg_phi, &min_phi, &deg_lam, &min_lam)) != EOF)
            {
            /*  Count the number of nodes in the file  */
            /*  Could do this with a system call to wc.....*/
            i++;
            }

        /*
         *    Now linearly interpolate between start, mid and end 
         *    layback and depths.
         */

        halfNodes = i / 2;
for(n = 0; n < 3; n++)
printf("sr[%d] = %f\n", n, sr[n]);
for(n = 0; n < 3; n++)
printf("depth[%d] = %f\n", n, depth[n]);
printf("i = %d, halfNodes = %d\n", i, halfNodes);

        rangeIncrement1 = (sr[1] - sr[0]) / halfNodes;
        rangeIncrement2 = (sr[2] - sr[1]) / halfNodes;

printf("rangeIncrement1 = %f, rangeIncrement2 = %f\n", rangeIncrement1, rangeIncrement2);

        slope1 = (depth[1] - depth[0]) / (sr[1] - sr[0]);
        slope2 = (depth[2] - depth[1]) / (sr[2] - sr[1]);

        intercept1 = depth[0] - (slope1 * sr[0]);
        intercept2 = depth[1] - (slope2 * sr[1]);

        for(j = 0, k = 0; j < halfNodes; j++, k++)
            {
            slantRange[j] = sr[0] + rangeIncrement1 * k;;
            fishDepth[j] = slope1 * slantRange[j] + intercept1;
            }

        for(j = halfNodes, k = 0; j < i; j++, k++)
            {
            slantRange[j] = sr[1] + rangeIncrement2 * k;
            fishDepth[j] = slope2 * slantRange[j] + intercept2;
            }
        }

    /*
     *    make sure to always get the last slant range...
     */

    fileSize = getFileSize(sonar->infile);

    position = (long) (fileSize - scansize);
    position = lseek(infd, position, SEEK_SET);


    if((inbytes = read (infd, &sonar->ping_header, 256)) != 256)
        fprintf(stderr, "problem here ....\n");

    if(readHeader)
        {
        slantRange[i - 1] = sonar->ping_header.range;

        if(sonar->ping_header.depth == 0)
            fishDepth[i - 1] = sonar->ping_header.total_depth - 
                                                        sonar->ping_header.alt;
        else
            fishDepth[i - 1] = sonar->ping_header.depth;
        }

    (void)fseek(navfp, 0L, SEEK_SET);

    number_of_nodes = 0;

    while((fscanf (navfp,"%d %d %d %d %d %lf %d %lf", &jday, &hour, &min,
        &sec, &deg_phi, &min_phi, &deg_lam, &min_lam)) != EOF)
        {

        fprintf(origfp, "%03d %02d %02d %02d %02d %8.5f %4d %8.5f\n",
                     jday, hour, min, sec, deg_phi, min_phi, deg_lam, min_lam);

        navString[number_of_nodes] = (char *) XtCalloc(80, sizeof(char));

        sprintf(navString[number_of_nodes],
                  "%03d %02d %02d %02d %d %.5f %d %.5f",
                  jday, hour, min, sec, deg_phi, min_phi, deg_lam, min_lam);

        /*sprintf(tempBuffer, "  %.3lf  %.3lf\0",  FIX: really need the \0???*/
        sprintf(tempBuffer, "  %.3lf  %.3lf", 
                     slantRange[number_of_nodes], fishDepth[number_of_nodes]);
        strncat(navString[number_of_nodes], tempBuffer, strlen(tempBuffer));

        number_of_nodes++;
        }

    fclose(origfp);
    fclose(navfp);


    navStringPtr = (char *) XtCalloc(80 * number_of_nodes, sizeof(char));

    for(i = 0; i < number_of_nodes; i++)
        {
        strncat(navStringPtr, navString[i], 79);
        strcat(navStringPtr, "\n");
        }


    answer = createNavText(callingWidget, navStringPtr, sonar, number_of_nodes);


    if(answer == 2)
        {

        for(i = 0; i < number_of_nodes; i++)
            XtFree(navString[i]);

        XtFree(navStringPtr);
        return(0);
        }

    navStringPtrPosition = 0;


    for(i = 0; i < number_of_nodes; i++)
        {
        tempString = (char *) XtCalloc(80, sizeof(char));
        while(strrchr(tempString, '\n') == NULL)
            {
            strncat(tempString, navStringPtr, 1);
            navStringPtr++;
            navStringPtrPosition++;
            }
        strncpy(navString[i], tempString, strlen(tempString));
        XtFree(tempString);
        }

    navStringPtr -= navStringPtrPosition;

    i = 0;

    while(i < 2)
        {
        sscanf (navString[i], "%d %d %d %d %d %lf %d %lf %lf %lf", 
                &jday, &hour, &min, &sec, &deg_phi, &min_phi, &deg_lam, 
                &min_lam, &slantRange[i], &fishDepth[i]);

        degmintodd (&past_phi[i], deg_phi, min_phi);
        degmintodd (&past_lam[i], deg_lam, min_lam);
/*
fprintf(stdout, "slant range = %lf\tdepth = %lf\n", slantRange[i], fishDepth[i]);
fprintf(stdout, "input lat = %f\tinput lon = %f\n", past_phi[i], past_lam[i]);
fprintf(stdout, "lat = %d %f\tlon = %d %f\n", deg_phi, min_phi, deg_lam, min_lam);
*/
        i++;

        }


    range = h_range (slantRange[0], fishDepth[0], sonar);

    if(range == 0)
        {
        for(i = 0; i < number_of_nodes; i++)
            XtFree(navString[i]);
        XtFree(navStringPtr);
        return(0);
        }

    if(!sonar->c_lon)
        {
        sprintf(warningmessage, "Central longitude is 0");
        messages(sonar, warningmessage);
        for(i = 0; i < number_of_nodes; i++)
            XtFree(navString[i]);
        XtFree(navStringPtr);
        return(0);
        }

    /* first estimate of fish position */

    azimuth = geoazi(past_phi[1], past_lam[1], past_phi[0], past_lam[0]);
    azimuth /= RAD_TO_DEG;

    de = sin(azimuth) * range;
    dn = cos(azimuth) * range;

    geoutm(past_phi[0], past_lam[0], sonar->c_lon,  
                                &shipEast, &shipNorth, sonar->ellipsoid);

    shipNorth += dn;
    shipEast += de;

    utmgeo(&fish_phi, &fish_lam, sonar->c_lon, shipEast, 
                                         shipNorth, 'n', sonar->ellipsoid);

    sscanf (navString[0], "%d %d %d %d %d %lf %d %lf %lf %lf", 
                &jday, &hour, &min, &sec, &deg_phi, &min_phi, &deg_lam, 
                &min_lam, &slantRange[i], &fishDepth[i]);

    if((navfp = fopen(buffer, "w")) == NULL)
        {
        sprintf(warningmessage, "Error re-opening navigation file for writing");
        messages(sonar, warningmessage);
        for(i = 0; i < number_of_nodes; i++)
            XtFree(navString[i]);
        XtFree(navStringPtr);
        return(0);
        }

    /*
     *    write out first estimate of fish position 
     */

    fish_output (navfp, jday, hour, min, sec, fish_phi, fish_lam);

    mid_phi = (past_phi[0] + fish_phi) / 2.0;
    mid_lam = (past_lam[0] + fish_lam) / 2.0;

    for(i = 1; i < number_of_nodes; i++)
        {

        sscanf (navString[i], "%d %d %d %d %d %lf %d %lf %lf %lf", 
                &jday, &hour, &min, &sec, &deg_phi, &min_phi, &deg_lam, 
                &min_lam, &slantRange[i], &fishDepth[i]);

        if (slantRange[i] == 0)
            {
            for(i = 0; i < number_of_nodes; i++)
                XtFree(navString[i]);
            XtFree(navStringPtr);
            sprintf(warningmessage, "There is a zero input slant range");
            messages(sonar, warningmessage);
            fclose(navfp);
            return(0);
            }

        degmintodd (&ship_phi, deg_phi, min_phi);
        degmintodd (&ship_lam, deg_lam, min_lam);

        range = h_range (slantRange[i], fishDepth[i], sonar);

        if(range == 0)
            {
            fclose(navfp);
            return(0);
            }

        mid_range = geodis(ship_phi, ship_lam, mid_phi, 
                                                mid_lam, sonar->ellipsoid);

	/* FIX: this call was missing sonar!!! */
        if(fish_position (ship_phi, ship_lam, range, mid_phi, 
                               mid_lam, mid_range, &fish_phi, &fish_lam, sonar))
            ;    /* do nothing, calculated fish position AOK   */
        else
            {
            fclose(navfp);
            return(0);
            }
/*
fprintf(stdout, "mid range = %f\n", mid_range);
fprintf(stdout, "shiplat = %f\tshiplon = %f\n", ship_phi, ship_lam);
fprintf(stdout, "fishlat = %f\tfishlon = %f\n", fish_phi, fish_lam);
fprintf(stdout, "\n");
fprintf(stdout, "calculated distance = %lf\n", geodis(ship_phi, ship_lam, fish_phi, fish_lam));
*/

        fish_output (navfp, jday, hour, min, sec, fish_phi, fish_lam);

        mid_phi = (ship_phi + fish_phi) / 2.0;
        mid_lam = (ship_lam + fish_lam) / 2.0;
    }


    for(i = 0; i < number_of_nodes; i++)
        XtFree(navString[i]);

    XtFree(navStringPtr);

    fclose(navfp);

    return(1);
}

/* 
 *
 * returns the fish position at a given range from the ship forcing the vector
 * fish-ship to pass through the midpoint position 
 *
 */

int fish_position (double ship_phi, double ship_lam, double range, double mid_phi, double mid_lam, double mid_range, double *fish_phi, double *fish_lam, MainSonar *sonar)
{
    double range_ratio;
    double delta_phi, delta_lam;
    char warning[100];

    if (mid_range == 0.0)
        {
        sprintf(warning, "geodis(): Mid range calculation is zero");
        messages(sonar, warning);
        return(0);
        }

    range_ratio = range / mid_range;
    delta_phi = ship_phi - mid_phi;
    delta_lam = ship_lam - mid_lam;
    *fish_phi = ship_phi - delta_phi * range_ratio;
    *fish_lam = ship_lam - delta_lam * range_ratio;

    return(1);
}

void fish_output (FILE *fishfp, int jday, int hour, int min, int sec, double fish_phi, double fish_lam)
{
    double fish_min_phi, fish_min_lam;
    int fish_deg_phi, fish_deg_lam;

    ddtodegmin (fish_phi, &fish_deg_phi, &fish_min_phi);
    ddtodegmin (fish_lam, &fish_deg_lam, &fish_min_lam);

    fprintf (fishfp, "%03d %02d %02d %02d %3d %.5f %4d %.5f\n", jday, hour, min,
                 sec, fish_deg_phi, fish_min_phi, fish_deg_lam, fish_min_lam);
    return;
}

/* returns the horizontal range from fish to ship */
double h_range (double s_range, double depth, MainSonar *sonar)
{
    double arg; 
    double h_r;
    char warning[100];

    arg = (double)(s_range*s_range - depth*depth);
    if (arg < 0)
        {
        sprintf(warning, "Sqrt of a negative number\n");
        strcat(warning, "Check that no zero slant ranges exist");
        messages(sonar, warning);
        return(0);
        } 
    h_r = sqrt(arg) + 8.53;  /*  8.53 =  distance from antenna to transducer */
    return (h_r);            /*  R/V  Karluk  */
}

double distance(double lat1, double lon1, double lat2, double lon2, MainSonar *sonar)
{

    double returnDistance;
    double utm1, utm2, utm3, utm4;
    double clon;
    double disSquared;

    clon = (double) getcentlon(lon1);

    geoutm(lat1, lon1, clon, &utm1, &utm2, sonar->ellipsoid);
    geoutm(lat2, lon2, clon, &utm3, &utm4, sonar->ellipsoid);

    disSquared = (utm3 - utm1) * (utm3 - utm1) + (utm4 - utm2) * (utm4 - utm2);
    returnDistance = sqrt(disSquared);
    return(returnDistance);

}
