#include "mpi.h"
#include <stdio.h>
#include <math.h>
int main(int argc, char *argv[]){
    int i;
    int rank, nproc;
    int node_color, node_key;
    int head_color, head_key;
    const int root = 5;//暂时这么设定
    int ibuf;
    int tag=100;
    MPI_Comm HeadWorld, NodeWorld;
    MPI_Status status;

    int nblock = 3; // block = 3 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root) ibuf = 12345;
    else ibuf = 0;
    printf("Init: my rank = %d ibuf = %d\n", rank, ibuf);

    //MPI_Barrier(MPI_COMM_WORLD);// can move, for easy to see

    if (root != 0){// move ibuf to rank=0
            if (rank == root){
                    MPI_Send(&ibuf, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
            }else if (rank == 0){
                    MPI_Recv(&ibuf, 1, MPI_INT, root, tag, MPI_COMM_WORLD, &status);
            }
    }

    head_color = rank / 3; // < 3， 分一组
    head_key   = rank % 3;
    MPI_Comm_split(MPI_COMM_WORLD, head_color, head_key, &HeadWorld);
    MPI_Bcast(&ibuf, 1, MPI_INT, 0, HeadWorld);//%3， key = 0 broadcast
    printf("After HeadBcast: my rank = %d ibuf = %d\n", rank, ibuf);

    //MPI_Barrier(MPI_COMM_WORLD); // can move, for easy to see

    node_color = rank % 3;
    node_key   = rank / 3;
    MPI_Comm_split(MPI_COMM_WORLD, node_color, node_key, &NodeWorld);
    MPI_Bcast(&ibuf, 1, MPI_INT, 0, NodeWorld);// <3的广播，key=0
    printf("NodeBcast: my rank = %d ibuf = %d\n", rank, ibuf);
    MPI_Finalize();
    return 0;
}