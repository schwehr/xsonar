#include "map.h"

/*      nec2dec.c
        subroutine to convert NEC single precision floating point values
        (IEEE format) to DEC single precision floating point format.

        Usage:  nec2dec(array_address, #values_to_convert)
                converted values are left in original array.
                From lowest address in NEC to highest:
                low byte,high word....lsb exponent,7 msb's mantissa
                high byte,high word...sign bit, 7 msb's exponent
                low byte, low word....lowest 8 bits mantissa
                high byte, low word...mantissa bits 8 thru 15
                NOTE:   DEC exponent is excess 128 (1000 0000)
                        NEC exponent is excess 127 (0111 1111)
                        DEC mantissa is < 1.0  with leading binary 1 implied
                        NEC mantissa is >=1.0  with leading binary 1 implied
                therefore, DEC value is 1 bit more precise but with a factor
                of 2 less range
*/
/* FIX: could return an int */
void nec2dec(float *a,int n)
#if 0
float *a;
int n;
#endif
        {
        float x;
	long  *w1,*w2;
        short int i,/*j,*/*z,y;

       z = (short *)&x;
	w1 = (long *)a;
	w2 = (long *)z;

        for(i=0;i<n;i++)
                {
                *w2 = w1[i];  /* had to pass as longs to avoid illegal
				floating point instruction */
                if((z[1] & 0x7f80) == 0x7f80) 
                /* exp = 255 , reserved NEC (NAN or INF); reserved DEC */
                        {
                        z[0] = 0x8000; /* DEC format, high word */
                        z[1] = 0x0000;
                        continue;
                        }
                else if((z[1] & 0x7f80) == 0) /* exp == 0? */
                        {
                        if(( (z[1] & 0x007f) == 0) && ( (z[0] & 0xffff) == 0))
/* if both NEC exp and mantissa = 0, value = 0; make a DEC zero */
                                {
                                z[0] = 0x0000; /* DEC format, high word */
                                z[1] = 0x0000;
                                }
/* if NEC exp = 0, and NEC mantissa .NE. 0, then NEC denormal; treat as
        underflow for DEC and make a zero */
                        else    
                                {
                                z[0] = 0x0000; /* DEC format, high word */
                                z[1] = 0x0000;
                                }
                        }
                else  /* it's a good number */
                        {
                        y = z[1] + 256; /* bump exp by 2 */
/* need a bump of 2 since DEC exp is excess 128, and DEC mantissa is 1 bit
        more precise, and therfore half the value of the NEC mantissa */
                        z[1] = z[0];
                        z[0] = y;
                        a[i] = x;
                        }
                }
        }
