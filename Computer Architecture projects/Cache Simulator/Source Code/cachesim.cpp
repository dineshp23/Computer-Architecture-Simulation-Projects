/*
* ACA Lab 3
* Cache Simulator
* Author: Dinesh Patil
*/

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <bitset>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <stdlib.h>

using namespace std;
long long hits, total, coldMisses, conflictMisses, capacityMisses, setsUsed, numberSets, cacheSize;
int blockSize, numberWays;
class setObject {
public:
	std::map<string, int> blockTable;
	int busyBlocks;
	int setFull;
};
std::map<string, setObject> setTable;
//AccessCache method takes the set and tag adresses as input and simulates the cache access operation and keeps count of the misses(cold, conflict and capacity)
void static AccessCache(string setAddress, string tagAddress)
{
	if (setTable.find(setAddress) == setTable.end())
	{
		setObject *dummyObject = new setObject();
		dummyObject->blockTable[tagAddress] = 0;
		dummyObject->busyBlocks = 1;
		setTable[setAddress] = *dummyObject;
		coldMisses++;
	}
	else
	{
		setObject objSet = setTable[setAddress];
		setObject *ptrSet = &objSet;
		if (ptrSet->blockTable.find(tagAddress) == ptrSet->blockTable.end())
		{
			if (ptrSet->busyBlocks < numberWays)
			{
				typedef std::map<string, int>::iterator it_type;
				for (it_type iterator = ptrSet->blockTable.begin(); iterator != ptrSet->blockTable.end(); iterator++)
				{
					iterator->second = iterator->second + 1;
				}
				ptrSet->blockTable[tagAddress] = 0;
				ptrSet->busyBlocks = ptrSet->busyBlocks + 1;
				coldMisses++;
			}
			else
			{
				if (ptrSet->setFull == 0)
				{
					ptrSet->setFull = 1;
					setsUsed++;
				}
				string removalKey;
				typedef std::map<string, int>::iterator it_type;
				for (it_type iterator = ptrSet->blockTable.begin(); iterator != ptrSet->blockTable.end(); iterator++)
				{
					if (iterator->second == numberWays - 1)
						removalKey = iterator->first;
					iterator->second = iterator->second + 1;
				}
				ptrSet->blockTable.erase(removalKey);
				ptrSet->blockTable[tagAddress] = 0;
				conflictMisses++;
				if (setsUsed >= numberSets)
				{
					capacityMisses++;
					conflictMisses--;
				}
			}


		}
		else
		{
			hits++;
			int currentTagLevel = ptrSet->blockTable[tagAddress];
			typedef std::map<string, int>::iterator it_type;
			for (it_type iterator = ptrSet->blockTable.begin(); iterator != ptrSet->blockTable.end(); iterator++)
			{
				if (iterator->second < currentTagLevel)
				{
					iterator->second = iterator->second + 1;
				}
			}
			ptrSet->blockTable[tagAddress] = 0;
		}
		setTable[setAddress] = *ptrSet;
	}
	total++;
}
int main(int argc, char *argv[]) {
	hits = 0;
	total = 0;
	coldMisses = 0;
	conflictMisses = 0;
	if (argc != 5) {
		cerr << "ERROR: Please provide a file name for input trace file, cache size in Bytes, block size in Bytes and number of ways (1 for direct mapped cahe) \n";
		return 0;
	}

	ifstream fileIn(argv[1]);
	if (!fileIn.is_open()) {
		cerr << "ERROR: The input file does not exist!\n";
		return 1;
	}
	cacheSize = atol(argv[2]);
	blockSize = atoi(argv[3]);
	numberWays = atoi(argv[4]);
	if (numberWays == -1)
	{
		numberWays = ceil((double)cacheSize / (blockSize));
		cout << "cache size= : " << cacheSize << "  ,  " << "block size= : " << blockSize << "  ,  " << "number of ways= : ( Fully Set associative )" << numberWays << endl;
	}
	else
	{
		cout << "cache size= : " << cacheSize << "  ,  " << "block size= : " << blockSize << "  ,  " << "number of ways= : " << numberWays << endl;
	}

	long long list_array[10];
	string pc_addr[10];
	char branch_status[10];
	long long index = 0;
	long long offset;
	char instructionType;

	int tagBits, indexBits, offsetBits;
	numberSets = ceil((double)cacheSize / (blockSize*numberWays));
	indexBits = log(numberSets) / log(2);
	offsetBits = log(blockSize) / log(2);
	tagBits = 32 - indexBits - offsetBits;
	cout << "number of sets= : " << numberSets << "  ,  " << "number of tag bits= : " << tagBits << "  ,  " << "number of index bits= : " << indexBits << "  ,  " << "number of offset bits= : " << offsetBits << endl;
	cout << "Calculating Cache hit and miss rate ..." << endl;
	while (!fileIn.eof())
	{

		string currentAddress, tagAddress, setAddress, offsetAddress;

		fileIn >> instructionType;
		if (fileIn.fail()) break;

		fileIn >> dec >> offset;
		if (fileIn.fail()) break;

		long long address;
		fileIn >> hex >> address;
		if (fileIn.fail()) break;

		fileIn.ignore(256, '\n');

		std::string binary = std::bitset<44>(address).to_string();
		currentAddress = binary.substr(binary.length() - 32);

		tagAddress = currentAddress.substr(0, tagBits);
		setAddress = currentAddress.substr(tagBits, indexBits);

		AccessCache(setAddress, tagAddress);


	}

	cout << "Cache Hits: " << hits << "  ,  " << "Cache Miss: " << total - hits << endl;
	cout << "Total: " << total << endl;
	cout << "Cold Misses: " << coldMisses << "  ,  " << "Conflict Misses: " << conflictMisses << "  ,  " << "Capacity Misses: " << capacityMisses << endl;
	cout << "Cache Hit rate: " << ((double)(hits) / total) * 100 << " %" << "  ,  " << "Cache Miss rate: " << ((double)(total - hits) / total) * 100 << " %" << endl;
	cout << "=========================================================================" << endl;

	return 0;
}


