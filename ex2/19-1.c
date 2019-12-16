#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define a(x,y) a[x*M+y]
#define b(x) b[x]
#define A(x,y) A[x*M+y]
#define B(x) B[x]
int M, N, m;
double *A, *B;
double startime, time1, time2;
int rank, nproc;
int l;
int p;
MPI_Status status;

int main(int argc, char **argv){
    int i, j, k, t, tem, w;
    FILE *fp;
    double *a, *b, *sum, *f, temp, *x;
    int *shift;
    int v, l;
    double lmax;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    p = nproc;
    if (rank == 0){
        startime=MPI_Wtime();
        fp = fopen("./19-1/dataIn.txt","r");
        fscanf(fp, "%d%d",&M, &N);
        //M == N - 1

        A = (double *)malloc(sizeof(double)*M*M);
        B = (double *)malloc(sizeof(double)*M);

        for (i=0;i<M;i++){
            for (j=0;j<M;j++){
                fscanf(fp, "%lf", A+M*i+j);
            }
            fscanf(fp, "%lf", B+i);
        }
        fclose(fp);
    }

    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    m = M/nproc;
    if (M%p!=0) m++;

    a = (double*)malloc(sizeof(double)*m*M);
    b = (double*)malloc(sizeof(double)*m);
    sum = (double*)malloc(sizeof(double)*m);
    f = (double*)malloc(sizeof(double)*(M+1));
    shift = (int*)malloc(sizeof(int)*M);
    x = (double*)malloc(sizeof(double)*M);

    for (i=0;i<M;i++) shift[i] = i;

    if (rank == 0){
        //自身
        for (i=0;i<m;i++) for (j=0;j<M;j++) a(i, j) = A(i*p, j);
        for (i=0;i<m;i++) b(i) = B(i*p);
        //其他
        for (i=0;i<M;i++) if (i%nproc != 0){
            MPI_Send(&(A(i, 0)), M, MPI_DOUBLE, 
                    i%nproc, i/nproc, MPI_COMM_WORLD);
            MPI_Send(&(B[i]), 1, MPI_DOUBLE,
                    i%nproc, i/nproc, MPI_COMM_WORLD);
        }
    }else{
        for (i=0;i<m;i++) if (i*nproc+rank < M){
            MPI_Recv(&a(i, 0), M, MPI_DOUBLE,
                    0, i, MPI_COMM_WORLD, &status);
            MPI_Recv(&b(i), 1, MPI_DOUBLE,
                    0, i, MPI_COMM_WORLD, &status);
        }
    }

    time1=MPI_Wtime();

    for (i=0;i<m;i++){
        for (j=0;j<p;j++){
            v= i*p+j;
            if (v>=M) break;
            if (rank==j){//主行元素
                lmax = a(i, v);
                l = v;

                for (k=v+1;k<M;k++){ //同行找
                    if (fabs(a(i, k)) > lmax){
                        lmax = a(i, k);
                        l = k;
                    }
                }
                if (l!=v){
                    for (t=0;t<m;t++){
                        temp = a(t, v);
                        a(t, v) = a(t, l);
                        a(t, l) = temp;
                    }
                    tem = shift[v];
                    shift[v] = shift[l];
                    shift[l] = tem;
                }

                for (k=v+1;k<M;k++) a(i, k) = a(i, k)/a(i, v);

                b(i)/= a(i, v);
                a(i, v)=1;
                for (k=v+1;k<M;k++) f[k] = a(i, k);
                f[M] = b(i);

                MPI_Bcast(f, M+1, MPI_DOUBLE, rank, MPI_COMM_WORLD);
                MPI_Bcast(&l, 1, MPI_INT, rank, MPI_COMM_WORLD);
            }
            else{
                MPI_Bcast(&f[0], M+1, MPI_DOUBLE, j, MPI_COMM_WORLD);
                MPI_Bcast(&l, 1, MPI_INT, j, MPI_COMM_WORLD);

                if (l!=v){
                    for (t=0;t<m;t++){
                        temp = a(t, v); a(t, v) = a(t, l); a(t, l) = temp;
                    }
                    tem = shift[v]; shift[v] = shift[l]; shift[l] = tem; 
                }
            }

            if (rank<=j){
                for (k=i+1;k<m;k++) if (k*nproc+rank<M){
                    for (w=v+1;w<M;w++){
                        a(k, w) = a(k, w) - f[w]*a(k, v);
                    }
                    b(k) = b(k) - f[M]*a(k, v);
                }
            }else{ // rank>j
                for (k=i;k<m;k++) if (k*nproc+rank<M){
                    for (w=v+1;w<M;w++){
                        a(k, w) = a(k, w) - f[w]*a(k, v);
                    }
                    b(k) = b(k) - f[M]*a(k, v);
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    //回代
    for (i=0;i<m;i++) sum[i] = 0.0;
    for (i=m-1;i>=0;i--){
        for (j=nproc-1;j>=0;j--){
            v = i*nproc+j;
            if (v>=M) continue;
            if (rank==j){
                x[v] = (b(i)-sum[i])/a(i, v);
                MPI_Bcast(&(x[i*nproc+j]), 1, MPI_DOUBLE, j, MPI_COMM_WORLD);

                for (k=0;k<i;k++)
                    sum[k] = sum[k]+a(k, v)*x[v];
            }else{
                MPI_Bcast(&(x[i*nproc+j]), 1, MPI_DOUBLE, j, MPI_COMM_WORLD);
                if (rank>j){
                    for (k=0;k<i;k++)
                        sum[k] = sum[k]+a(k, v)*x[v];
                }
                if (rank<j){
                    for (k=0;k<=i;k++)
                        sum[k] = sum[k]+a(k, v)*x[v];
                }
            }
            //printf("rank=%d: i=%d, %d %.2lf\n", rank, i, j, x[v]);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    if (rank!=0){
        for (i=0;i<m;i++)
            MPI_Send(&x[i*nproc+rank], 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
    }
    else{
        for (i=1;i<nproc;i++){
            for (j=0;j<m;j++){
                MPI_Recv(&x[j*nproc+i], 1, MPI_DOUBLE, i, j, MPI_COMM_WORLD, &status);
            }
        }
    }

    if (rank==0){
        for (k=0;k<M;k++){
            for (i=0;i<M;i++){
                if (shift[i]==k) printf("x[%d]=%lf\n", k, x[i]);
            }
        }
    }
    time2 = MPI_Wtime();
    if (rank==0){
        printf("\n");
        printf("Whole running time    = %f seconds\n",time2-startime);
        printf("Distribute data time  = %f seconds\n",time1-startime);
        printf("Parallel compute time = %f seconds\n",time2-time1);
    }
    MPI_Finalize();
    free(a);
    free(b);
    free(x);
    free(f);
    return 0;
}