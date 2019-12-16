#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define INF 1e9+7
int n, S;
double *W;
double *dist;
int *vis;
#define W(i, j) W[i*n+j]

int nproc, rank;
int each, mynum;//每个处理器分到的节点
void readMatrix(){
    int i, j;
    MPI_Status status;
    if (rank == 0){
        char file[60]= {"./15-3/my.txt"};
        //scanf("%s", file);
        FILE *fp = fopen(file, "r");
        fscanf(fp, "%d", &n);
        W = (double *)malloc(sizeof(double)*n*n);
        for (i=0;i<n;i++){
            for (j=0;j<n;j++){
                fscanf(fp, "%lf", &W(j,i)); //翻转， 方便分配
            }
        }
        fscanf(fp, "%d", &S); //start node
        fclose(fp);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // n总点数
    MPI_Bcast(&S, 1, MPI_INT, 0, MPI_COMM_WORLD); // S开始点

    //决定每个处理器平均分配到的节点数
    each = n / (nproc-1);
    if (n % (nproc-1) != 0) each++;
    // 0 ~ each-1 : 1
    
    //每个处理器分配到的节点数
    mynum = each;
    if (rank == nproc-1) mynum = n - (nproc-2) * each;
    else if (rank == 0) mynum = 0;

    //printf("rank=%d, mynum=%d\n", rank, mynum);
    if (rank == 0){
        //printf("&W(0,0)=%x, &W(50, 0)=%x\n", &(W(0, 0)), &(W(50, 0)));
        for (i=1;i<nproc;i++){
            if (i!=nproc-1)
                MPI_Send(&(W((i-1)*each, 0)), each*n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
            else
                MPI_Send(&(W((i-1)*each, 0)), (n-(nproc-2)*each)*n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
        }
    }else{
        W = (double*)malloc(sizeof(double)*mynum*n);
        MPI_Recv(W, mynum*n, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD, &status); // no &!

        /*printf("rank = %d :", rank);
        for (i=0;i<mynum*n && i<10;i++) printf("%.2lf ", W[i]);
        printf("\n");*/
        dist = (double*)malloc(sizeof(double)*mynum);
        vis = (int*)malloc(sizeof(int)*mynum);
        for (i=0;i<mynum;i++){
            if ((rank-1)*each+i == S){
                dist[i] = 0;
            }else{
                dist[i] = INF;
            }
            vis[i] = 0;
        }
    }
}
void FindMinWay(){
    MPI_Status status;
    int i, j;
    for (i=0;i<n;i++){
        //从每个进程里选出最小的
        double mx = INF;
        int id = -1, real_id = -1;
        if (rank != 0){//rank=0只负责输入
            for (j=0;j<mynum;j++){
                if (!vis[j] && (id==-1 || dist[id]>dist[j])) id=j;
            }
            if (id != -1){
                mx = dist[id];
                real_id = (rank-1)*each+id;
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        //printf("i=%d proc=%d: mx=%.2lf real_id=%d\n", i, rank, mx, real_id);
        for (j=nproc-1;j>1;j=(j+1)/2){ // [1,j]
            if (rank != 0 && (!(j%2==1 && rank == (j+1)/2))){//j是奇数, 中间的数不动
                // for barrier
                if (rank>(j+1)/2){
                    MPI_Send(&mx, 1, MPI_DOUBLE, rank-(j+1)/2, 100, MPI_COMM_WORLD);
                    MPI_Send(&real_id, 1, MPI_INT, rank-(j+1)/2, 101, MPI_COMM_WORLD);
                }else{
                    double mx_temp;
                    int real_id_temp;
                    MPI_Recv(&mx_temp, 1, MPI_DOUBLE, rank+(j+1)/2, 100, MPI_COMM_WORLD, &status);
                    MPI_Recv(&real_id_temp, 1, MPI_INT, rank+(j+1)/2, 101, MPI_COMM_WORLD, &status);
                    if (real_id_temp != -1 && mx_temp < mx){
                        mx = mx_temp;
                        real_id = real_id_temp;
                    }
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        
        MPI_Bcast(&mx, 1, MPI_DOUBLE, 1, MPI_COMM_WORLD);
        MPI_Bcast(&real_id, 1, MPI_INT, 1, MPI_COMM_WORLD);
        if (rank != 0){
            for (j=0;j<mynum;j++){
                if (dist[j] > mx + W(j, real_id)){ // W已经转置了
                    dist[j] = mx + W(j, real_id);
                }
            }
            if ((rank-1)*each <= real_id && real_id < rank*each){ // 如果最小值在当前的进程里面
                vis[real_id % each] = 1;
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //聚集到rank=0
    if (rank != 0){
        for (j=0;j<mynum;j++){
            printf("node[%d]=%.2lf\n", (rank-1)*each+j, dist[j]);
        }
        free(vis);
        free(dist);
    }
}
int main(int argc, char *argv[]){
    double starttime, endtime;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //处理器0读入边权邻接矩阵W
    //并将矩阵按列分发给其他处理器
    
    starttime = MPI_Wtime();
    readMatrix();
    //printf("S=%d\n", S);
    FindMinWay();
    free(W);
    endtime = MPI_Wtime();
    if (rank==0){
        printf("time = %.2lf\n", endtime-starttime);
    }
    MPI_Finalize();
    return 0;
}