#include <stdio.h>
#include <stdlib.h>

#define W(i, j) W[i*n+j]

double *W;
int n;
void readMatrix(){
    int i, j;
    char file[60]= {"1.txt"};
    //scanf("%s", file);
    FILE *fp = fopen(file, "r");
    fscanf(fp, "%d", &n);
    W = (double *)malloc(sizeof(double)*n*n);
    for (i=0;i<n;i++){
        for (j=0;j<n;j++){
            fscanf(fp, "%lf", &W(j,i)); //翻转， 方便分配
        }
    }
    fclose(fp);
}

void floyd(){
    for (int k=0;k<n;k++){
        for (int i=0;i<n;i++){
            for (int j=0;j<n;j++){
                if (W(i,j) > W(i,k)+W(k,j)){
                    W(i,j) = W(i,k)+W(k,j);
                }
            }
        }
    }
}
int main(int argc, char *argv[]){
    readMatrix();
    floyd();

    for (int i=0;i<n;i++) printf("%d %lf\n", i, W(0, i));
    return 0;
}