#include <stdio.h>

const int N = 100+10;
double A[N][N], L[N][N], U[N][N];
int main(){
        freopen("4.txt","r",stdin);
        int n = 3;
        for (int i=1;i<=n;i++){
                for (int j=1;j<=n;j++){
                        scanf("%lf", &A[i][j]);
                }
        }
        for (int k=1;k<=n;k++){
                for (int i=k+1;i<=n;i++) A[i][k] /= A[k][k]; 
                for (int i=k+1;i<=n;i++){
                        for (int j=k+1;j<=n;j++){
                                A[i][j] -= A[i][k]*A[k][j];
                        }
                }
        }
        for (int i=1;i<=n;i++){
                for (int j=1;j<=n;j++){
                        if (i<j) U[i][j] = A[i][j];
                        else     L[i][j] = A[i][j];
                }
                L[i][i] = 1;
        }
        for (int i=1;i<=n;i++) for (int j=1;j<=n; j++){
                printf("%.2lf ", L[i][j]);
                if (j==n) printf("\n");
        }
        printf("\n");
        for (int i=1;i<=n;i++) for (int j=1;j<=n;j++){
                printf("%.2lf ", U[i][j]);
                if (j==n) printf("\n");
        }
        return 0;
}