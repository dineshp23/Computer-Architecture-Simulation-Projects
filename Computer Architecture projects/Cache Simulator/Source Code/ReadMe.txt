Steps to compile:
g++ cachesim.cpp -o cachesim

Steps to run: ./cachesim <inputFile> <cache size in Bytes> <block size in Bytes> <#ofWays>
1) ./cachesim gcc-10K.memtrace.txt 524288 16 4
2) for fully set associative, just add -1 in place of #ofWays
	./cachesim gcc-10K.memtrace.txt 524288 16 -1

*Extra Credit implemented.* 