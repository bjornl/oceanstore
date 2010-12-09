int reads( int size, int stride, char *buf )
{
  int i,s=0;

  for( i=0; i<size; i+=stride ) {
    s |= buf[i];
  }

  return s;
}
