/* THis file contains the definitions of various methods that run the tomasulo algorithm*/
#include "stdafx.h"
#include "Header.h"
#include <iostream>
#include <fstream>
#include <sstream>

Register registerFile[16];

Register::Register(void) {

	dataValue = 0;
	registerNumber = -1;
	registerName = "";

}

Instruction::Instruction(void) {

	type = NOP;
	dest = -1;
	src1 = -1;
	src2 = -1;
	stage = NONE;
	instructionNumber = -1;

}

Instruction::Instruction(std::string newInst) {
	isLoop = false;
	std::string buf;
	std::stringstream ss(newInst);
	std::vector<std::string> tokens;

	while (ss >> buf) {
		tokens.push_back(buf);
	}

	if (tokens[0] == "ADD")
		type = ADD;
	else if (tokens[0] == "SUB")
		type = SUB;
	else if (tokens[0] == "MULT")
		type = MULT;
	else if (tokens[0] == "DIV")
		type = DIV;
	else if (tokens[0] == "LW")
		type = LW;
	else if (tokens[0] == "SW")
		type = SW;
	else if (tokens[0] == "BNEZ")
		type = BNEZ;
	else if (tokens[0].find("LOOP"))
	{
		isLoop = true;
		loopIdentifier = std::string(tokens[0]);
	}
	else
		type = NOP;

	dest = -1;
	src1 = -1;
	src2 = -1;

	if (!isLoop)
	{
		if (tokens.size() > 1) {
			dest = atoi(tokens[1].erase(0, 1).c_str());
		}
		if (tokens.size() > 2) {
			src1 = atoi(tokens[2].erase(0, 1).c_str());
		}
		if (tokens.size() > 3) {
			src2 = atoi(tokens[3].erase(0, 1).c_str());
		}
	}
	else
	{
		if (tokens.size() > 1) {
			if (tokens[1] == "ADD")
				type = ADD;
			else if (tokens[1] == "SUB")
				type = SUB;
			else if (tokens[1] == "MULT")
				type = MULT;
			else if (tokens[1] == "DIV")
				type = DIV;
			else if (tokens[1] == "LW")
				type = LW;
			else if (tokens[1] == "SW")
				type = SW;
			else if (tokens[1] == "BNEZ")
				type = BNEZ;
			else
				type = NOP;
		}
		if (tokens.size() > 2) {
			dest = atoi(tokens[2].erase(0, 1).c_str());
		}
		if (tokens.size() > 3) {
			src1 = atoi(tokens[3].erase(0, 1).c_str());
		}
		if (tokens.size() > 4) {
			src2 = atoi(tokens[4].erase(0, 1).c_str());
		}
	}

	// Store and BNEZ has 2 source operands and no destination operand
	if (type == SW || type == BNEZ) {
		src2 = src1;
		src1 = dest;
		dest = -1;
	}

	stage = NONE;
}

Application::Application(void) {

	PC = 0;//Sets PC to the beginning of the instruction trace

}
// Initialises the application class object and sets up required parameters.
void Application::loadApplication(std::string fileName, int numberAddRSVal, int numberMultRSVal, int numberLoadStoreRSVal, int numberBranchRSVal, int cyclesForAddVal, int cyclesForMultVal, int cyclesForLoadStoreVal, int cyclesForBranchVal, std::string staticBranchPredictionVal, int addFU, int multFU, int lsFU, int branchFU, int memoryFU, int maxIterationVal)
{
	numberAddRS = numberAddRSVal;
	numberBranchRS = numberBranchRSVal;
	numberLoadStoreRS = numberLoadStoreRSVal;
	numberMultRS = numberMultRSVal;
	cyclesForAdd = cyclesForAddVal;
	cyclesForBranch = cyclesForBranchVal;
	cyclesForLoadStore = cyclesForLoadStoreVal;
	cyclesForMult = cyclesForMultVal;
	staticBranchPrediction = staticBranchPredictionVal;
	totalAddFU = addFU;
	totalMultFU = multFU;
	totalLSFU = lsFU;
	totalBranchFU = branchFU;
	totalMemoryFU = memoryFU;
	maxIterations = maxIterationVal;
	currentIteration = 0;
	sizeOfLoop = 0;
	std::string sLine = "";
	Instruction *newInstruction;
	std::ifstream infile;
	infile.open(fileName.c_str(), std::ifstream::in);

	if (!infile) {
		std::cout << "Failed to open file " << fileName << std::endl;
		return;
	}
	int instrCounter = 0;
	while (!infile.eof())
	{
		getline(infile, sLine);
		if (sLine.empty())
			break;
		newInstruction = new Instruction(sLine);
		newInstruction->instructionNumber = instrCounter;
		instructions.push_back(newInstruction);
		if (newInstruction->isLoop == true)
		{
			loopsInFile[newInstruction->loopIdentifier] = instrCounter;
			loops.push_back(instrCounter);
		}
		instrCounter++;
	}

	infile.close();
	std::cout << "Read file completed!!" << std::endl;
	InitilizeReservationStations();
	InitilizeRegisterFiles();
	printApplication();
	iteration = 0;
	totalInstructionsToRun = instructions.size();
	instructionCounter = 0;

}

void Application::printApplication(void) {

	std::cout << "Printing Application: " << std::endl;
	std::vector<Instruction*>::iterator it;
	for (it = instructions.begin(); it < instructions.end(); it++) {

		(*it)->printInstruction();
		std::cout << std::endl;
	}

}
//Gets the next instruction based on PC value
Instruction* Application::getNextInstruction() {

	Instruction *nextInst = NULL;
	Instruction *currentInst = NULL;
	int iterationOfInstruction;
	int tempCurrentPC;
	if (PC != 0)
	{
		tempCurrentPC = PC - 1;
		currentInst = instructions[PC - 1];
	}
	if (currentInst != NULL && currentInst->type == InstructionType::BNEZ && staticBranchPrediction == "T")
	{
		iteration++;
		if (iteration<maxIterations)
		{
			PC = loops[0];
			if (totalInstructionsToRun == instructions.size())
			{
				totalInstructionsToRun = totalInstructionsToRun + ((maxIterations - 1) * (tempCurrentPC - PC + 1));
				sizeOfLoop = abs(tempCurrentPC - PC + 1);
			}
		}
	}
	if (PC < instructions.size()) {
		nextInst = instructions[PC];
		nextInst->iteration = iteration;
		nextInst->printIteration = iteration;
		if (iteration >= maxIterations || loopComplete)
		{
			loopComplete = true;
			nextInst->iteration = iteration;
			nextInst->printIteration = 0;
			if (iteration >= maxIterations)
			{
				nextInst->iteration = iteration - 1;
				iteration--;
			}
		}
	}

	if (nextInst == NULL)
		nextInst = new Instruction();
	return nextInst;
}

/*
Instruction* Application::getNextInstruction(int loopedTo) {

	Instruction *nextInst = NULL;
	PC = loopedTo;
	if (PC < instructions.size()) {
		nextInst = instructions[PC];
		//PC += 1;
	}

	if (nextInst == NULL)
		nextInst = new Instruction();

	return nextInst;
}*/
//Sets up the Reservation station entries for use
void Application::InitilizeReservationStations()
{
	for (int i = 0; i < numberAddRS; i++)
	{
		ReservationStation *dummyRS = new ReservationStation;
		dummyRS->busy = false;
		dummyRS->waitingFor1 = "";
		dummyRS->waitingFor2 = "";
		dummyRS->addr = 0;
		dummyRS->reservationStationName = "Add" + std::to_string(i);
		dummyRS->typeRS = InstructionType::ADD;
		dummyRS->instructionInRS = Instruction();
		reservationFile.push_back(*dummyRS);
	}
	for (int i = 0; i < numberMultRS; i++)
	{
		ReservationStation *dummyRS = new ReservationStation;
		dummyRS->busy = false;
		dummyRS->waitingFor1 = "";
		dummyRS->waitingFor2 = "";
		dummyRS->addr = 0;
		dummyRS->reservationStationName = "Mult" + std::to_string(i);
		dummyRS->typeRS = InstructionType::MULT;
		dummyRS->instructionInRS = Instruction();
		reservationFile.push_back(*dummyRS);
	}
	for (int i = 0; i < numberLoadStoreRS; i++)
	{
		ReservationStation *dummyRS = new ReservationStation;
		dummyRS->busy = false;
		dummyRS->waitingFor1 = "";
		dummyRS->waitingFor2 = "";
		dummyRS->addr = 0;
		dummyRS->reservationStationName = "Load" + std::to_string(i);
		dummyRS->typeRS = InstructionType::LW;
		dummyRS->instructionInRS = Instruction();
		reservationFile.push_back(*dummyRS);
	}
	for (int i = 0; i < numberBranchRS; i++)
	{
		ReservationStation *dummyRS = new ReservationStation;
		dummyRS->busy = false;
		dummyRS->waitingFor1 = "";
		dummyRS->waitingFor2 = "";
		dummyRS->addr = 0;
		dummyRS->reservationStationName = "Branch" + std::to_string(i);
		dummyRS->typeRS = InstructionType::BNEZ;
		dummyRS->instructionInRS = Instruction();
		reservationFile.push_back(*dummyRS);
	}
}

//Sets up the register file
void Application::InitilizeRegisterFiles()
{
	for (int i = 0; i < 10; i++)
	{
		registerFile[i].registerNumber = i + 1;
		registerFile[i].waitingFor = std::string();
	}
}

//Constructor for the Tomasulo object
Tomasulo::Tomasulo(Application *app) {

	application = app;
	tomasuloDone = false;
	//Initialize registers
	for (int i = 0; i<10; i++)
		registerFile[i] = app->registerFile[i];
	//Initialize Functional Units
	for (int i = 0; i < application->totalAddFU; i++)
	{
		AddExecList.push_back(Instruction());
	}
	for (int i = 0; i < application->totalMultFU; i++)
	{
		MultExecList.push_back(Instruction());
	}
	for (int i = 0; i < application->totalLSFU; i++)
	{
		LSExecList.push_back(Instruction());
	}
	for (int i = 0; i < application->totalBranchFU; i++)
	{
		BranchExecList.push_back(Instruction());
	}
	for (int i = 0; i < application->totalMemoryFU; i++)
	{
		MemoryUnits.push_back(Instruction());
	}
}

// This method contains most of the logic for implementing tomasulo
// 1. Check if any instruction can be written back.
// 2. Check if any instruction can be executed, any memory acces be inititated.
// 3. Fetch instruction based on PC
// 4. Check if instruction fetched, can be saved to reservation station. 
//		If yes, increment PC and save the current instruction to correspondoing type RS. Update the value for waitingFor1(Qi) and waitingFor2 (Qj).
void Tomasulo::cycle(void) {

	cycleTime += 1;
	/*int tempAddInstructionNumber = -1;
	int tempMultInstructionNumber = -1;
	int tempLSInstructionNumber = -1;
	int tempBranchInstructionNumber = -1;*/
	int tempIteration = -1;
	std::vector<Instruction> addWB_List, multWB_list, branch_WBList, memoryLSWB_List;
	bool cycleWriteback = false;
	//Check Add,Mult,Branch from executionList and LS from memory unit for completion, and then add to writebackList/writebackVector
	//if (!cycleWriteback)
	{
		for (Instruction& addInstruction : AddExecList)
		{
			if (addInstruction.instructionNumber != -1)
			{
				if (cycleTime >= addInstruction.executeStartAt + application->cyclesForAdd)
				{
					addInstruction.writeBackStartAt = cycleTime;
					writebackList.push_back(addInstruction);
					//tempAddInstructionNumber = addInstruction.instructionNumber;
					addWB_List.push_back(addInstruction);
					addInstruction.instructionNumber = -1;
					addWrittenBack = true;
					cycleWriteback = true;
					//break;
				}
			}
		}
	}
	/*AddExecList[0].instructionNumber = 22;
	AddExecList[0].executeStartAt = 55;*/
	//if (!cycleWriteback)
	{
		for (Instruction& multInstr : MultExecList)
		{
			if (multInstr.instructionNumber != -1)
			{
				if (cycleTime >= multInstr.executeStartAt + application->cyclesForMult)
				{
					multInstr.writeBackStartAt = cycleTime;
					writebackList.push_back(multInstr);
					//tempMultInstructionNumber = multInstr.instructionNumber;
					multWB_list.push_back(multInstr);
					multInstr.instructionNumber = -1;
					multWrittenBack = true;
					cycleWriteback = true;
					//break;
				}
			}
		}
	}
	//if (!cycleWriteback)
	{
		for (Instruction& branchInstr : BranchExecList)
		{
			if (branchInstr.instructionNumber != -1)
			{
				if (cycleTime >= branchInstr.executeStartAt + application->cyclesForBranch)
				{
					tomasuloIteration++;
					branchInstr.writeBackStartAt = cycleTime;
					writebackList.push_back(branchInstr);
					//tempBranchInstructionNumber = branchInstr.instructionNumber;
					branch_WBList.push_back(branchInstr);
					branchInstr.instructionNumber = -1;
					branchWrittenBack = true;
					cycleWriteback = true;
					//break;
				}
			}
		}
	}

	//if (!cycleWriteback)
	{
		for (Instruction& memoryInstr : MemoryUnits)
		{
			if (memoryInstr.instructionNumber != -1)
			{
				if (cycleTime >= memoryInstr.executeStartAt + application->cyclesForLoadStore)
				{
					memoryInstr.writeBackStartAt = cycleTime;
					writebackList.push_back(memoryInstr);
					//tempLSInstructionNumber = memoryInstr.instructionNumber;
					memoryLSWB_List.push_back(memoryInstr);
					memoryInstr.instructionNumber = -1;
					lsWrittenBack = true;
					cycleWriteback = true;
					//break;
				}
			}
		}
	}

	for (Instruction& memInstr : MemoryUnits)
	{
		for (Instruction& lsInstr : LSExecList)
		{
			if (memInstr.instructionNumber == -1)
			{
				//check LS instructions from executionList and if execution is over or not, if over add to memoryUnit
				if (lsInstr.instructionNumber != -1)
				{
					if ((cycleTime >= lsInstr.executeStartAt + 1) && lsInstr.memoryWaitingFor=="") // Assume 1 cycle to calculate the memory address
					{
						memInstr = lsInstr;
						memInstr.memoryStartAt = cycleTime;
						lsInstr.instructionNumber = -1;
						break;
					}
				}
			}
		}
	}
	for (Instruction& addInstr : AddExecList)
	{
		if (addInstr.instructionNumber == -1)
		{
			int oldestIndex = -1;
			int tempMinimum = 1000;
			//Find oldest instruction to be taken for execution
			for (int k = 0; k < application->reservationFile.size(); k++)
			{
				if (tomasuloIteration >= application->reservationFile[k].instructionInRS.iteration &&  application->reservationFile[k].busy == true && application->reservationFile[k].instructionInRS.executeStartAt == 0 && application->reservationFile[k].waitingFor1 == "" && application->reservationFile[k].waitingFor2 == "" && (application->reservationFile[k].typeRS == InstructionType::ADD || application->reservationFile[k].typeRS == InstructionType::SUB))
				{
					if (application->reservationFile[k].instructionInRS.instructionNumber < tempMinimum)
					{
						tempMinimum = application->reservationFile[k].instructionInRS.instructionNumber;
						oldestIndex = k;
					}
				}
			}
			if (oldestIndex != -1)
			{
				application->reservationFile[oldestIndex].instructionInRS.executeStartAt = cycleTime;
				addInstr = application->reservationFile[oldestIndex].instructionInRS;
			}
		}
	}
	for (Instruction& multInstr : MultExecList)
	{
		if (multInstr.instructionNumber == -1)
		{
			int oldestIndex = -1;
			int tempMinimum = 1000;
			//Find oldest instruction to be taken for execution
			for (int k = 0; k < application->reservationFile.size(); k++)
			{
				if (tomasuloIteration >= application->reservationFile[k].instructionInRS.iteration &&  application->reservationFile[k].busy == true && application->reservationFile[k].instructionInRS.executeStartAt == 0 && application->reservationFile[k].waitingFor1 == "" && application->reservationFile[k].waitingFor2 == "" && (application->reservationFile[k].typeRS == InstructionType::MULT || application->reservationFile[k].typeRS == InstructionType::DIV))
				{
					if (application->reservationFile[k].instructionInRS.instructionNumber < tempMinimum)
					{
						tempMinimum = application->reservationFile[k].instructionInRS.instructionNumber;
						oldestIndex = k;
					}
				}
			}
			if (oldestIndex != -1)
			{
				application->reservationFile[oldestIndex].instructionInRS.executeStartAt = cycleTime;
				multInstr = application->reservationFile[oldestIndex].instructionInRS;
			}
		}
	}
	for (Instruction& lsInstr : LSExecList)
	{
		if (lsInstr.instructionNumber == -1)
		{
			int oldestIndex = -1;
			int tempMinimum = 1000;
			//Find oldest instruction to be taken for execution
			for (int k = 0; k < application->reservationFile.size(); k++)
			{
				if (tomasuloIteration >= application->reservationFile[k].instructionInRS.iteration &&  application->reservationFile[k].busy == true && application->reservationFile[k].instructionInRS.executeStartAt == 0  && application->reservationFile[k].waitingFor2 == "" && (application->reservationFile[k].typeRS == InstructionType::LW || application->reservationFile[k].typeRS == InstructionType::SW))
				{
					if (application->reservationFile[k].instructionInRS.instructionNumber < tempMinimum)
					{
						tempMinimum = application->reservationFile[k].instructionInRS.instructionNumber;
						oldestIndex = k;
					}
				}
			}
			if (oldestIndex != -1)
			{
				application->reservationFile[oldestIndex].instructionInRS.executeStartAt = cycleTime;
				lsInstr = application->reservationFile[oldestIndex].instructionInRS;
				lsInstr.memoryWaitingFor = application->reservationFile[oldestIndex].waitingFor1;
			}
		}
	}
	for (Instruction& branchInstr : BranchExecList)
	{
		if (branchInstr.instructionNumber == -1)
		{
			int oldestIndex = -1;
			int tempMinimum = 1000;
			//Find oldest instruction to be taken for execution
			for (int k = 0; k < application->reservationFile.size(); k++)
			{
				if (tomasuloIteration >= application->reservationFile[k].instructionInRS.iteration &&  application->reservationFile[k].busy == true && application->reservationFile[k].instructionInRS.executeStartAt == 0 && application->reservationFile[k].waitingFor1 == "" && application->reservationFile[k].waitingFor2 == "" && application->reservationFile[k].typeRS == InstructionType::BNEZ)
				{
					if (application->reservationFile[k].instructionInRS.instructionNumber < tempMinimum)
					{
						tempMinimum = application->reservationFile[k].instructionInRS.instructionNumber;
						oldestIndex = k;
					}
				}
			}
			if (oldestIndex != -1)
			{
				application->reservationFile[oldestIndex].instructionInRS.executeStartAt = cycleTime;
				branchInstr = application->reservationFile[oldestIndex].instructionInRS;
			}
		}
	}

	Instruction *instr = application->getNextInstruction();
	InstructionType instrType = instr->type;
	InstructionType normalizedInstrType;
	if (instrType == InstructionType::SUB || instrType == InstructionType::ADD)
		normalizedInstrType = InstructionType::ADD;
	if (instrType == InstructionType::DIV || instrType == InstructionType::MULT)
		normalizedInstrType = InstructionType::MULT;
	if (instrType == InstructionType::SW || instrType == InstructionType::LW)
		normalizedInstrType = InstructionType::LW;
	if (instrType == InstructionType::BNEZ)
		normalizedInstrType = InstructionType::BNEZ;
	if (instrType == InstructionType::NOP)
		normalizedInstrType = InstructionType::NOP;
	ReservationStation *instrRS = new ReservationStation();
	if (application->reservationFile.size() <= application->numberAddRS + application->numberMultRS + application->numberLoadStoreRS + application->numberBranchRS)
	{
		for (int i = 0; i < application->reservationFile.size(); i++)
		{
			if (application->reservationFile[i].typeRS == normalizedInstrType && application->reservationFile[i].busy == false)
			{
				instr->fetchedAt = cycleTime;
				instrRS->instructionInRS = *instr;
				instrRS->busy = true;
				instrRS->typeRS = normalizedInstrType;
				for (int j = 0; j < 10; j++)
				{
					if (registerFile[j].registerNumber == instr->src1)
					{
						if (registerFile[j].waitingFor != "")
							instrRS->waitingFor1 = registerFile[j].waitingFor;
					}
					if (registerFile[j].registerNumber == instr->src2)
					{
						if (registerFile[j].waitingFor != "")
							instrRS->waitingFor2 = registerFile[j].waitingFor;
					}
					if (registerFile[j].registerNumber == instr->dest)
						registerFile[j].waitingFor = application->reservationFile[i].reservationStationName;
				}
				instrRS->instructionInRS.fetchedAt = cycleTime;
				instrRS->reservationStationName = application->reservationFile[i].reservationStationName;
				application->reservationFile[i] = *instrRS;
				application->PC++;
				totalInstructionsTomasulo = application->totalInstructionsToRun;
				break;
			}

		}
	}
	//Clear RS after writeBack
	if (addWrittenBack)
	{
		addWrittenBack = false;
		for each (Instruction wbVar in addWB_List)
		{
			for (int i = 0; i < application->reservationFile.size(); i++)
			{
				if (application->reservationFile[i].instructionInRS.instructionNumber == wbVar.instructionNumber && application->reservationFile[i].instructionInRS.iteration == wbVar.iteration)
				{
					application->reservationFile[i].busy = false;
					application->reservationFile[i].instructionInRS = Instruction();
					application->reservationFile[i].waitingFor1 = "";
					application->reservationFile[i].waitingFor2.clear();
					// Update Registers Also 
					for (int j = 0; j < 11; j++)
					{
						if (registerFile[j].waitingFor == application->reservationFile[i].reservationStationName)
							registerFile[j].waitingFor.clear();
					}
				}
			}
		}
	}
	if (multWrittenBack)
	{
		multWrittenBack = false;
		for each (Instruction wbVar in multWB_list)
		{
			for (int i = 0; i < application->reservationFile.size(); i++)
			{
				if (application->reservationFile[i].instructionInRS.instructionNumber == wbVar.instructionNumber && application->reservationFile[i].instructionInRS.iteration == wbVar.iteration)
				{
					application->reservationFile[i].busy = false;
					application->reservationFile[i].instructionInRS = Instruction();
					application->reservationFile[i].waitingFor1 = "";
					application->reservationFile[i].waitingFor2.clear();
					// Update Registers Also 
					for (int j = 0; j < 11; j++)
					{
						if (registerFile[j].waitingFor == application->reservationFile[i].reservationStationName)
							registerFile[j].waitingFor.clear();
					}
				}
			}
		}
	}
	if (lsWrittenBack)
	{
		lsWrittenBack = false;
		for each (Instruction wbVar in memoryLSWB_List)
		{
			for (int i = 0; i < application->reservationFile.size(); i++)
			{
				if (application->reservationFile[i].instructionInRS.instructionNumber == wbVar.instructionNumber && application->reservationFile[i].instructionInRS.iteration == wbVar.iteration)
				{
					application->reservationFile[i].busy = false;
					application->reservationFile[i].instructionInRS = Instruction();
					application->reservationFile[i].waitingFor1 = "";
					application->reservationFile[i].waitingFor2.clear();
					// Update Registers Also 
					for (int j = 0; j < 11; j++)
					{
						if (registerFile[j].waitingFor == application->reservationFile[i].reservationStationName)
							registerFile[j].waitingFor.clear();
					}
				}
			}
		}
	}
	if (branchWrittenBack)
	{
		branchWrittenBack = false;
		for each (Instruction wbVar in branch_WBList)
		{
			for (int i = 0; i < application->reservationFile.size(); i++)
			{
				if (application->reservationFile[i].instructionInRS.instructionNumber == wbVar.instructionNumber && application->reservationFile[i].instructionInRS.iteration == wbVar.iteration)
				{
					application->reservationFile[i].busy = false;
					application->reservationFile[i].instructionInRS = Instruction();
					application->reservationFile[i].waitingFor1 = "";
					application->reservationFile[i].waitingFor2.clear();
					// Update Registers Also 
					for (int j = 0; j < 11; j++)
					{
						if (registerFile[j].waitingFor == application->reservationFile[i].reservationStationName)
							registerFile[j].waitingFor.clear();
					}
				}
			}
		}
	}
	//Clear waitingFor entries of writtenBack dependent RS instructions 
	for (int x = 0; x < application->reservationFile.size(); x++)
	{
		std::string wf1 = application->reservationFile[x].waitingFor1;
		std::string wf2 = application->reservationFile[x].waitingFor2;
		bool wf1Found = true;
		bool wf2Found = true;
		for (int y = 0; y < application->reservationFile.size(); y++)
		{
			if (wf1 == application->reservationFile[y].reservationStationName && application->reservationFile[y].busy == false)
				wf1Found = false;
			if (wf2 == application->reservationFile[y].reservationStationName && application->reservationFile[y].busy == false)
				wf2Found = false;
		}
		if (!wf1Found)
			application->reservationFile[x].waitingFor1.clear();
		if (!wf2Found)
			application->reservationFile[x].waitingFor2.clear();
	}
	for (Instruction& lsInstr : LSExecList)
	{
		bool wf1Found = true;
		for (int y = 0; y < application->reservationFile.size(); y++)
		{
			if (lsInstr.memoryWaitingFor == application->reservationFile[y].reservationStationName && application->reservationFile[y].busy == false)
				wf1Found = false;
		}
		if (!wf1Found)
		{
			lsInstr.memoryWaitingFor = "";
		}
	}

}
//Checks if all instructions including iterations have been completed and wrttenBack. If yes, mark tomasulo complete
bool Tomasulo::TomasuloDone()
{
	int lastIterationCount = 0;
	for each (Instruction var in writebackList)
	{
		if (var.instructionNumber + (var.iteration*application->sizeOfLoop) == totalInstructionsTomasulo - 1)
		{
			for each (Instruction instr in writebackList)
			{
				if (instr.iteration == var.iteration)
				{
					lastIterationCount++;
				}
			}
			//Check if not-Taken static prediction  and all instructions in the iteration are writtenBack
			if (totalInstructionsTomasulo > application->instructions.size() && lastIterationCount == application->sizeOfLoop)
			{
				done = true;
			}
			else if (lastIterationCount == application->instructions.size())
			{
				//If all instruction in the trace complete and writtenBack
				done = true;
			}
		}
	}
	return done;

}
//Prints output of Tomasulo
void Tomasulo::PrintTomasulo()
{
	std::cout << "\n ================== Tomasulo Output ==================" << std::endl;
	std::cout << "Iteration\tInstruction \t\t\tFetch@ \tExecute@ Memory@ WrtBack@ " << std::endl;
	for each (Instruction wbInstr in writebackList)
	{
		if (wbInstr.type == InstructionType::ADD || wbInstr.type == InstructionType::SUB)
		{
			std::cout << wbInstr.printIteration << "\t\t" << instructionNames[wbInstr.type] << " r" << wbInstr.dest << "  r" << wbInstr.src1 << "  r" << wbInstr.src2 << "\t\t\t" << wbInstr.fetchedAt << "\t" << wbInstr.executeStartAt + application->cyclesForAdd - 1 << "\t * \t" << wbInstr.writeBackStartAt << std::endl;
		}
		if (wbInstr.type == InstructionType::MULT || wbInstr.type == InstructionType::DIV)
		{
			std::cout << wbInstr.printIteration << "\t\t" << instructionNames[wbInstr.type] << " r" << wbInstr.dest << "  r" << wbInstr.src1 << "  r" << wbInstr.src2 << "\t\t\t" << wbInstr.fetchedAt << "\t" << wbInstr.executeStartAt + application->cyclesForMult - 1 << "\t * \t" << wbInstr.writeBackStartAt << std::endl;
		}
		if (wbInstr.type == InstructionType::LW)
		{
			std::cout << wbInstr.printIteration << "\t\t" << instructionNames[wbInstr.type] << " r" << wbInstr.dest << "  r" << wbInstr.src1 << "  " << "\t\t\t" << wbInstr.fetchedAt << "\t" << wbInstr.executeStartAt + application->cyclesForLoadStore - 1 << "\t " << wbInstr.memoryStartAt << "\t" << wbInstr.writeBackStartAt << std::endl;
		}
		if (wbInstr.type == InstructionType::SW)
		{
			std::cout << wbInstr.printIteration << "\t\t" << instructionNames[wbInstr.type] << " r" << wbInstr.src1 << "  r" << wbInstr.src2 << "  " << "\t\t\t" << wbInstr.fetchedAt << "\t" << wbInstr.executeStartAt + application->cyclesForLoadStore - 1 << "\t " << wbInstr.memoryStartAt << "\t *" << std::endl;
		}
		if (wbInstr.type == InstructionType::BNEZ)
		{
			std::cout << wbInstr.printIteration << "\t\t" << instructionNames[wbInstr.type] << " " << "  r" << wbInstr.src1 << "  r" << wbInstr.src2 << "\t\t\t" << wbInstr.fetchedAt << "\t" << wbInstr.executeStartAt + application->cyclesForBranch - 1 << "\t * \t *" << std::endl;
		}
	}
}
//Prints Instructions from input Trace
void Instruction::printInstruction(void) {
	if (isLoop)
	{
		if (type == NOP)
			std::cout << "Loop " << instructionNames[type] << "         ";
		else if (type == SW || type == BNEZ)
			std::cout << "Loop " << instructionNames[type] << " r" << src1 << " r" << src2;
		else if (type == LW)
			std::cout << "Loop " << instructionNames[type] << " r" << dest << " r" << src1;
		else
			std::cout << "Loop " << instructionNames[type] << " r" << dest << " r" << src1 << " r" << src2;
	}
	else
	{
		if (type == NOP)
			std::cout << instructionNames[type] << "         ";
		else if (type == SW || type == BNEZ)
			std::cout << instructionNames[type] << " r" << src1 << " r" << src2;
		else if (type == LW)
			std::cout << instructionNames[type] << " r" << dest << " r" << src1;
		else
			std::cout << instructionNames[type] << " r" << dest << " r" << src1 << " r" << src2;
	}
}
