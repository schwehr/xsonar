#include <math.h>

#include "map.h"
double hmtodjd (int jd, int hour, int min, double sec)
#if 0
int jd, hour, min;
double sec;
#endif
{
        double decjd;

        decjd = ((double)jd + (double)hour/24.0 + (double)min/1440.0 + sec/86400.0);
        return (decjd);
}
