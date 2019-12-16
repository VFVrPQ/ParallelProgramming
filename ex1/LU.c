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
        //begin{initial}
        for (int i=1;i<=n;i++){
                for (int j=i;j<=n;j++){
                        if (i==j) L[i][j] = 1;
                        else L[i][j] = 0;
                }
        }
        for (int i=1;i<=n;i++){
                for (int j=1;j<i;j++){
                        U[i][j] = 0;
                }
        }
        //end{initial}
        for (int k=1;k<=n;k++){
                //U
                for (int j=k;j<=n;j++) U[k][j] = A[k][j];
                //L
                for (int i=k+1;i<=n;i++) L[i][k] = A[i][k]/U[k][k]; 
                //A
                for (int i=k+1;i<=n;i++){
                        for (int j=k+1;j<=n;j++){
                                A[i][j] -= L[i][k]*U[k][j];
                        }
                }
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