#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MAX_N 50
#define PI    3.1415926535897932
#define EPS   10E-8
#define V_TAG 99
#define P_TAG 100
#define Q_TAG 101
#define R_TAG 102
#define S_TAG 103
#define S_TAG2 104

typedef enum {FALSE, TRUE}
BOOL;

typedef struct
{
    double r;
    double i;
} complex_t;

complex_t p[MAX_N], q[MAX_N], s[2*MAX_N], r[2*MAX_N];
complex_t w[2*MAX_N];
int variableNum;
double transTime = 0, totalTime = 0, beginTime;
MPI_Status status;

void comp_add(complex_t* result, const complex_t* c1, const complex_t* c2)
{
    result->r = c1->r + c2->r;
    result->i = c1->i + c2->i;
}


void comp_multiply(complex_t* result, const complex_t* c1, const complex_t* c2)
{
    result->r = c1->r * c2->r - c1->i * c2->i;
    result->i = c1->r * c2->i + c2->r * c1->i;
}


/*
 * Function:    shuffle
 * Description: 移动f中从beginPos到endPos位置的元素，使之按位置奇偶
 *              重新排列。举例说明:假设数组f，beginPos=2, endPos=5
 *              则shuffle函数的运行结果为f[2..5]重新排列，排列后各个
 *              位置对应的原f的元素为: f[2],f[4],f[3],f[5]
 * Parameters:  f为被操作数组首地址
 *              beginPos, endPos为操作的下标范围
 */
void shuffle(complex_t* f, int beginPos, int endPos)
{
    int i;
    complex_t temp[2*MAX_N];

    for(i = beginPos; i <= endPos; i ++)
    {
        temp[i] = f[i];
    }

    int j = beginPos;
    for(i = beginPos; i <= endPos; i +=2)
    {
        f[j] = temp[i];
        j++;
    }
    for(i = beginPos +1; i <= endPos; i += 2)
    {
        f[j] = temp[i];
        j++;
    }
}


/*
 * Function:		evaluate
 * Description:	对复数序列f进行FFT或者IFFT(由x决定)，结果序列为y，
 * 			产生leftPos 到 rightPos之间的结果元素
 * Parameters:	f : 原始序列数组首地址
 * 			beginPos : 原始序列在数组f中的第一个下标
 * 			endPos : 原始序列在数组f中的最后一个下标
 * 			x : 存放单位根的数组，其元素为w,w^2,w^3...
 * 			y : 输出序列
 * 			leftPos : 所负责计算输出的y的片断的起始下标
 * 			rightPos : 所负责计算输出的y的片断的终止下标
 * 			totalLength : y的长度
 */
void evaluate(complex_t* f, int beginPos, int endPos,
const complex_t* x, complex_t* y,
int leftPos, int rightPos, int totalLength)
{
    int i;
    if ((beginPos > endPos)||(leftPos > rightPos))
    {
        printf("Error in use Polynomial!\n");
        exit(-1);
    }
    else if(beginPos == endPos)
    {
        for(i = leftPos; i <= rightPos; i ++)
        {
            y[i] = f[beginPos];
        }
    }
    else if(beginPos + 1 == endPos)
    {
        for(i = leftPos; i <= rightPos; i ++)
        {
            complex_t temp;
            comp_multiply(&temp, &f[endPos], &x[i]);
            comp_add(&y[i], &f[beginPos], &temp);
        }
    }
    else
    {
        complex_t tempX[2*MAX_N],tempY1[2*MAX_N], tempY2[2*MAX_N];
        int midPos = (beginPos + endPos)/2;

        shuffle(f, beginPos, endPos);

        for(i = leftPos; i <= rightPos; i ++)
        {
            comp_multiply(&tempX[i], &x[i], &x[i]);
        }

        evaluate(f, beginPos, midPos, tempX, tempY1,
            leftPos, rightPos, totalLength);
        evaluate(f, midPos+1, endPos, tempX, tempY2,
            leftPos, rightPos, totalLength);

        for(i = leftPos; i <= rightPos; i ++)
        {
            complex_t temp;
            comp_multiply(&temp, &x[i], &tempY2[i]);
            comp_add(&y[i], &tempY1[i], &temp);
        }
    }
}


/*
 * Function:    print
 * Description: 打印数组元素的实部
 * Parameters:  f为待打印数组的首地址
 *              fLength为数组的长度
 */
void print(const complex_t* f, int fLength)
{
    BOOL isPrint = FALSE;
    int i;

    /* f[0] */
    if (abs(f[0].r) > EPS)
    {
        printf("%f", f[0].r);
        isPrint = TRUE;
    }

    for(i = 1; i < fLength; i ++)
    {
        if (f[i].r > EPS)
        {
            if (isPrint)
                printf(" + ");
            else
                isPrint = TRUE;
            printf("%ft^%d", f[i].r, i);
        }
        else if (f[i].r < - EPS)
        {
            if(isPrint)
                printf(" - ");
            else
                isPrint = TRUE;
            printf("%ft^%d", -f[i].r, i);
        }
    }
    if (isPrint == FALSE)
        printf("0");
    printf("\n");
}


/*
 * Function:    myprint
 * Description: 完整打印复数数组元素，包括实部和虚部
 * Parameters:  f为待打印数组的首地址
 *              fLength为数组的长度
 */
void myprint(const complex_t* f, int fLength)
{
    int i;
    for(i=0;i<fLength;i++)
    {
        printf("%f+%fi , ", f[i].r, f[i].i);
    }
    printf("\n");
}


/*
 * Function:   addTransTime
 * Description:累计发送数据所耗费的时间
 * Parameters: toAdd为累加的时间
 */
void addTransTime(double toAdd)
{
    transTime += toAdd;
}


/*
 * Function:    readFromFile
 * Description:	从dataIn.txt读取数据
 */
BOOL readFromFile()
{
    int i;
    FILE* fin = fopen("dataIn.txt", "r");

    if (fin == NULL)
    {
        printf("Cannot find input data file\n"
            "Please create a file \"dataIn.txt\"\n"
            "2\n"
            "1.0  2\n"
            "2.0  -1\n"
            );
        return(FALSE);
    }

    fscanf(fin, "%d\n", &variableNum);
    if ((variableNum < 1)||(variableNum > MAX_N))
    {
        printf("variableNum out of range!\n");
        return(FALSE);
    }

    for(i = 0; i < variableNum; i ++)
    {
        fscanf(fin, "%lf", &p[i].r);
        p[i].i = 0.0;
    }

    for(i = 0; i < variableNum; i ++)
    {
        fscanf(fin, "%lf", &q[i].r);
        q[i].i = 0.0;
    }

    fclose(fin);

    printf("Read from data file \"dataIn.txt\"\n");
    printf("p(t) = ");
    print(p, variableNum);
    printf("q(t) = ");
    print(q, variableNum);

    return(TRUE);
}


/*
 * Function:    sendOrigData
 * Description: 把原始数据发送给其它进程
 * Parameters:  size为集群中进程的数目
 */
void sendOrigData(int size)
{
    int i;
    for(i = 1; i < size; i ++)
    {
        MPI_Send(&variableNum, 1, MPI_INT, i, V_TAG, MPI_COMM_WORLD);
        MPI_Send(p, variableNum * 2, MPI_DOUBLE, i, P_TAG, MPI_COMM_WORLD);
        MPI_Send(q, variableNum * 2, MPI_DOUBLE, i, Q_TAG, MPI_COMM_WORLD);
    }
}


/*
 * Function:    recvOrigData
 * Description:	接受原始数据
 */
void recvOrigData()
{
    MPI_Recv(&variableNum, 1, MPI_INT, 0, V_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(p, variableNum * 2, MPI_DOUBLE, 0, P_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(q, variableNum * 2, MPI_DOUBLE, 0, Q_TAG,MPI_COMM_WORLD, &status);
}


int main(int argc, char *argv[])
{
    int rank,size, i;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(rank == 0)
    {
        /* 0# 进程从文件dataIn.txt读入多项式p,q的阶数和系数序列 */
        if(!readFromFile())
            exit(-1);

        /* 进程数目太多，造成每个进程平均分配不到一个元素，异常退出 */
        if(size > 2*variableNum)
        {
            printf("Too many Processors , reduce your -np value\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        beginTime = MPI_Wtime();

        /* 0#进程把多项式的阶数、p、q发送给其它进程 */
        sendOrigData(size);

        /* 累计传输时间 */
        addTransTime(MPI_Wtime() - beginTime);
    }
    else                                          /* 其它进程接收进程0发送来的数据，包括variableNum、数组p和q */
    {
        recvOrigData();
    }
    /* 初始化数组w，用于进行傅立叶变换 */
    int wLength = 2*variableNum;
    for(i = 0; i < wLength; i ++)
    {
        w[i].r = cos(i*2*PI/wLength);
        w[i].i = sin(i*2*PI/wLength);
    }

    /* 划分各个进程的工作范围 startPos ~ stopPos */
    int everageLength = wLength / size;
    int moreLength = wLength % size;
    int startPos = moreLength + rank * everageLength;
    int stopPos  = startPos + everageLength - 1;

    if(rank == 0)
    {
        startPos = 0;
        stopPos  = moreLength+everageLength - 1;
    }

    /* 对p作FFT，输出序列为s，每个进程仅负责计算出序列中 */
    /* 位置为startPos 到 stopPos的元素 */
    evaluate(p, 0, variableNum - 1, w, s, startPos, stopPos, wLength);

    /* 对q作FFT，输出序列为r，每个进程仅负责计算出序列中 */
    /* 位置为startPos 到 stopPos的元素 */
    evaluate(q, 0, variableNum - 1, w, r, startPos, stopPos, wLength);

    /* s和r作点积，结果保存在s中，同样，每个进程只计算自己范围内的部分 */
    for(i = startPos; i <= stopPos ; i ++)
    {
        complex_t temp;
        comp_multiply(&temp, &s[i], &r[i]);
        s[i] = temp;
        s[i].r /= wLength * 1.0;
        s[i].i /= wLength * 1.0;
    }

    /* 各个进程都把s中自己负责计算出来的部分发送给进程0，并从进程0接收汇总的s */
    if (rank > 0)
    {
        MPI_Send(s + startPos, everageLength * 2, MPI_DOUBLE, 0, S_TAG, MPI_COMM_WORLD);
        MPI_Recv(s, wLength * 2, MPI_DOUBLE, 0, S_TAG2, MPI_COMM_WORLD, &status);
    }
    else
    {
        /* 进程0接收s片断，向其余进程发送完整的s */
        double tempTime = MPI_Wtime();

        for(i = 1; i < size; i ++)
        {
            MPI_Recv(s + moreLength + i * everageLength, everageLength * 2,
                MPI_DOUBLE, i, S_TAG, MPI_COMM_WORLD,&status);
        }

        for(i = 1; i < size; i ++)
        {
            MPI_Send(s, wLength * 2,
                MPI_DOUBLE, i,
                S_TAG2, MPI_COMM_WORLD);
        }

        addTransTime(MPI_Wtime() - tempTime);
    }

    /* swap(w[i],w[(wLength-i)%wLength]) */
    /* 重新设置w，用于作逆傅立叶变换 */
    complex_t temp;
    for(i = 1; i < wLength/2; i ++)
    {
        temp = w[i];
        w[i] = w[wLength - i];
        w[wLength - i] = temp;
    }

    /* 各个进程对s作逆FFT，输出到r的相应部分 */
    evaluate(s, 0, wLength - 1, w, r, startPos, stopPos, wLength);

    /* 各进程把自己负责的部分的r的片断发送到进程0 */
    if (rank > 0)
    {
        MPI_Send(r + startPos, everageLength * 2, MPI_DOUBLE,
            0,R_TAG, MPI_COMM_WORLD);
    }
    else
    {
        /* 进程0接收各个片断得到完整的r，此时r就是两多项式p,q相乘的结果多项式了 */
        double tempTime = MPI_Wtime();

        for(i = 1; i < size; i ++)
        {
            MPI_Recv((r+moreLength+i*everageLength), everageLength * 2,
                MPI_DOUBLE, i, R_TAG, MPI_COMM_WORLD, &status);
        }

        totalTime = MPI_Wtime();
        addTransTime(totalTime - tempTime);
        totalTime -= beginTime;

        /* 输出结果信息以及时间统计信息 */
        printf("\nAfter FFT r(t)=p(t)q(t)\n");
        printf("r(t) = ");
        print(r, wLength - 1);
        printf("\nUse prossor size = %d\n", size);
        printf("Total running time = %f(s)\n", totalTime);
        printf("Distribute data time = %f(s)\n", transTime);
        printf("Parallel compute time = %f(s)\n", totalTime - transTime);
    }
    MPI_Finalize();
}
