#include "wool.h"
#include <stdio.h>
#include <stdlib.h>

// Basic matrix multiply code, no blocking, parallelization of outermost loop.

LOOP_BODY_4( mm, LARGE_BODY, int, i, int, rows, double*, a, double*, b, double*, c)
{
  int j;

  for( j=0; j<rows; j++ ) {
    double sum = 0.0;
    int k;
    for( k=0; k<rows; k++ ) {
      sum += a[i*rows+k]*b[k*rows+j];
    }
    c[i*rows+j] = sum;
  }
}


TASK_2(int, main, int, argc, char**, argv) {
  int i,j,ok;
  double *a,*b,*c;
  int rows;

  /* Decode arguments */

  if(argc < 2) {
    fprintf(stderr, "Usage: %s [wool options] <matrix rows>\n", argv[0]);
    exit(1);
  }
  rows = atoi(argv[1]);

  

  /* Allocate and initialize matrices */

  a = (double *) malloc(rows*rows*sizeof(double));
  b = (double *) malloc(rows*rows*sizeof(double));
  c = (double *) malloc(rows*rows*sizeof(double));

  for( i=0; i<rows; i++ ) {
    for( j=0; j<rows; j++ ) {
      a[i*rows+j] = 0.0;
      b[i*rows+j] = 0.0;
    }
    a[i*rows+i] = 1.0;
    b[i*rows+i] = 1.0;
  }


  /* Multiply matrices */

  FOR( mm, 0, rows, rows, a, b, c );

  /* Check result */

  ok = 1;
  for( i=0; i<rows; i++ ) {
    for( j=0; j<rows; j++ ) {
      if( i!=j && c[i*rows+j] != 0.0 ) {
        ok = 0;
      }
      if( i==j && c[i*rows+j] != 1.0 ) {
        ok = 0;
      }
    }
  }

  printf("Ok: %d\n", ok);
  return 0;

}
