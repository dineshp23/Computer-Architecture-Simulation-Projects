//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <bitset>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;
std::map<string, int> ght1, ght2;
std::map<string, int> entriesUsed;
long long hits, total;
std::vector<string> list_pcaddr;// = new std::list<string>();
std::vector<char> branch_history;
class historyObject {
public:
	std::map<string, int> bhTable;
};
std::map<string, historyObject> ghTable;

void branchPredictor_1(string pcAddress, int branchStatus)
{
	int predictedBranchStatus, previousBranchStatus;
	if (ght1.find(pcAddress) == ght1.end())
	{

		if (branchStatus == 0)
		{
			hits++;
		}
		ght1[pcAddress] = branchStatus;
	}
	else
	{
		previousBranchStatus = ght1[pcAddress];
		if (previousBranchStatus == branchStatus)
		{
			hits++;
		}
		else
		{
			ght1[pcAddress] = branchStatus;
		}
	}
	total++;
}

void branchPredictor_2(string pcAddress, int branchStatus)
{
	int predictedBranchStatus, previousBranchStatus;

	if (ght2.find(pcAddress) == ght2.end())
	{

		if (branchStatus == 0)
		{
			hits++;
			ght2[pcAddress] = -1;
		}
		else
		{
			ght2[pcAddress] = 2;
		}
	}
	else
	{
		previousBranchStatus = ght2[pcAddress];
		if (branchStatus == 1)
		{
			if (previousBranchStatus == 2)
			{
				hits++;
			}
			else if (previousBranchStatus == 1)
			{
				hits++;
				ght2[pcAddress] = 2;
			}
			else if (previousBranchStatus == 0)
			{
				ght2[pcAddress] = 2;
			}
			else if (previousBranchStatus == -1)
			{
				ght2[pcAddress] = 0;
			}
		}
		else if (branchStatus == 0)
		{
			if (previousBranchStatus == 2)
			{
				ght2[pcAddress] = 1;
			}
			else if (previousBranchStatus == 1)
			{
				ght2[pcAddress] = -1;
			}
			else if (previousBranchStatus == 0)
			{
				ght2[pcAddress] = -1;
				hits++;
			}
			else if (previousBranchStatus == -1)
			{
				hits++;
			}
		}
	}
	total++;
}

void mnBranchPredictor(string pcAddress, int branchStatus, string historyM, int n)
{
	int predictedBranchStatus, previousBranchStatus;
	/*string historyM;
	if (m>0)
	{
	for (int i = m; i > 0; i--)
	{
	string iStatus;
	if (index<m)
	{
	iStatus = "0";
	}
	else if (branch_history[index - i] == 'T')
	{
	iStatus = "1";
	}
	else
	{
	iStatus = "0";
	}
	historyM = historyM + iStatus;
	}
	}*/
	if (n == 1)
	{
		if (ghTable.find(pcAddress) == ghTable.end())
		{
			if (branchStatus == 0)
			{
				hits++;
				historyObject *dummyObject = new historyObject();
				dummyObject->bhTable[historyM] = 0;
				ghTable[pcAddress] = *dummyObject;
			}
			else
			{
				historyObject *dummyObject = new historyObject();
				dummyObject->bhTable[historyM] = 1;
				ghTable[pcAddress] = *dummyObject;
			}
		}
		else
		{
			//if (pcAddress == "001011")
			//{
			//	int trial = 0; // n=1
			//}
			historyObject objHistory = ghTable[pcAddress];
			historyObject *ptrHistory = &objHistory;
			previousBranchStatus = ptrHistory->bhTable[historyM];
			if (previousBranchStatus == branchStatus)
			{
				hits++;
			}
			ptrHistory->bhTable[historyM] = branchStatus;
			ghTable[pcAddress] = *ptrHistory;
		}
	}
	if (n == 2)
	{
		if (ghTable.find(pcAddress) == ghTable.end())
		{
			if (branchStatus == 0)
			{
				hits++;
				historyObject *dummyObject = new historyObject();
				dummyObject->bhTable[historyM] = -1;
				ghTable[pcAddress] = *dummyObject;
			}
			else
			{
				historyObject *dummyObject = new historyObject();
				dummyObject->bhTable[historyM] = 0;
				ghTable[pcAddress] = *dummyObject;
			}
		}
		else
		{
			//if (pcAddress == "001011")
			//{
			//	int trial = 0; //n=2
			//}
			historyObject objHistory = ghTable[pcAddress];
			historyObject *ptrHistory = &objHistory;
			if (ptrHistory->bhTable.find(historyM) == ptrHistory->bhTable.end())
			{
				previousBranchStatus = -1;
			}
			else
			{
				previousBranchStatus = ptrHistory->bhTable[historyM];
			}
			//previousBranchStatus = ptrHistory->bhTable[historyM];
			int updatedBranchStatus;
			if (branchStatus == 1)
			{
				if (previousBranchStatus == 2)
				{
					hits++;
					updatedBranchStatus = 2;
				}
				else if (previousBranchStatus == 1)
				{
					hits++;
					updatedBranchStatus = 2;
				}
				else if (previousBranchStatus == 0)
				{
					updatedBranchStatus = 2;
				}
				else if (previousBranchStatus == -1)
				{
					updatedBranchStatus = 0;
				}
			}
			else if (branchStatus == 0)
			{
				if (previousBranchStatus == 2)
				{
					updatedBranchStatus = 1;
				}
				else if (previousBranchStatus == 1)
				{
					updatedBranchStatus = -1;
				}
				else if (previousBranchStatus == 0)
				{
					updatedBranchStatus = -1;
					hits++;
				}
				else if (previousBranchStatus == -1)
				{
					hits++;
					updatedBranchStatus = -1;
				}
			}
			ptrHistory->bhTable[historyM] = updatedBranchStatus;
			ghTable[pcAddress] = *ptrHistory;
		}
	}
	total++;
}
int main(int argc, char *argv[]) {
	hits = 0;
	total = 0;
	//ifstream fileIn(argv[1]);
	if (argc != 4 && (argc > 5 || argc < 4)) {
		cerr << "ERROR: Please provide a file name for input trace file, m, n and k last bits of the PC to be use \n";
		return 0;
	}

	ifstream fileIn(argv[1]);
	if (!fileIn.is_open()) {
		cerr << "ERROR: The input file does not exist!\n";
		return 1;
	}
	//ifstream fileIn("gcc-10K.txt");
	/*if (!fileIn.is_open()) {
	cerr << "ERROR: The input file does not exist!\n";
	return 1;
	}*/
	//ofstream fileOut("Output10K.txt", std::ios_base::app);
	int m = atoi(argv[2]);
	int n = atoi(argv[3]);
	cout << "m,n: " << m << "," << n << endl;
	//m = 4;
	//n = 2;
	int k = 6;
	if (argv[4] != NULL)
	{
		k = atoi(argv[4]);
	}
	cout << "k= " << k << "( Last k bits of PC address used )" << endl;

	long long list_array[10];
	string pc_addr[10];
	char branch_status[10];
	int index = 0;
	int trial = 0;
	string historyM;
	for (int i = 0; i < m; i++)
	{
		historyM = historyM + '0';
	}
	cout << "Calculating Misprediction rate ..." << endl;
	//std::map<string, int> ght1,ght2;
	while (!fileIn.eof())
	{
		string currentAddressPC;
		// Reading Trace File
		// format: address(hex)\ T/NT(actual branch direction)
		long long address;
		fileIn >> hex >> address;
		if (fileIn.fail()) break;

		char branchTaken;
		fileIn >> branchTaken;
		if (fileIn.fail()) break;

		fileIn.ignore(256, '\n');

		std::string binary = std::bitset<24>(address).to_string();
		currentAddressPC = binary.substr(binary.length() - k);

		string entry = currentAddressPC + historyM;
		if (entriesUsed.find(entry) == entriesUsed.end())
		{
			entriesUsed[entry] = 1;
		}

		/*if (trial < 2)
		cout << "PC: " << currentAddressPC << endl;
		trial++;*/
		//list_pcaddr.push_back(binary.substr(18));
		//branch_history.push_back(branchTaken);
		char branchTakenBinary = 0;
		if (m >= 0)
		{
			if (branchTaken == 'T')
			{
				branchTakenBinary = '1';
				mnBranchPredictor(currentAddressPC, 1, historyM, n);
			}
			else
			{
				branchTakenBinary = '0';
				mnBranchPredictor(currentAddressPC, 0, historyM, n);
			}
		}
		if (m > 0)
		{
			historyM = historyM + branchTakenBinary;
			historyM = historyM.substr(1);

		}
		//index++;
	}

	/*if ((total % 10000) == 0)
	{
	cout << "Total: " << total << endl;
	}*/
	//cout << "Hits: " << hits << endl;
	//cout << "Total: " << total << endl;
	cout << "Misprediction rate: " << ((double)(total - hits) / total) * 100 << " %"<<endl;
	//cout << "Entries Used: " << entriesUsed.size() << endl;
	cout << "==================" << endl;
	/*fileOut << "{m,n} = {" << m << "," << n << "}" << endl;
	fileOut << "Hits: " << hits << endl;
	fileOut << "Total: " << total << endl;
	fileOut << "Misprediction rate: " << ((double)(total - hits) / total) * 100 << endl;
	fileOut << "==================" << endl;*/
	return 0;
}

