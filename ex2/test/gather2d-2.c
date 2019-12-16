#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]){
    int global[6][6];
    int local[3][3];
    int i, j;
    int rank, nproc;
    int tag =100;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype newtype, resizedtype;
    int sizes[2] = {6, 6};
    int subsizes[2] = {3, 3};
    int starts[2] = {0, 0};
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &newtype);
    MPI_Type_create_resized(newtype, 0, 3*sizeof(int), &resizedtype);
    MPI_Type_commit(&resizedtype);

    int counts[4] = {1, 1, 1, 1};
    int displs[4] = {0, 1, 6, 7};

    if (rank==0){
        for (i=0;i<6;i++) for (j=0;j<6;j++) global[i][j] = i*6+j;
    }
    MPI_Scatterv(global, counts, displs,
                resizedtype,
                local, 3*3, MPI_INT,
                0, MPI_COMM_WORLD);

    /*if (rank == 0){
        MPI_Send(&(global[0][3]), 1, newtype, 1, tag, MPI_COMM_WORLD);
        MPI_Send(&(global[3][0]), 1, newtype, 2, tag, MPI_COMM_WORLD);
        MPI_Send(&(global[3][3]), 1, newtype, 3, tag, MPI_COMM_WORLD);
    }else{
        MPI_Recv(&(local[0][0]), 3*3, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        for (i=0;i<3;i++) for (j=0;j<3;j++){
            printf("%d ", local[i][j]);
            if (j==2) printf("\n");
        }
    }*/

    MPI_Gatherv(&(local[0][0]), 3*3, MPI_INT, 
            global, counts, displs, resizedtype, 
            0, MPI_COMM_WORLD);

    if (rank == 0){
        for (i=0;i<6;i++) for (j=0;j<6;j++){
            printf("%d ", global[i][j]);
            if (j==5) printf("\n");
        }
    }
    MPI_Finalize();
    return 0;
}