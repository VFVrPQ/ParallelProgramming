1. compile:
mpicc gauss.c -o gauss

2. run:
mpirun -np 4 gauss

3. result:
Input of file "dataIn.txt"
4       5
1.000000        4.000000        -2.000000       3.000000        6.000000
2.000000        2.000000        0.000000        4.000000        2.000000
3.000000        0.000000        -1.000000       2.000000        1.000000
1.000000        2.000000        2.000000        -3.000000       8.000000

Output of solution
x[0]=1.000000
x[1]=2.000000
x[2]=0.000000
x[3]=-1.000000

Whole running time    = 0.011314 seconds
Distribute data time  = 0.002518 seconds
Parallel compute time = 0.008796 seconds
