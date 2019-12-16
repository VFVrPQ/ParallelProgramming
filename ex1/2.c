#include "mpi.h"
#include <stdio.h>

int main(int argc, char* argv[]){
        int i;
        int rank, nproc;
        int send[32], recv[32];
        int tag=100;
        MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        printf("Before : My rank = %d <", rank);
        for (i=0;i<nproc;i++)
                printf(" %d ", send[i] = i + nproc * rank);
        printf(" >\n");

        for (i=0;i<rank;i++){ // recv from i
                MPI_Recv(&recv[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
        }
        //begin{send}
        //rank+1 is the last one to send
        for (i=0;i<rank;i++){
                //printf("send from %d to %d\n", rank, i);
                MPI_Send(&send[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        for (i=rank+2;i<nproc;i++){
                //printf("send from %d to %d\n", rank, i);
                MPI_Send(&send[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        if (rank+1<nproc){
                //printf("send from %d to %d\n", rank, rank+1);
                MPI_Send(&send[rank+1], 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
        }
        //end{send}
        for (i=rank+1;i<nproc;i++){//recv from i
                MPI_Recv(&recv[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
        }
        printf("After : My rank = %d <", rank);
        for (i=0;i<nproc;i++){
                if (i==rank) printf("%d ", send[i]);
                else printf("%d ", recv[i]);
        }
        printf(" >\n");
        MPI_Finalize();
        return 0;
}