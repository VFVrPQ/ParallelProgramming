#include "mpi.h"
#include <stdio.h>

int main( int argc, char* argv[] )
{
    int rank;
    int ibuf[5];
    int i;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    if(rank == 0) {
        for (i=0;i<5;i++) ibuf[i]=i*i;
    }
    else {
        for (i=0;i<5;i++) ibuf[i]=0; // set ibuf Zero for non-root processes
    }
    MPI_Bcast(ibuf, 5, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank !=0 ){
        printf("my rank = %d ", rank);
        for (int i=0;i<5;i++) printf("ibuf[%d] = %d, ", i,ibuf[i]);
        printf("\n");
    }
    MPI_Finalize();
}
