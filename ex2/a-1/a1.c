#include <stdio.h>
#include "mpi.h"
#include <unistd.h>

int main(int argc, char *argv[]){
        int rank, nproc;
        int ibuf, count;
        int send, recv;
        int i;
        int tag=100;
        MPI_Request send_handle, recv_handle;
        MPI_Status send_status, recv_status;
        char hostname[60];
        double starttime, endtime;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        gethostname(hostname, sizeof(hostname));
        
        starttime = MPI_Wtime();
        ibuf = rank; // initial
        for (i=1;i<nproc;i*=2){
            send = ibuf;
            MPI_Irecv(&recv, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &recv_handle);
            MPI_Isend(&send, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &send_handle);
            
            MPI_Wait(&recv_handle, &recv_status);
            ibuf += recv;
            MPI_Barrier(MPI_COMM_WORLD);
        }
        endtime = MPI_Wtime();
        printf("hostname=%s proc %d: sum = %d runtime=%.5lf\n", hostname, rank, ibuf, endtime-starttime);
        //printf("proc %d: sum = %d runtime=%.5lf\n", rank, ibuf, endtime-starttime);
        MPI_Finalize();
        return 0;
}

//MPI_Get_count(&recv_status, MPI_INT, &count);
//printf(" recv = %d source = %d tag = %d count = %d\n",
//                recv, recv_status.MPI_SOURCE, recv_status.MPI_TAG, count);

//MPI_Wait(&send_handle, &send_status);