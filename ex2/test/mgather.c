
#include "mpi.h" /*MPI head file*/

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv )
{

int myrank,size;

int A[20][20];
int buf[20][20][20];

MPI_Datatype EvenLine;

void *TempBuffer;

int i, j, k;

MPI_Status status; 

MPI_Init(&argc, &argv); /*initializing */
MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /*Process#*/
MPI_Comm_size(MPI_COMM_WORLD, &size); /*Total processes#*/


MPI_Type_vector(5, 5, 5, MPI_DOUBLE, &EvenLine);
MPI_Type_commit(&EvenLine);


for(i=0;i<5;i++) 
    for(j=0;j<5;j++)
        A[i][j] = myrank*25+i*5+j;
 
printf("rank=%d :", myrank);
for (i=0;i<5;i++) for (j=0;j<5;j++) printf("%d ",A[i][j]);
printf("\n");
MPI_Barrier(MPI_COMM_WORLD);

MPI_Gather(&A, 1, EvenLine, buf, 1, EvenLine, 0, MPI_COMM_WORLD);


if (myrank == 0){
    for (k=0;k<size;k++)
    for(i=0;i<5;i++)
    {
    for(j=0;j<5;j++)
        printf("%d ",buf[k][i][j]);  
    printf("\n");  
    }
}
MPI_Finalize(); /*quit from MPI world*/
return (0);
}

