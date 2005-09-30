#include <math.h>

#include "map.h"

/* FIX: could be int */
void ddtodegmin(double decdeg, int *deg, double *min)
     /*double decdeg, *min;*/
     /*int *deg;*/
/* subroutine to convert decimal degrees to degrees and minutes   */
{	
	float d;

	d = fabs(decdeg);
	*deg = (int)d;
	*min = (d - *deg) * 60.0;
	if(decdeg < 0.0)
		*deg = -*deg;
}

