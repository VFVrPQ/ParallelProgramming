#include "mpi.h"
#include <stdio.h>


int P(int X){
        return X+3;
}
int Q(int X){
        return X*X;
}
int R(int X){
        return X+5;
}
int main(int argc, char *argv[]){
        int n = 5; // 5 times
        int rank, nproc;
        int currentX=-2, nextX=-2, currentY=-2, previousY=-2, count;
        MPI_Request send_handle, recv_handle;
        MPI_Status send_status, recv_status;
        int TAG[3]={1,2,3};
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0){
                nextX = 0;
                for (int i = 0; i < n+rank+1; i++){
                        currentX = nextX;
                        previousY = currentY;
                        nextX = i+1; // recv
                        if (i != rank)
                                MPI_Isend(&previousY, 1, MPI_INT, rank+1, TAG[rank], MPI_COMM_WORLD, &send_handle);//first is not send
                        currentY = P(currentX);
                        if (i != rank)
                                MPI_Wait(&send_handle, &send_status);
                }
        }else if (rank == 1){
                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);// i = rank
                MPI_Wait(&recv_handle, &recv_status);
                printf("i = %d nextX = %d rank = %d\n", rank-1, nextX, rank);
                for (int i = 0; i < n+rank+2; i++)if (i>rank){
                        //printf("rank = %d, i = %d\n", rank, i);
                        currentX = nextX;
                        previousY = currentY;
                        if (i < n+rank){//the last is not recv
                                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);
                        }
                        if (i != rank+1)
                                MPI_Isend(&previousY, 1, MPI_INT, rank+1, TAG[rank], MPI_COMM_WORLD, &send_handle);
                        currentY = Q(currentX); //the first is wrong
                        if (i < n+rank){
                                MPI_Wait(&recv_handle, &recv_status);
                                printf("i = %d nextX = %d rank = %d\n", i, nextX, rank);
                        }
                        if (i != rank+1)
                                MPI_Wait(&send_handle, &send_status);
                        //if (i != rank)
                        //      printf("currentX = %d currentY = %d recv_tag = %d\n",
                        //              currentX, currentY, recv_status.MPI_TAG);
                }
        }else if (rank == 2){
                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);
                MPI_Wait(&recv_handle, &recv_status);
                printf("i = %d nextX = %d\n", rank-1, nextX);
                for (int i = 0; i < n+rank+1; i++)if (i >= rank){
                        //printf("rank = %d, i = %d\n", rank, i);
                        currentX = nextX;
                        previousY = currentY;
                        if (i < n+rank-1)
                                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);
                        //if (i != rank)
                                //printf("previousY = %d\n",
                                //              previousY);
                        currentY = R(currentX);
                        if (i < n+rank-1){
                                MPI_Wait(&recv_handle, &recv_status);
                                printf("i = %d nextX = %d\n", i, nextX);
                        }
                }
        }
        MPI_Finalize();
        return 0;
}