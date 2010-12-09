#include "wool.h"
#include <stdlib.h>
#include <stdio.h>

extern int loop(int);

LOOP_BODY_1( work, 100, int, i, int, n )
{
  loop( n );
}

TASK_2( int, main, int, argc, char **, argv )
{
  int grainsize = atoi( argv[1] );
  int p_iters = atoi( argv[2] );
  int s_iters = atoi( argv[3] );
  int i;

  for( i=0; i<s_iters; i++ ) {
    FOR( work, 0, p_iters, grainsize );
  }

  printf( "%d %d %d\n", grainsize, p_iters, s_iters );

  return 0;
}

