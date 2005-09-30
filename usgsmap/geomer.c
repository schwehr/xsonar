#include <math.h>
#ifndef __APPLE__
#  include <values.h>
#endif

#include "map.h"

/* FIX: could return an int */
void geomer (float phi, float lam, float clam, float cphi, float scale,
	     float *x, float *y)
     /*     float phi, lam, clam, cphi, scale, *x, *y; */

/* Subroutine to convert latitude and longitude of a given point into
   x and y in rectangular Mercator coordinates (millimeters; scale is 1:scale
   at central latitude cphi) with origin at the equator and central longitude
   (clam). For a scale of 1 / x, scale equals x.
   Call: 	geomer (phi, lam, clam, cphi, scale, &x, &y);
   Source for formulas: J.P. Snyder, Map projections used by the U.S.G.S.,
   Geol. Survey Bulletin 1532, p. 50, 1982.
   A. Malinverno
   March 7, 1986.	*/
{
	double dphi, dlam, dclam, dcphi;
	double e2 = 0.00676863;		/* Clarke 1866 ellipsoid */
	double a = 6378206.4;
	double e4 = e2 * e2;
	double e6 = e2 * e2 * e2;
	double sinphi, sin3phi, sin5phi;
	double t, scaling;
	/* double sin(), cos(), sqrt(), log(); */

	dphi = phi * M_PI / 180.0;
	dlam = lam * M_PI / 180.0;
	dclam = clam * M_PI / 180.0;
	dcphi = cphi * M_PI / 180.0;
	sinphi = sin(dphi);
	sin3phi = sin(3.0 * dphi);
	sin5phi = sin(5.0 * dphi);
	scaling = (1000.0/scale) * cos(dcphi) / sqrt(1.0 - e2*sin(dcphi)*sin(dcphi));

	*x = a * (dlam - dclam);
	t = tan(M_PI/4.0 + dphi/2.0);
	*y = a * (log (t) - (e2 + e4/4.0 + e6/8.0)*sinphi + 
		(e4/12.0 + e6/16.0)*sin3phi - (e6/80.0)*sin5phi);
	*x = *x * scaling;
	*y = *y * scaling;
}
