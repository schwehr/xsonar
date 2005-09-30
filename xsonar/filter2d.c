/**************************************************************************

filter2d - Convolve a MATRIX with a filter kernel

Requires pointer to input MATRIX, pointer to filter kernel.
Returns pointer to output matrix.
Exits with error message if unable to allocate output matrix.

void filter2d(Matrix *inMatrix , Matrix *outMatrix, Matrix kernel)

*************************************************************************/

#include <Xm/Label.h>

#include "xsonar.h"

#include "sonar_struct.h"

void filter2d(Matrix *inMatrix, Matrix *outMatrix, Matrix *kernel, char *type, MainSonar *sonar, int addBack, GC gc)
{

  Widget cancelButton;
  Widget progressWindow;

  Arg args[4];

  Cardinal n;

  int row, column, i, j, sumval, normal_factor;
  int kernel_rows, kernel_cols, dead_rows, dead_cols;
  int kernelSize;
  int count;
  int cancelCheck;
  int tempInt; /* FIX: can be used uninitialized */

  float percentDone;

  Dimension progressWindowWidth;
  Dimension progressWindowHeight;
  Dimension drawAmount;

  char  *filptr;

  unsigned char  *outptr, *inptr;
  unsigned char *BufferPtr;

  /*void showProgress();*/

  cancelButton = XtNameToWidget(sonar->toplevel, "*CancelButton");

  /* 
   *  Set the size of the kernel rows and columns 
   */ 

  kernel_rows = kernel->rows;
  kernel_cols = kernel->columns;

  kernelSize = kernel_rows * kernel_cols;

  dead_rows = kernel_rows/2;
  dead_cols = kernel_cols/2;

  /* 
   *  Calculate the normalization factor for the kernel matrix 
   */

  normal_factor = 0;
  for (row=0; row < kernel_rows; row++)
      {
      filptr = (char *) kernel->elements[row];
      for (column=0; column < kernel_cols; column++)
          {
          normal_factor += (int)filptr[column];
          }
      }

  /* 
   *  Make sure the normalization factor is not 0 
   */

  if(!normal_factor)
      normal_factor = 1;

  /*
  printf("normalization factor = %d\n", normal_factor);
  */

  /*
   *  Allocate space for the median buffer.
   */

  BufferPtr = (unsigned char *) calloc(kernelSize, sizeof(unsigned char));

  progressWindow = XtNameToWidget(sonar->toplevel, 
                                     "*StatusDialog*DrawProgress");
  XtVaGetValues(progressWindow, XmNwidth, &progressWindowWidth, NULL);
  XtVaGetValues(progressWindow, XmNheight, &progressWindowHeight, NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("Filtering Data ...", 
              XmSTRING_DEFAULT_CHARSET)); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetValues(XtNameToWidget(sonar->toplevel, "*StatusDialog*StatusMessage"),
                   args, n);

  for (row=0; row < (int)((inMatrix->rows) - kernel_rows + 1); row++)
      {

  /*
      if(row%50 == 0)
          fprintf(stderr, "filtering record # %d\r", row);
  */

      percentDone = ( (float)row / (float)inMatrix->rows) / 3 + 0.3333;
      drawAmount = (Dimension) (percentDone * (float)progressWindowWidth);

      XDrawLine(XtDisplay(sonar->toplevel), XtWindow(progressWindow),
                 gc, drawAmount, 0, drawAmount, progressWindowHeight);

      XmUpdateDisplay(sonar->toplevel);


      /*
       *   Check for events in the queue, if any, and dispatch them.
       */

      while(XtAppPending(sonar->toplevel_app))
          XtAppProcessEvent(sonar->toplevel_app, XtIMAll);

      /*
       *    Check on the status of the cancel button.
       */

      XtVaGetValues(cancelButton, XmNuserData, &cancelCheck, NULL);
      if(cancelCheck)
          {
          free(BufferPtr);
          return;
          }

      outptr = (unsigned char *) outMatrix->elements[row+dead_rows];

      for (column=0; column < (int)((inMatrix->columns) - kernel_cols + 1); column++)
          {

          sumval = 0;
          count = 0;
  
          for (i = 0; i < kernel_rows; i++)
              {

              /* row offset */
              inptr = (unsigned char *)inMatrix->elements[i+row];

              /* col offset */
              inptr = inptr + column;

              filptr = (char *) kernel->elements[i];

              for (j = 0; j < kernel_cols; j++)
                 {
/*
fprintf(stderr, "element [%d][%d] of kernel = %d\n", i + row, j, kernel->elements[i+row][j]);
fprintf(stderr, "element [%d][%d] of input = %d\n",i + row, j + column, *inptr);
*/
                 if(!strcmp(type, "hpfk") || !strcmp(type, "lpf"))
                     sumval += (*inptr++) * (*filptr++);

                 if(!strcmp(type, "med"))
                     BufferPtr[count++] = *inptr++;

                 if(!strcmp(type, "hpfs"))
                     {
                     BufferPtr[count++] = *inptr++;
                     sumval += (*inptr++);
                     }

                 }
              }

          if(!strcmp(type, "hpfk") || !strcmp(type, "lpf"))
              tempInt = sumval/normal_factor + addBack;

          if(!strcmp(type, "hpfs"))
              tempInt = (int)BufferPtr[(kernelSize - 1) / 2] -
                                       (int)(sumval/normal_factor) + addBack;

          if(!strcmp(type, "med"))
              tempInt = (int)c_median(BufferPtr, kernelSize) + addBack;

          if(tempInt < 0)
              tempInt = 0;
          if(tempInt > 255)
              tempInt = 255;

          outptr[column + dead_cols] = (unsigned char) tempInt;

/*
fprintf(stdout, "type  = %s\n", type);
fprintf(stdout, "sumval  = %d\n", sumval);
fprintf(stdout, "output filtered element  = %d\n", outptr[column+dead_cols]);
fprintf(stdout, "\n");
*/

          }
      }

  free(BufferPtr);

  printf("\n");

}
