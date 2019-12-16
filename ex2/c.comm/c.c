#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int MAX_ROUND = 3;
    int nproc, rank;
    int my_nproc;
    int P, Q;
    int tag = 100;
    int round, i;
    int send, *recv;
    int col, val;
    long long sum;
    double avg;
    char hostname[60];
    MPI_Request send_handle, *recv_handle;
    MPI_Status recv_status;
    MPI_Comm my_world;

    gethostname(hostname, sizeof(hostname));
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    P = 3; //前p个是服务器
    col = rank % P;
    val = rank / P;
    MPI_Comm_split(MPI_COMM_WORLD, col, val, &my_world);
    
    if (rank < P){
        MPI_Comm_size(my_world, &my_nproc);
        recv_handle = (MPI_Request*)malloc(sizeof(MPI_Request)*my_nproc);
        recv = (int*)malloc(sizeof(int)*my_nproc);
        //printf("my_nproc=%d \n", my_nproc);
    }
    srand(time(NULL));
    for (round = 0; round < MAX_ROUND; round++){
        if (rank < P){// server
            for (i=1;i<my_nproc;i++){ //i=0 itself
                MPI_Irecv(&recv[i], 1, MPI_INT, i*P+rank%P, tag, MPI_COMM_WORLD, &(recv_handle[i]));
            }
            avg = 0.0;
            
            for (i=1;i<my_nproc;i++){
                MPI_Wait(&(recv_handle[i]), &recv_status);
                avg += recv[i];
            }
            avg /= (my_nproc-1);
            printf("node=%s round=%d server=%d: avg=%.5lf, from ", hostname, round, rank%P, avg);
            for (i=1;i<my_nproc;i++){
                printf("%d ", i*P+rank%P);
            }
            printf("\n");
            
           
        }else{ // client
            send = rand();
            MPI_Isend(&send, 1, MPI_INT, rank%P, tag, MPI_COMM_WORLD, &send_handle);
            //发送完成了就行，不需要对面是否收到
            printf("node=%s round=%d client=%d: send=%d\n", hostname, round, rank, send);
        }
        MPI_Bcast(&avg, 1, MPI_DOUBLE, 0, my_world);
        //printf("node=%s, rank=%d, avg=%.5lf\n", hostname, rank, avg);
    }
    MPI_Finalize();
    return 0;
}