#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

enum endian {LITTLE,BIG,UNKNOWN_ENDIAN}; // aka LOW, HIGH

enum endian getHostEndian(void) {
  int short s=1;
  char *b = (char *)&s;
  if (b[0]) return (LITTLE);
  return (BIG);
}

int
main (int argc, char *argv[]) {
  switch (getHostEndian()) {
#if 0
  case LITTLE: printf("little\n"); break;
  case BIG: printf("big\n"); break;
#else
  case LITTLE: printf("-DINTEL=1 -DMOTOROLA=0\n"); break;
  case BIG:    printf("-DINTEL=0 -DMOTOROLA=1\n"); break;
#endif
  case UNKNOWN_ENDIAN: printf("ERROR: UNKNOWN endian\n"); exit(EXIT_FAILURE);
  default: printf("ERROR: unexpected endian\n"); exit(EXIT_FAILURE);
  };
  return(EXIT_SUCCESS);
}

/* NOTES - What machines return, what xsonar returns
 *
 * Darwin/Mac OSX/PPC - big
 * IRIX/SGI/Mips      - big,    MOTOROLA
 * Linux/x86          - little, INTEL
 */
