#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define V(i, j) V[i*n+j]
#define W(i, j) W[i*n+j]
double *W, *f, *V;
int n, each;
int rank, nproc;
MPI_Status status;
void readMatrix(){
    int i, j;
    if (rank==0){ //read
        char file[60]= {"1.txt"};
        //scanf("%s", file);
        FILE *fp = fopen(file, "r");
        fscanf(fp, "%d", &n);
        V = (double *)malloc(sizeof(double)*n*n);
        for (i=0;i<n;i++){
            for (j=0;j<n;j++){
                fscanf(fp, "%lf", &V(i,j));
                //if (i==0 && j<10)printf("V(%d, %d)=%.2lf\n", i, j, V(i, j));
            }
        }
        fclose(fp);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // n总点数

    each = n/nproc;
    if (each != 0) each++;

    W = (double*)malloc(sizeof(double)*each*n);
    if (rank == 0){
        for (i=0;i<n;i+=nproc) for (j=0;j<n;j++) W(i/nproc, j) = V(i, j);
    }
    for (i=0;i<n;i++)if (i%nproc!=0){
        MPI_Sendrecv(&V(i, 0), n, MPI_DOUBLE, i%nproc, i, 
                     &W(i/nproc, 0), n, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (i=rank;i<n;i+=nproc){
        printf("%d:", i);
        for (int j=0;j<n;j++){
            printf("%.5lf ", W(i/nproc, j));
        }
        printf("\n");
    }
}
int main(int argc, char *argv[]){
    int i;
    double starttime, endtime;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    starttime = MPI_Wtime();
    readMatrix();

    
    MPI_Finalize();
    return 0;
}