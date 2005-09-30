/*
 *   kernals.h
 *
 *   Various convolutions kernals used in the filtering of input images.
 *   
 */

char gaussian3x3[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
        };
      
char gaussian5x5[5][5] = {
        {1, 1, 2, 1, 1},
        {1, 2, 4, 2, 1},
        {2, 4, 8, 4, 2},
        {1, 2, 4, 2, 1},
        {1, 1, 2, 1, 1}
        };

char hiPassA[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
        };
      
char hiPassB[3][3] = {
        {-1, -1, -1},
        {-1,  9, -1},
        {-1, -1, -1}
        };
      
char hiPassD[3][3] = {
        { 1,-2, 1},
        {-2, 5,-2},
        { 1,-2, 1}
        };
      
