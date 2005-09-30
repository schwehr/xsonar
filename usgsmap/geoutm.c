#include <math.h>
#ifndef __APPLE__
#include <values.h>
#endif

#include "map.h"

#if 0
#define WGS84        1
#define CLARKE1866   2
#endif

void geoutm (double phi, double lam, double clam, double *x, double *y, int ellipsoid)
#if 0
double phi, lam, clam;
double *x, *y;
int ellipsoid;
#endif
/* subroutine to convert latitude and longitude of a given point (degrees)
   returning x (easting) and y (northing) in UTM standards.
   Source for formulas: J.P. Snyder, Map projections used by the U.S.G.S.,
   call: geoutm (phi, lam, clam, &x, &y);
   Geol. Survey Bulletin 1532, p. 68, 1982.
   A. Malinverno
   March 3, 1986.	*/
{
	double k0; /* central scale factor */
	double x0; /* x0 to be added in standard UTM */
	double y0; /* y0 to be added in standard UTM for southern hemisphere */
	double e2;
	double a;
	double e4;
	double e6;
	double epr2;
	double cosphi, sinphi, tanphi;
	double n, t, t2, c;
	double b, b2, b3, b4, b5, b6;
	double m, m1, m2, m3, m4;
	double dphi, dlam, dclam;
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
                
	k0 = 0.9996;	
	x0 = 500000.0;
	y0 = 10000000.0; 

        e4 = e2 * e2;
        e6 = e2 * e2 * e2;
        epr2 = e2 / (1.0 - e2);

	dphi = phi * M_PI / 180.0;
	dlam = lam * M_PI / 180.0;
	dclam = clam * M_PI / 180.0;
	cosphi = cos(dphi);
	sinphi = sin(dphi);
	tanphi = tan(dphi);
	n = a / sqrt(1.0 - e2 * sinphi * sinphi);
	t = tanphi * tanphi;
	t2 = t * t;
	c = epr2 * cosphi * cosphi;
	b = (dlam - dclam) * cosphi;	/* b is A in Snyder's formulas */
	b2 = b * b;
	b3 = b * b * b;
	b4 = b * b * b * b;
	b5 = b * b * b * b * b;
	b6 = b * b * b * b * b * b;
	m1 = 1.0 - e2/4.0 - 3.0*e4/64.0 - 5.0*e6/256.0;
	m2 = 3.0*e2/8.0 + 3.0*e4/32.0 + 45.0*e6/1024.0;
	m3 = 15.0*e4/256.0 + 45.0*e6/1024.0;
	m4 = 35.0*e6/3072.0;
	m = a * (m1*dphi - m2*sin(2.0*dphi) + m3*sin(4.0*dphi) - m4*sin(6.0*dphi));
	*x = k0 * n * (b + (1.0-t+c)*b3/6.0 + (5.0-18.0*t+t2+72.0*c-58.0*epr2)*b5/120.0);
	*x = *x + x0;
	*y = k0 * (m + n*tanphi*(b2/2.0 + (5.0-t+9.0*c+4.0*c*c)*b4/24.0 + (61.0-58.0*t+t2+600.0*c-330.0*epr2)*b6/720.0));
	if (dphi < 0.0) *y = *y + y0;
}

