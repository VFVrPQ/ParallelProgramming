#include <mpi.h>
#include <stdio.h>

typedef struct {
    double real, imag;
} Complex;

void myfunc(Complex *in, Complex *inout, int *len, MPI_Datatype *dttp){
    int i;
    Complex c;
    for (i=0;i<(*len);i++){
        c.real = in->real + inout->real;
        c.imag = in->imag + inout->imag;
        *inout = c;
        in++; inout++;
    }
    *inout = c;
}
int main(int argc, char *argv[]){
    int t, rank, nproc;
    int i, j, k;
    Complex A[100], ans[100];
    MPI_Datatype ctype;
    MPI_Op myOp;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    for (i=0;i<nproc;i++) A[i].real = rank, A[i].imag = 1;

    MPI_Type_contiguous(2, MPI_DOUBLE, &ctype);
    MPI_Type_commit(&ctype);

    
    MPI_Op_create(myfunc, 1, &myOp);
    
    MPI_Reduce(A, ans, nproc, ctype, myOp, 0, MPI_COMM_WORLD);
    printf("rank=%d\n", rank);

    //if (rank == 0){
        printf("%d %.5lf %.5lf\n", rank, ans[0].real, ans[0].imag);
    //}
    MPI_Finalize();
}