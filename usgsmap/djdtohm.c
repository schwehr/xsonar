#include <math.h>

#include "map.h"

void djdtohm(double decjd, int *hour, int *min, double *sec)
#if 0
double decjd;
int *hour, *min;
double *sec;
#endif
/* subroutine to convert decimal julian day to hour, minute, and seconds */
{	
	int d;
	double dechour, decmin/*, decsec*/;

	d = (int)decjd;
	decjd -= d;
	dechour = decjd * 24.0;
	*hour = (int)dechour;
	decmin = (dechour - *hour) * 60.0;
	*min = (int)decmin;
	*sec = (decmin - *min) * 60.0;
/*	*sec = (int)decsec;  */
	if (*min >= 60.0)
	{
		*hour += 1;
		*min -= 60;
	}

        return;
}

