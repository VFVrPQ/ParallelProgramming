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
                for (int i = rank; i < n+rank+2; i++){
                        nextX = i; // recv
                        if (i >= rank + 2) //timeStamp=2, begin to send
                                MPI_Isend(&previousY, 1, MPI_INT, rank+1, TAG[rank], MPI_COMM_WORLD, &send_handle);//first is not send
                        if (i > rank) // timeStamp=rank, has no useful value 
                                currentY = P(currentX);
                        //recv wait
                        if (i >= rank + 2) //send wait
                                MPI_Wait(&send_handle, &send_status);
                        currentX  = nextX;
                        previousY = currentY;
                }
        }else if (rank == 1){// can be changed to 1<=rank<=nproc-2
                for (int i = rank; i < n+rank+2; i++){
                        if (i < n + rank) // recv n times
                                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);
                        if (i >= rank + 2)
                                MPI_Isend(&previousY, 1, MPI_INT, rank+1, TAG[rank], MPI_COMM_WORLD, &send_handle);
                        if (i > rank)
                                currentY = Q(currentX);
                        if (i < n + rank){
                                MPI_Wait(&recv_handle, &recv_status);
                                //printf("i = %d nextX = %d rank = %d\n", i, nextX, rank);
                        }
                        if (i >= rank + 2)
                                MPI_Wait(&send_handle, &send_status);
                        currentX  = nextX;
                        previousY = currentY;
                }
        }else if (rank == 2){//rank = nproc-1
                for (int i = rank; i < n+rank+2; i++){
                        //printf("rank = %d, i = %d\n", rank, i);
                        if (i < n + rank)
                                MPI_Irecv(&nextX, 1, MPI_INT, rank-1, TAG[rank-1], MPI_COMM_WORLD, &recv_handle);
                        if (i > rank){
                                currentY = R(currentX);
                                printf("currentY = %d\n", currentY);
                        }
                        if (i < n + rank){
                                MPI_Wait(&recv_handle, &recv_status);
                        }
                        currentX  = nextX;
                        previousY = currentY;
                }
        }
        MPI_Finalize();
        return 0;
}