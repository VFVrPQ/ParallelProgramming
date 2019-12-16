#include <stdio.h>
#include <math.h>
#include <stdlib.h>


int malloc2dLL(long long ***array, int n, int m){
    long long *p = (long long*)malloc(n*m*sizeof(long long));
    if (!p) return -1;

    for (int i=0;i<n;i++){
        (*array)[i] = &(p[i*m]);
    }
    return 0;
}
int free2dLL(long long ***array, int n, int m){
    free((*array)[0]);
    return 0;
}

int main(){
    long long **A;
    malloc2dLL(&A, 4, 4);
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            A[i][j]=i*4+j;
            printf("%lld ", A[i][j]);
            if (j==3) printf("\n");
        }
    }
    free2dLL(&A, 4, 4);
    return 0;
}