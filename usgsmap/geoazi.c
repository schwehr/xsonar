#include <math.h>
#ifndef __APPLE__
#include <values.h>
#endif

#include "map.h"

float geoazi (float phi1, float lam1, float phi2, float lam2)
     /* float phi1, lam1, phi2, lam2;	*/
/* returns azimuth between 1 and 2. Angles are all in decimal degrees */
{
	double sinphi1, sinphi2, cosphi1, cosphi2, cosdlam;
	double cosdelta, sindelta, cosazi;
	double azi; 
	double dphi1, dphi2, dlam1, dlam2;
	float fazi;

	dphi1 = phi1 * M_PI / 180.0;
	dlam1 = lam1 * M_PI / 180.0;
	dphi2 = phi2 * M_PI / 180.0;
	dlam2 = lam2 * M_PI / 180.0;
	sinphi1 = sin (dphi1);
	sinphi2 = sin (dphi2);
	cosphi1 = cos (dphi1);
	cosphi2 = cos (dphi2);
	cosdlam = cos (dlam2 - dlam1);

	cosdelta = sinphi1 * sinphi2 + cosphi1 * cosphi2 * cosdlam;
	cosdelta = fabs (cosdelta);
	sindelta = sqrt (1.0 - cosdelta*cosdelta);
	cosazi = (sinphi2 - sinphi1*cosdelta) / (cosphi1 * sindelta);
	if (cosazi > 1.0) cosazi = 1.0;
	if (cosazi < -1.0) cosazi = -1.0;
	azi = acos (cosazi);
	if (dlam2 < dlam1) azi = -azi;
	if (azi < 0) azi = azi + 2.0 * M_PI;
	fazi = azi * 180.0 / M_PI;
	return (fazi);
}

