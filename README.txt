07-July-2004:

Builds on IRIX and Mandrake 8.1 x86 in addition to Darwin
Removed -lXext -lXmu from the LIBFLAGS

06-July-2004:

xsonar-kds-0.3 release

Got most of the compiler warnings taken care of accept the OPTIMIZE=1
used uninitialized troubles.

Byte order should now work on pretty much any host.


05-July-2004:

xsonar-kds-0.2 release

Working on converting xsonar to ANSI C/C90 standards.  So far, it
compiles with almost no warnings with:

 -Wall -Wimplicit -Wimplicit-int -Wimplicit-function-declaration -W
 -Wstrict-prototypes

Still need to check bite order and void floatshuf(float *number) is
probably broken in utilities.c

Added __attribute__((__unused__)) to unused parameters for gcc.  Still
need to make this work for non-gcc compilers.

Still need to test on solaris and irix.