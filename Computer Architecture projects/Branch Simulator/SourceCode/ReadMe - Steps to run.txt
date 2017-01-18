1.
To Compile:
g++ branchsim.cpp -o branchsim

To Run:
./branchsim gcc-10K.txt 4 2
(Format is ./branchsim tracefilename m n)

Extra Credit:
./branchsim gcc-10K.txt 4 2 16
(Format is ./branchsim tracefilename m n k    ...  where k is the last k bits of the PC used)

Sample Output:
m,n: 4,1
k= 6( Last k bits of PC address used )
Misprediction rate: 24.0015 %
===============

2. To find number of entries utilised, please follow above steps but with branchsimWithEntries.cpp as the source file.