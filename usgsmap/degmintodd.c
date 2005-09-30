#include <math.h>

#include "map.h"

/* FIX: could be int return */
void degmintodd(double *decdeg, int deg, double min)
#if 0
double *decdeg;
double min;
int deg;
#endif
/* subroutine to convert degrees and minutes to decimal degrees   */

{	
        double fractdeg;	

        fractdeg = min / 60.0;
        if (deg < 0)
                fractdeg = (0.0 - fractdeg);
	*decdeg = (double)deg + fractdeg;
}

