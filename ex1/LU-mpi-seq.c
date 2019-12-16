#include "mpi.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    double A[3][3] = {{1,2,3},
                                {4,5,6},
                                {3,-3,5}};
    double f[3];
    int n = 3;
    int i, p, j, k;
    int nproc, rank;
    int tag = 100;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int m = (n + nproc - 1) / nproc; // number
    //printf("%d\n", m);
    for (i = 0; i < n; i++){
            if (i/m == rank){ // rank = 0, is main row
                    for (k = 0; k < n; k++) f[k] = A[i][k]; // v->n-1 is ok.
            }else{
                    for (k = 0; k < n; k++) f[k] = 0;
            }
            MPI_Bcast(&f, n, MPI_DOUBLE, i/m, MPI_COMM_WORLD); // i/m
            int start = m * rank;
            if (start < i+1) start = i+1; // for rank <= i/m
            int end   = m * (rank + 1);
            if (end > n)   end   = n; // for the last rank

            for (k = start; k < end; k++){ // row
                    A[k][i] /= f[i];
                    for (j = i+1; j < n; j++){ // column
                            A[k][j] -= A[k][i] * f[j];
                    }
            }
            MPI_Barrier(MPI_COMM_WORLD);
    }
    
    if (rank == 0){
            for (i = 0; i < n; i++){
                    if (i/m == rank) continue;
                    for (k = 0; k < n; k++) A[i][k] = 0;
            }
            printf("Before Gather:\n");
            for (i = 0; i < n; i++){
                    for (j = 0; j < n; j++){
                            printf("%.2lf ", A[i][j]);
                    }
                    printf("\n");
            }
            printf("\n");
    }

    for (i = 0; i < n; i++){
            if (i/m == 0) continue;
            if (rank == 0){
                    MPI_Recv(&A[i], n, MPI_DOUBLE, i/m, tag, MPI_COMM_WORLD, &status);
            }else if (i/m == rank){
                    MPI_Send(&A[i], n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
            }
    }
    if (rank == 0){
            for (i = 0; i < n; i++){
                    for (j = 0; j < n; j++){
                            printf("%.2lf ", A[i][j]);
                    }
                    printf("\n");
            }
    }
    MPI_Finalize(); 
    return 0;
}