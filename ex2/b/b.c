#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

int malloc2dLL(long long ***array, int n, int m){
    int i;
    long long *p = (long long*)malloc(n*m*sizeof(long long));
    if (!p) return -1;

    /* allocate the row pointers into the memory */
    (*array) = (long long **)malloc(n*sizeof(long long*));
    if (!(*array)) {
       free(p);
       return -1;
    }

    for (i=0;i<n;i++){
        (*array)[i] = &(p[i*m]);
    }
    return 0;
}
int free2dLL(long long ***array){
    free(&((*array)[0][0]));

    /* free the pointers into the memory */
    free(*array);
    return 0;
}
int main(int argc, char *argv[]){
    int i, j, u, v, k, round;
    int rank, nproc;
    int n, p, blocksize, row, col;
    int tag = 100;
    long long **global_A, **global_B, **global_C;
    long long **A, **B, **C;
    MPI_Datatype Line;
    MPI_Comm row_world, col_world;
    MPI_Status handler;
    if (argc < 2){
        printf("please input n");
        return -1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    n = atoi(argv[1]);
    if (rank == 0){
        malloc2dLL(&global_A, n, n);
        malloc2dLL(&global_B, n, n);
        malloc2dLL(&global_C, n, n);
        for (i=0;i<n;i++){
            for (j=0;j<n;j++){
                global_A[i][j] = i+j*n;
                global_B[i][j] = i*n+j;
                printf("%lld ", global_A[i][j]);
                //if (j==n-1) printf("\n");
            }
        }
    }

    //initial var, and matrix,
    //nproc = p*p
    p = sqrt(nproc);//n行，n列分成p行、p列
    blocksize = n/p;//每块的大小为blocksize
    row = rank / p;
    col = rank % p;
    
    int sizes[2]    = {n, n};         /* global size */
    int subsizes[2] = {n/p, n/p};     /* local size */
    int starts[2]   = {0,0};                        /* where this one starts */
    MPI_Datatype type, subarrtype;
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_LONG_LONG_INT, &type);
    MPI_Type_create_resized(type, 0, (n/p)*sizeof(long long), &subarrtype);
    MPI_Type_commit(&subarrtype);

    double starttime, endtime;
    starttime = MPI_Wtime();
    //创建A，B，C的矩阵
    malloc2dLL(&A, blocksize, blocksize);
    malloc2dLL(&B, blocksize, blocksize);
    malloc2dLL(&C, blocksize, blocksize);
    int sendcounts[p*p];
    int displs[p*p];

    if (rank == 0){
        for (i=0;i<p*p;i++) sendcounts[i] = 1;
        int disp = 0;
        for (i=0;i<p;i++){
            for (j=0;j<p;j++){
                displs[i*p+j] = disp;
                disp++;
                //printf("%d %d : %d %d\n", i, j, sendcounts[i*p+j], displs[i*p+j]);
            }
            disp += (n/p-1)*p;
        }
        
    }

    long long *globalptr_A = NULL;
    long long *globalptr_B = NULL;
    long long *globalptr_C = NULL;
    if (rank == 0){
        globalptr_A = &(global_A[0][0]);
        globalptr_B = &(global_B[0][0]);
        globalptr_C = &(global_C[0][0]);
    }
    //初始化A，B，C
    //if (rank == 0) printf("%p, %p\n", &(global_A[0][0]), &(global_B[0][0]));
    //printf("%p, %p\n", &(A[0][0]), &(B[0][0]));
    MPI_Scatterv(globalptr_A, sendcounts, displs, subarrtype, &(A[0][0]),
        blocksize*blocksize, MPI_LONG_LONG_INT,
        0, MPI_COMM_WORLD);
    MPI_Scatterv(globalptr_B, sendcounts, displs, subarrtype, &(B[0][0]),
        blocksize*blocksize, MPI_LONG_LONG_INT,
        0, MPI_COMM_WORLD);
    for (i=0;i<blocksize;i++){
        for (j=0;j<blocksize;j++){
            C[i][j] = 0;
        }
    }
    /*printf("rank=%d:", rank);
    for (int i=0;i<p;i++){
        for (int j=0;j<p;j++){
            printf("%lld ", A[i][j]);
        }
        printf("\n");
    }*/
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Comm_split(MPI_COMM_WORLD, row, col, &row_world); // color, key
    MPI_Comm_split(MPI_COMM_WORLD, col, row, &col_world); 

    long long *ibuf_temp = (long long *)malloc(blocksize*blocksize*sizeof(long long));
    long long *send = (long long *)malloc(blocksize*blocksize*sizeof(long long));
    long long *recv = (long long *)malloc(blocksize*blocksize*sizeof(long long));
    for (round=0;round<p;round++){
        //step 1
        if ((row+round)%p == col){
            for (i=0;i<blocksize;i++)for (j=0;j<blocksize;j++){
                ibuf_temp[i*blocksize+j] = A[i][j];
            }
        }
        MPI_Bcast(ibuf_temp, blocksize*blocksize, MPI_LONG_LONG_INT, (row+round)%p, row_world); // row=col的广播, 自动有Barrier
    
        //step 2
        for (i=0;i<blocksize;i++)for (j=0;j<blocksize;j++)for (k=0;k<blocksize;k++){
            C[i][j] += ibuf_temp[i*blocksize+k] * B[k][j]; // 只能用接收来的ibuf做矩阵乘法
        }
        MPI_Barrier(MPI_COMM_WORLD);
        
        //step 3
        for (i=0;i<blocksize;i++)for (j=0;j<blocksize;j++) send[i*blocksize+j] = B[i][j];
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Sendrecv(send, blocksize*blocksize, MPI_LONG_LONG_INT, (row-1+p)%p, tag,
                     recv, blocksize*blocksize, MPI_LONG_LONG_INT, (row+1)%p, tag, 
                     col_world, &handler);
        for (i=0;i<blocksize;i++)for (j=0;j<blocksize;j++) B[i][j] = recv[i*blocksize+j];
        MPI_Barrier(MPI_COMM_WORLD);
    }
    free(ibuf_temp);free(send);free(recv);

    if (rank==0){
        for (i=0;i<n;i++)for (j=0;j<n;j++)for (k=0;k<n;k++){
            global_C[i][j] += global_A[i][k] * global_B[k][j];
        }
        printf("right Matrix:\n");
        for (i=0;i<n;i++) for (j=0;j<n;j++){
            printf("%lld ", global_C[i][j]);
            if (j==n-1) printf("\n");
        }
    }
    MPI_Gatherv(&(C[0][0]), blocksize*blocksize, MPI_LONG_LONG_INT, 
                globalptr_C, sendcounts, displs, subarrtype, 
                0, MPI_COMM_WORLD);

    //MPI_Barrier(MPI_COMM_WORLD);
    free2dLL(&A); free2dLL(&B); free2dLL(&C);
    //free2dLL(&global_A); free2dLL(&global_B); 
    if (rank==0){
        printf("my Matrix:\n");
        for (i=0;i<n;i++) for (j=0;j<n;j++){
            printf("%lld ", global_C[i][j]);
            if (j==n-1) printf("\n");
        }
        printf("\n");
    }
    //free2dLL(&global_C);

    if (rank==0){
        endtime = MPI_Wtime();
        printf("time %.2lf\n", endtime-starttime);
    }
    MPI_Finalize();

    
    return 0;
}