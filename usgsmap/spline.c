/* spline.c
   subprogram that uses the spline algorithm in Cheney and Kincaid, 
   "Numerical Mathematics and Computing", Brooks/Cole, p. 258 ff., 1985.
   -	inputt is the input time or x coordinate;
   -	inputy is the input function to interpolate;
   -	inputn is the number of input points;
   -	outputn is the number of interpolations, and must be passed by the 
   	calling program. Note that this subroutine returns outputn + 1
	points, because of the endpoint.
   A. Malinverno
   Sep. 9 1986	*/

#include <stdio.h>

#include "map.h"

int spline (double *inputt, double *inputy, int inputn, double *outputt, double *outputy, int outputn)
#if 0
double *inputt, *inputy, *outputt, *outputy;
int inputn, outputn;
#endif
{
	int i;
	double t0, tmax, deltat, t;
	double z[1000];
#if 0
	double spl3();
	void zspl3();
#endif

	t0 = inputt[0];
	tmax = inputt[inputn - 1];
	deltat = (tmax - t0) / outputn;
	zspl3 (inputt, inputy, z, inputn);
	for (i = 0; i < outputn + 1; i++)
	{
		t = t0 + i * deltat;
		outputt[i] = t;
		outputy[i] = spl3 (t, inputt, inputy, z, inputn);
	}
	return (i);
}
	
void zspl3 (double *t, double *y, double *z, int n)
#if 0
double *t, *y, *z;
int n;
#endif
/* subroutine to compute the z's and return them in the z array */
{
	double h[1000], b[1000], u[1000], v[1000];
	int i;

	for (i = 0; i < (n - 1); i++)
	{
		h[i] = t[i+1] - t[i];  /* diff of next time and current time */
		b[i] = (y[i+1] - y[i]) / h[i];
	}
	u[1] = 2.0 * (h[0] + h[1]);
	v[1] = 6.0 * (b[1] - b[0]);
	for (i = 2; i < (n - 1); i++)
	{
		u[i] = 2.0 * (h[i] + h[i-1]) - h[i-1] * h[i-1] / u[i-1];
		v[i] = 6.0 * (b[i] - b[i-1]) - h[i-1] * v[i-1] / u[i-1];
	}
	z[n-1] = 0.0;
	for (i = (n - 2); i > 0; i--)
		z[i] = (v[i] - h[i] * z[i+1]) / u[i];

	z[0] = 0.0;

}

double spl3 (double x, double *t, double *y, double *z, int n)
#if 0
     double x, *t, *y, *z;
     int n;
#endif
/* returns the value of the spline at x */
{

	double sply, diff, h, b, p;
	int i;

	for (i = (n - 2); i > 0; i--)
	{
		diff = x - t[i];
		if (diff >= 0.0) break;
	}
	if (diff < 0.0)
	{
		i = 0;
		diff = x - t[0];
	}
	h = t[i+1] - t[i];
	b = (y[i+1] - y[i]) / h - h * (z[i+1] + 2.0 * z[i]) / 6.0;
	p = 0.5 * z[i] + diff * (z[i+1] - z[i]) / (6.0 * h);
	p = b + diff * p;
	sply = y[i] + diff * p;
	return (sply);
}

