#include "wool.h"
#include <stdio.h>
#include <stdlib.h>

int reads( int, int, char * );
char *buf;

int g, stride;

TASK_2( unsigned, tree, int, d, int, n )
{
  if( d>0 ) {
    int a,b;
    SPAWN( tree, d-1, n + ( 1 << (d+g-1) ) );
    b = CALL( tree, d-1, n );
    a = SYNC( tree );
    return a+b;
  } else {
    reads( 1<<g, 1<<stride, buf+n );
    return 1;
  }
}

TASK_2( int, main, int, argc, char **, argv )
{
  int i, d, n, m, np;
  unsigned sum=0;

  if( argc < 5 ) {
    fprintf( stderr, 
    "Usage: stress <log size> <depth> <log stride> <reps>\n" );
    return 1;
  }

  n  = atoi( argv[1] );
  np = 1 << n;
  d  = atoi( argv[2] );
  g  = n-d;
  stride = atoi( argv[3] );
  m  = atoi( argv[4] );

  buf = (char *) malloc( np * sizeof( char ) );

  for( i=0; i<m; i++) {
    sum += CALL( tree, d, 0 );
  }

  printf( "DONE: %u\n", sum );

  return 0;
}
