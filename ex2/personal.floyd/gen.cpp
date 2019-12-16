#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define a(i, j) a[i*n+j]
int main(){
    FILE *fp = fopen("1.txt","w");
    srand(time(NULL));
    int n = 1000;
    int *a = (int*)malloc(sizeof(int)*n*n);
    fprintf(fp, "%d\n", n);
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            if (i==j) fprintf(fp, "0 ");
            else if (i<j){
                a(i, j) = rand() % n;
                fprintf(fp, "%d ",a(i, j));
            }else{
                fprintf(fp, "%d ", a(j, i));
            }
            if (j==n-1) fprintf(fp, "\n");
        }
    }
    fprintf(fp, "0\n");//
    free(a);
    fclose(fp);
    return 0;
}