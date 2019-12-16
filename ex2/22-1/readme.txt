1. compile:
mpicc fft.c -o fft -lm

2. run:
mpirun -np 4 fft

3. result:
Read from data file "dataIn.txt"
p(t) = 1.000000 + 3.000000t^1 + 3.000000t^2 + 1.000000t^3
q(t) = 1.000000t^1 + 2.000000t^2 + 1.000000t^3

After FFT r(t)=p(t)q(t)
r(t) = 1.000000t^1 + 5.000000t^2 + 10.000000t^3 + 10.000000t^4 + 5.000000t^5 + 1.000000t^6

Use prossor size = 4
Total running time = 0.016373(s)
Distribute data time = 0.006959(s)
Parallel compute time = 0.009414(s)
