/*
 * subroutine to convert decimal degrees to degrees, minutes, and seconds
 */

#include <math.h>
#include "map.h"

/* FIX: could be int return */
void ddtodegminsec(double decdeg, int *deg, int *min, double *sec)
#if 0
double decdeg, *sec;
int *deg, *min;
#endif
{	
	double d, m;

	d = fabs(decdeg);
	*deg = (int) d;
	m = (d - *deg) * 60.0;
	*min = (int) m;
	*sec = (m - *min) * 60.0;

	if(decdeg < 0.0)
		*deg = -*deg;
}

