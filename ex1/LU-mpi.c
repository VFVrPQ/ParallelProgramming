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

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int m = (n + nproc - 1) / nproc; //assume n=nproc=3, m=1
        for (i = 0; i < m; i++){
                for (p = 0; p < nproc; p++){
                        int v = i * nproc + p; // the v-th row
                        if (v > n) break; //
                        if (p == rank){
                                for (k = 0; k < n; k++) f[k] = A[v][k]; // v->n-1 is ok.
                        }else{
                                for (k = 0; k < n; k++) f[k] = 0;
                        }
                        MPI_Bcast(&f, n, MPI_DOUBLE, p, MPI_COMM_WORLD);

                        int start = i * nproc + rank;
                        if (rank <= p) start += nproc; // rows is under v, so add m
                        for (k = start; k < n; k += nproc){ // row
                                A[k][v] /= f[v];
                                for (j = v+1; j < n; j++){ // column
                                        A[k][j] -= A[k][v] * f[j];
                                } 
                        }
                        MPI_Barrier(MPI_COMM_WORLD);
                }
        }

        if (rank == 0){
                for (i = 0; i < n; i++){
                        if (i % nproc == rank) continue;
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

        for (i = 0; i < m; i++){
                for (k = 0; k < n; k++) f[k] = A[i*nproc+rank][k];
                MPI_Gather(&f, n, MPI_DOUBLE, A+i*nproc, n, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
        }
        if (rank == 0){
                //printf("rank = %d : ", rank);
                //i = rank;
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