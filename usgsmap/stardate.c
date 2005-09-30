#include <math.h>

#include "map.h"

#define year   ymdhms[0]
#define month  ymdhms[1]
#define day    ymdhms[2]
#define hour   ymdhms[3]
#define minute ymdhms[4]
#define second ymdhms[5]


/* Convert an shorteger array of year, month, day, hour, minute and second
   to a 'stardate' consisting of an shorteger part ( the Julian date for the
   year, month day ) and a fractional part ( the hour, minute, second are
   coverted to a fracional part of a day ). The stardate is returned as a
   double. The time is assumed to be in 24 hour format. Make sure the year
   is four digits if it's in the 20th Century.
*/

double
stardate( short *ymdhms )
#if 0
     short *ymdhms;       /* Notice you send the address of the date/time array */
#endif
{           
            
   double fd; /* fractional part of star date */
   long jd;   /* shorteger part of star date */

#if 0
   double hms2sd();
   long julian();
#endif


      jd = julian( ymdhms );  /* get whole number part of stardate for */
                              /* year, month, day */

      fd = hms2sd( ymdhms );  /* get fractional part of stardate for */
                              /* hour, minute, second */

      return( jd + fd );
}

/* Inverse of 'stardate' above. Input double precision stardate and this
   function modifies the date/time array (see #defines above) to the 
   corresponding calendar date and 24 hour day time.
*/

/* void invstardate( double sd, short *ymdhms ) */
void invstardate( double sd, short ymdhms[] )
#if 0
double sd;           /* stardate to be converted to year, month etc */
short *ymdhms;       /* integer array where computed year, month, hour, */
                   /* minute and second will be placed               */
#endif
{
      long jd;  /* shorteger part of stardate */
      double fd; /* fractional part of stardate */
      /*short cdate(), sd2hms();*/
      static double halfsec = 5.78703703703704e-6; /* stardate of 1/2 second */

      sd += halfsec; /* add in 1/2 second for round-off error */

      jd = (long)sd;   /* get the integral part of the stardate */

      fd = sd - (double)jd; /* get the fractional part of the stardate */

      cdate(jd, ymdhms);

      sd2hms(fd, ymdhms);

      return;

}

/* convert hours, minutes and seconds (all shortegers) to a fractional
   part of a day (in double precision).
*/

double hms2sd( short ymdhms[] )
#if 0
     short *ymdhms;     /* Notice you send the address of the date/time array */ 
#endif
{
   return((hour / 24.) + (minute / 1440.) + (second / 86400.));
}

/* Convert a fractional part of a day (double precision) shorto hours, minutes,
   and seconds in a 24 hour day. 
*/

short sd2hms( double fracday, short ymdhms[] )
#if 0
double fracday;    /* Fractional part of day to be conv. to hr/min/sec */
short *ymdhms;       /* Notice you pass the address of the date/time array */
#endif
{

   fracday = fracday * 24.;
   hour = fracday;

   fracday = (fracday - hour) * 60.;
   minute = fracday;

   fracday = ((fracday - minute) * 60.);
   second = fracday;

   return second;
}

/* julian - input calendar date, return julian date */

long
julian ( short ymdhms[] )
#if 0
     short *ymdhms;      /* Notice you pass the address of the date/time array */
#endif
{

    short tempyear, tempmonth;

	if (month <= 2) {
		tempmonth = month + 9;
		tempyear = year - 1;
	} else {
		tempmonth = month - 3;
                tempyear = year;
        }

	return ( (146097L * (tempyear / 100))/4 + (1461L * (tempyear % 100))/4 +
		((153 * tempmonth + 2)/5 + day) + 1721120L );
}

/* cdate - convert Julian date to calandar date */

short cdate(long jd, short ymdhms[])
#if 0
long jd;        /* Julian date to be converted to year, month, day */
short *ymdhms;    /* Notice you pass the address of the date/time array */
#endif
{
        long tempyr, tempmo, tempda;

	jd -= 1721120L;
	tempyr = (4 * jd - 1)/146097L;
	jd = 4 * jd - 1 - 146097L * tempyr;
	tempda = jd / 4;
	jd = (4 * tempda + 3) / 1461;
	tempda = (4 * tempda + 3 - 1461 * jd + 4) / 4;
	tempmo = (5 * tempda - 3) / 153;
	tempda = (5 * tempda - 3 - 153 * tempmo + 5) / 5;
	tempyr = 100 * tempyr + jd;
	if (tempmo >= 10) {
		tempmo -=9;
		tempyr++;
	} else
		tempmo += 3;

        year = tempyr;
        month = tempmo;
        day = (short)tempda;
	return day; /* FIX: is this really what was supposed to be returned? */
}
 
/* Function to convert a difference in days, hours, minutes and    */
/* seconds to the corresponding difference in stardate.  Year and  */
/* month make no sense in a time difference.                       */
/* This is similar to stardate() in that ymdhms[] array is entered */
/* and the corresponding stardate is returned.                     */

/* FIX: is the length of this array known? */

double sd_diff( short ymdhms [] )
#if 0
  short ymdhms[];  /* integer array with time difference to convert */
#endif
{
  /*double sd, hms2sd(); */
      double sd;

      sd = (double)ymdhms[2] + hms2sd(ymdhms); /* shorteger part is days */
                                               /* fraction is time of day */
      return( sd );
}

/* Function to convert a difference in stardate to the corresponding  */
/* difference in days, hours, minutes and seconds.  Year and month    */
/* make no sense in a time difference.                                */
/* This is similar to invstardate() in that a stardate is entered and */
/* the ymdhms[] array is modified with the returned values.           */

short
invsd_diff( double sd, short ymdhms[] )
#if 0
double sd;   /* stardate difference to be converted */
short ymdhms[];  /* integer array to modify with results */
#endif
{
  short r; /* return from sd2hms */
      static double halfsec = 5.78703703703704e-6; /* stardate of 1/2 second */

      sd += halfsec;  /* add in 1/2 second for round-off error */
      ymdhms[0] = ymdhms[1] = 0;
      ymdhms[2] = (long)sd;   /* shorteger part of stardate is number of days */
      r = sd2hms( (sd - (double)ymdhms[2]), ymdhms ); /* fractional part of */
                                                  /* stardate to hr,min,sec */
      return (r);
}
