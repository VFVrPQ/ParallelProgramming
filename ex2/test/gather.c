#include "mpi.h"
#include <stdio.h>
int main( int argc, char* argv[] ){
    int i;
    int rank, nproc;
    int isend[2], irecv[64];
 
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &nproc );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    isend[0] = 2*rank;
    isend[1] = 2*rank+1;
    MPI_Gather( isend, 2, MPI_INT, irecv, 2, MPI_INT, 0,
                           MPI_COMM_WORLD);
    if(rank == 0) {
        for(i=0; i<nproc*2; i++)
            printf("irecv = %d\n", irecv[i]);
        }
    MPI_Finalize();
}

