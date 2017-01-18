// ACA_Tomasulo : Project is basically an automated tomasulo simulator.
// Supports configurable multiple functional units, number of reservation station, cycle durations, static branch prediction and maximum iterations for loop on branch.
//Authors: Dinesh Patil (861309827) && Nisarg Shah (861306236)
//

#include "stdafx.h"
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
#include <sstream>
#include "Header.h"
using namespace std;
int main(int argc, char *argv[]) {
	if (argc != 17) {
		cerr << "ERROR: Please provide a file name for input trace file, number of add RS,mult RS,LoadStore Buffer/RS, Branch RS, numer of cycles for add,mult,memory,static Branch Prediction (T for taken,any other char for not taken), number of functional units for add,mult, memory address calculation, branch,memory operation , maximum iterations for loop after branch\n";
		return 0;
	}
	int numberAddRS, numberMultRS, numberLoadStoreRS, numberBranchRS, cyclesForAdd, cyclesForMult, cyclesForLoadStore, cyclesForBranch, totalAddFU, totalMultFU, totalLSFU, totalBranchFU, totalMemoryFU, maxIterations;
	std::string staticBranchPrediction;
	ifstream fileIn(argv[1]);
	if (!fileIn.is_open()) {
		cerr << "ERROR: The input file does not exist!\n";
		return 1;
	}
	string fileName = string(argv[1]);
	numberAddRS = atoi(argv[2]);
	numberMultRS = atoi(argv[3]);
	numberLoadStoreRS = atoi(argv[4]);
	numberBranchRS = atoi(argv[5]);
	cyclesForAdd = atoi(argv[6]);
	cyclesForMult = atoi(argv[7]);
	cyclesForLoadStore = atoi(argv[8]);
	cyclesForBranch = atoi(argv[9]);
	staticBranchPrediction = string(argv[10]);
	totalAddFU = atoi(argv[11]);
	totalMultFU = atoi(argv[12]);
	totalLSFU = atoi(argv[13]);
	totalBranchFU = atoi(argv[14]);
	totalMemoryFU = atoi(argv[15]);
	maxIterations = atoi(argv[16]);
	maxIterations = maxIterations + 1;

	cout << "Loading application..." << fileName << endl;
	Application application;
	application.loadApplication(fileName, numberAddRS, numberMultRS, numberLoadStoreRS, numberBranchRS, cyclesForAdd, cyclesForMult, cyclesForLoadStore, cyclesForBranch, staticBranchPrediction, totalAddFU, totalMultFU, totalLSFU, totalBranchFU, totalMemoryFU, maxIterations);
	cout << "Initializing Tomasulo..." << endl;
	Tomasulo tomasulo(&application);


	do {
		tomasulo.cycle();
		//tomasulo.printPipeline();

	} while (!tomasulo.TomasuloDone());//(!tomasulo.done());
	tomasulo.PrintTomasulo();
	//cout << "Completed in " << pipeline.cycleTime - 1 << " cycles" << endl;
	return 0;
}



