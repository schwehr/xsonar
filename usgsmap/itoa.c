#include <string.h>

#include "map.h"

/* FIX: could return an int */
void itoa (int n, char s[])
#if 0
char s[];              
int n;
#endif
/* Subroutine to convert an integer n to a string s. Dimension the string as
   s[length]. From K & R. */
{
	int i, sign;
	if ((sign = n) < 0)   
		n = - n;      
	i = 0;
	do {                   /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   
	} while ((n /= 10) > 0);        
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

/* FIX: could return an int */
void reverse(char s[])
     /* char s[]; */
/* reverses string s in place */
{
	int c, i, j;
	for (i =  0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
