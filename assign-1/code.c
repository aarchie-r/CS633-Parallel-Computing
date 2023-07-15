#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main( int argc, char *argv[])
{
  int myrank, size; 
  MPI_Status status;
  double sTime, eTime, time, maxTime;

  MPI_Init(&argc, &argv);

  int count = atoi (argv[1])/4; // Size of int is 4 bytes, so we make an array of size/4 elememts
  int buf[count];

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank) ;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (int i=0; i<count; i++)
   buf[i] = myrank+i;

  sTime = MPI_Wtime();
  if (myrank == 0)
   MPI_Send (buf, count, MPI_INT, 1, 1, MPI_COMM_WORLD);	   
  else
   if (myrank == 1)
    MPI_Recv (buf, count, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);	   
  eTime = MPI_Wtime();
  time = eTime - sTime;
  MPI_Reduce (&time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (!myrank) printf ("%lf\n", maxTime);

  MPI_Finalize();
  return 0;

}
