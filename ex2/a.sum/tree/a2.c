    #include <stdio.h>
    #include <unistd.h>
    #include "mpi.h"

    int main(int argc, char *argv[]){
        int i, j;
        int rank, nproc;
        int tag=100;
        int ibuf, recv, send, count;
        MPI_Request recv_handle, send_handle;
        MPI_Status recv_status, send_status;
        double start_time, end_time;
        char hostname[60];

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);

        gethostname(hostname, sizeof(hostname));

        start_time = MPI_Wtime();
        ibuf = rank;
        for (i=1; i < nproc; i<<=1){ // logN
            if ((rank&(i-1)) == 0){ //rank是i的倍数
                if ((rank&i) == 0){ //rank是2i的倍数
                    MPI_Irecv(&recv, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &recv_handle);
                    MPI_Wait(&recv_handle, &recv_status);
                    MPI_Get_count(&recv_status, MPI_INT, &count);
                    ibuf += recv;
                }else{
                    send = ibuf;
                    MPI_Isend(&send, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &send_handle);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        printf("before broadcast : hostname=%s, proc=%d, sum=%d\n", hostname, rank, ibuf);
        MPI_Barrier(MPI_COMM_WORLD); // no need
        
        for (i=nproc/2; i >= 1; i>>=1){ // logN
            if ((rank&(i-1)) == 0){ // rank是i
                if ((rank&i) == 0){
                    send = ibuf;
                    MPI_Isend(&send, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &send_handle);    
                }else{
                    MPI_Irecv(&recv, 1, MPI_INT, rank^i, tag, MPI_COMM_WORLD, &recv_handle);
                    MPI_Wait(&recv_handle, &recv_status);
                    MPI_Get_count(&recv_status, MPI_INT, &count);
                    ibuf = recv;
                    //printf("proc=%d, source=%d, count=%d, recv=%d\n", rank, recv_status.MPI_SOURCE, count, recv);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        end_time = MPI_Wtime();
        printf("broadcast : hostname=%s, proc=%d, sum=%d, time=%.5lf\n", hostname, rank, ibuf, end_time-start_time);
        MPI_Finalize();
        return 0;
    }