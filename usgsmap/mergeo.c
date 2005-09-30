#include <math.h>
#ifndef __APPLE__
#include <values.h>
#endif

#include "map.h"

/* FIX: could return an int */
void mergeo (float *phi, float *lam, float clam, float cphi, float scale, float x, float y)
     /* float *phi, *lam, clam, cphi, scale, x, y; */
/* Subroutine to convert x and y of a given point in rectangular Mercator 
   coordinates (millimeters; scale is 1/scale at central latitude cphi;
   origin is at the equator and central longitude clam) into latitude 
   and longitude.
   Call: 	mergeo (&phi, &lam, clam, cphi, scale, x, y);
   Source for formulas: J.P. Snyder, Map projections used by the U.S.G.S.,
   Geol. Survey Bulletin 1532, p. 50, 1982.
   A. Malinverno
   March 7, 1986.	*/
{
	double dclam, dcphi, dx, dy;
	double e2 = 0.00676863;		/* Clarke 1866 ellipsoid */
	double a = 6378206.4;
	double e4 = e2 * e2;
	double e6 = e2 * e2 * e2;
	double chi, sin2chi, sin4chi, sin6chi;
	double t, scaling;
	/* double sin(), exp(), atan(); */

	dclam = clam * M_PI / 180.0;
	dcphi = cphi * M_PI / 180.0;
	scaling = (1000.0/scale) * cos(dcphi) / sqrt(1.0 - e2*sin(dcphi)*sin(dcphi));
	dx = x / scaling;
	dy = y / scaling;
	*lam = dclam + dx / a;
	*lam = *lam * 180.0 / M_PI;
	t = exp (- dy / a);
	chi = (M_PI / 2.0) - 2.0 * (atan (t));
	sin2chi = sin(2.0 * chi);
	sin4chi = sin(4.0 * chi);
	sin6chi = sin(6.0 * chi);
	*phi = chi + (e2/2.0 + 5.0*e4/24.0 + e6/12.0)*sin2chi + 
		(7.0*e4/48.0 + 29.0*e6/240.0)*sin4chi + 
		(7.0*e6/120.0)*sin6chi;
	*phi = *phi * 180.0 / M_PI;
}
