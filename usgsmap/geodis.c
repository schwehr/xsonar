#include <math.h>
#ifndef __APPLE__
#include <values.h>
#endif

#include "map.h"

#if 0
#define WGS84        1
#define CLARKE1866   2
#endif

double geodis (double phi1, double lam1, double phi2, double lam2, int ellipsoid)	
     /*
double phi1, lam1, phi2, lam2;		
int ellipsoid;
     */
/* returns distance between 1 and 2. Angles are in decimal degrees */
{
	double e2;
	double f;
	double a;	
        double piValue;

	double dphi1, dphi2, dlam1, dlam2;
	double sinphi1, sinphi2, cosphi1, cosphi2, cosdlam, sinmphi;
	double cosdelta, delta, radius;
	double dis; 

/*

Ellips: Clarke 1866               A:6378206.4 B:6356583.8
   1/f: 294.9786982  e2: 0.006768657998

Ellips: WGS-84                    A:6378137.0 B:6356752.3
   1/f: 298.2572236  e2: 0.006694379990

*/


        switch(ellipsoid)
            {
            case WGS84:
                e2 = 0.006694379990;
                a = 6378137.0;
                break;

            case CLARKE1866:
                e2 = 0.006768657998;
                a = 6378206.4;
                break;

            default:  /* Clarke 1866 */
                e2 = 0.00676863;
                a = 6378206.4;
                break;
            }

	f = 1.0 - sqrt (1.0 - e2);
        piValue = 3.14159265358979323846;

	dphi1 = phi1 * piValue / 180.0;
	dlam1 = lam1 * piValue / 180.0;
	dphi2 = phi2 * piValue / 180.0;
	dlam2 = lam2 * piValue / 180.0;
	sinmphi = sin ((dphi1 + dphi2) / 2.0);
	sinphi1 = sin (dphi1);
	sinphi2 = sin (dphi2);
	cosphi1 = cos (dphi1);
	cosphi2 = cos (dphi2);
	cosdlam = cos (fabs(dlam2) - fabs(dlam1));

	cosdelta = sinphi1 * sinphi2 + cosphi1 * cosphi2 * cosdlam;
	cosdelta = fabs (cosdelta);
	delta = acos (cosdelta);
	radius = a * (1.0 - f * sinmphi * sinmphi);	/* radius of ellipsoid at mean phi */
	dis = (radius * delta);
	return (dis);
}
