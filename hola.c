/* Ejemplo Hola */
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
int main (int argc, char *argv[])
{
int rank, size;
char hostname[256];
MPI_Init (&argc, &argv);
MPI_Comm_rank (MPI_COMM_WORLD,&rank);
MPI_Comm_size (MPI_COMM_WORLD, &size);
gethostname(hostname,255);
printf( "Hola desde el proceso %d de %d en el nodo %s\n", rank,
size, hostname);
MPI_Finalize();
return 0;
}