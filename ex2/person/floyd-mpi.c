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

    //printf("rank=%d %d\n", rank, each);
    W = (double*)malloc(sizeof(double)*each*n);
    if (rank == 0){
        for (i=0;i<n;i+=nproc) for (j=0;j<n;j++) W(i/nproc, j) = V(i, j);
        //for (j=0;j<10;j++) printf("%.2lf ", W(0, j));
        //printf("\n");
        for (i=0;i<n;i++)if (i%nproc!=0){
            MPI_Send(&V(i, 0), n, MPI_DOUBLE, i%nproc, i, MPI_COMM_WORLD);
        }
    }else{
        for (i=rank;i<n;i+=nproc){
            //printf("rank=%d, %d\n", rank, i/nproc);
            MPI_Recv(&W(i/nproc, 0), n, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, &status); // no &!
            /*if (rank==1 && i==5){
                printf("***\n");
                for (j=0;j<10;j++) printf("%.2lf ", W(i/nproc, j));
                printf("***\n");
            }*/
        }
    }

    //printf("rank=%d\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("rank=%d**\n", rank);

    /*if (rank==1){
        for (i=0;i<10;i++) printf("%.2lf ", W(0, i));
        printf("\n");
    }*/
}

void floyd(){
    int i, j, k;
    f = (double*)malloc(sizeof(double)*n);
    for (k=0;k<n;k++){
        if (k%nproc == rank){
            for (j=0;j<n;j++) f[j] = W(k/nproc, j);//

            for (i=0;i<nproc;i++) if (i!=rank){
                MPI_Send(f, n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
            }
        }else{ 
            MPI_Recv(f, n, MPI_DOUBLE, k%nproc, rank, MPI_COMM_WORLD, &status);
        }
        for (i=rank;i<n;i+=nproc){
            for (j=0;j<n;j++){
                if (W(i/nproc,j) > W(i/nproc,k)+f[j]){
                    W(i/nproc,j) = W(i/nproc,k)+f[j];
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    free(f);
}
int main(int argc, char *argv[]){
    double starttime, endtime;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    starttime = MPI_Wtime();
    readMatrix();
    floyd();

    
    //0 -> 其他点
    if (rank==0){
        for (int i=0;i<n;i++) printf("%d %lf\n", i, W(0, i));
        endtime = MPI_Wtime();
        printf("Time = %.2lf\n", endtime - starttime);
    }
    MPI_Finalize();
    return 0;
}