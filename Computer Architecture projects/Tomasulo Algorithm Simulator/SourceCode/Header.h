#pragma once
#include <string>
#include <vector>
#include <map>
/*Header contains the structure of classes used for implementing TOmasulo algorithm*/

/* Types of possible instruction types */
enum InstructionType {
	NOP = 0,	// NOP. Pipeline bubble.
	ADD,		// Add
	SUB,		// Subtract
	MULT,		// Multiply
	DIV,		// Divide
	LW,		// Load word
	SW,		// Store word
	BNEZ,		// Branch not equal to zero
	LOOP	//Loop begin

};

/* Names of possible instruction types */
const std::string instructionNames[9] = { "*", "ADD", "SUB", "MULT", "DIV", "LW", "SW", "BNEZ","LOOP" };

enum Stage {
	FETCH = 0,
	DECODE,
	EXEC,
	MEM,
	WB,
	NONE
};

const std::string stageNames[6] = { "FETCH", "DECODE", "EXEC", "MEM", "WB", "NONE" };


/* A Single Register Entry containing register number and register data value */
class Register {
public:
	Register();
	int dataValue;
	int registerNumber;
	std::string registerName;
	std::string waitingFor;
};

static class Instruction {
public:
	Instruction();
	Instruction(std::string);
	std::string loopIdentifier;
	bool isLoop;
	InstructionType type; // Type of instruction
	int dest; // Destination register number
	int src1; // Source register numbers
	int src2; // Source register number
	void printInstruction();
	Stage stage;
	std::string memoryWaitingFor;//reservation station name for which a memory operation is waiting
	int fetchedAt = 0;// cycle number for fetch of the instruction
	int executeStartAt = 0;//cycle number for execution start of the instruction
	int memoryStartAt = 0;//cycle number for memory access start for the instruction
	int writeBackStartAt = 0;//cycle at which instruction written to register file
	int instructionNumber;//Instruction number of the instruction
	int iteration = 0;//Iteration of the instruction
	int printIteration = 0;//Iteration for printing is different as compared to internal calculations for which iteration variable is used.
};

/* A Single Reservation Station entry */
class ReservationStation {
public:
	bool busy;
	InstructionType typeRS;
	std::string waitingFor1;
	std::string waitingFor2;
	long addr;
	std::string reservationStationName;
	Instruction instructionInRS;
	//std::vector<Instruction*> instructions;
};

// Maintains details of the application
class Application {

public:
	Application();
	void loadApplication(std::string, int, int, int, int, int, int, int, int, std::string, int, int, int, int, int, int);
	void printApplication();
	Instruction* getNextInstruction();
	Instruction* getNextInstruction(int);
	void InitilizeReservationStations();
	void InitilizeRegisterFiles();
	std::vector<Instruction*> instructions;
	std::map<std::string, int> loopsInFile;
	std::vector<int> loops;
	int PC;
	int numberAddRS, numberMultRS, numberLoadStoreRS, numberBranchRS, cyclesForAdd, cyclesForMult, cyclesForLoadStore, cyclesForBranch, totalAddFU, totalMultFU, totalLSFU, totalBranchFU, totalMemoryFU;
	std::string staticBranchPrediction;
	std::vector<ReservationStation> reservationFile;
	Register registerFile[16];
	int iteration;
	int totalInstructionsToRun;
	int instructionCounter;
	int currentIteration;
	int sizeOfLoop;
	int maxIterations;

	bool loopComplete = false;
};

// Class Maintains details of the current state of tomasulo algorithm and corresponding functions run the tomasulo algorithm
class Tomasulo
{
public:
	Tomasulo(Application*);
	int numberAddRS, numberMultRS, numberLoadStoreRS, numberBranchRS, cyclesForAdd, cyclesForMult, cyclesForLoadStore, cyclesForBranch, cycleTime;
	Application *application;
	void cycle();
	bool done;
	bool forwarding;
	std::vector<Instruction> instructionQueue;
	std::map<std::string, Instruction> executionList;
	Instruction memoryUnit;
	Instruction branchUnit;
	std::vector<Instruction> writebackList;
	Register registerFile[16];
	bool TomasuloDone();
	bool tomasuloDone;
	int totalInstructionsTomasulo;
	int tomasuloIteration = 0;
	void PrintTomasulo();
	bool addWrittenBack, multWrittenBack, lsWrittenBack, branchWrittenBack;
	std::vector<Instruction> AddExecList, MultExecList, LSExecList, BranchExecList, MemoryUnits;
};
class FunctionalUnit {
public:
	bool isFUBusy;
	std::string containsRS;
};

//#pragma once
