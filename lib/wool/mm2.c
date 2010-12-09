#include "wool.h"
#include <stdio.h>
#include <stdlib.h>

// Blocks for cache

void mul_block( double* a, double* b, double* c, int rows, int bs, int ii, int jj, int kk )
{
  int ilim = ii+bs <= rows ? ii+bs : rows;
  int i;
  //printf( "ii: %d jj %d kk: %d\n", ii, jj, kk );
  for( i = ii; i < ilim; i++ ) {
    int jlim = jj+bs <= rows ? jj+bs : rows;
    int j;
    for( j = jj; j < jlim; j++ ) {
      int klim = kk+bs <= rows ? kk+bs : rows;
      double sum = kk==0 ? 0.0 : c[i*rows+j];
      int k;
      for( k = kk; k < klim; k++ ) {
        sum += a[i*rows+k]*b[k*rows+j];
      }
      c[i*rows+j] = sum;
    }
  }
}

LOOP_BODY_5( mm, LARGE_BODY, int, ii, int, rows, int, bs, double*, a, double*, b, double*, c)
{
  int i = ii*bs, j, k;

  for( j=0; j<rows; j+=bs ) {
    for( k=0; k<rows; k+=bs ) {
      mul_block( a, b, c, rows, bs, i, j, k );
    }
  }
}


TASK_2(int, main, int, argc, char**, argv) {
  int i,j,ok;
  double *a,*b,*c;
  int rows, bs;

  /* Decode arguments */

  if(argc < 3) {
    fprintf(stderr, "Usage: %s [wool options] <matrix rows> <blocksize>\n", argv[0]);
    exit(1);
  }
  rows = atoi(argv[1]);
  bs = atoi(argv[2]);

  printf( "%d rows, blocksize %d\n", rows, bs );

  

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

  FOR( mm, 0, (rows+bs-1)/bs, rows, bs, a, b, c );

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
