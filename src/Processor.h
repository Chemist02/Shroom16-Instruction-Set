#include <fstream>
#include <iostream>
#include <functional>
#include <map>
#include <time.h>
#include <random>
#include <string>
#include "DataMemory.h"
#include "PixelScreen.h"

#ifndef PROCESSOR_H
#define PROCESSOR_H

// This class is the glue that holds the virtual machine together. It contains the instruction memory and is responible
// for actually executing instructions as we run our program. This is essentially what the main function is going to be
// interacting with.
class Processor {
public:
	static const std::string &getCharDisplay();
	static WORD getCurrentOutputNumber();
	static bool isWaitingForInput();
	static WORD getProgramCounter();
	static Instruction getNextInstruction();
	static void inputNumber(const std::string &numIn);
	static void setCurrentKeypadState(WORD state);
	static WORD getCurrentKeypadState();
		
	static WORD signExtendToWord(WORD value, unsigned int numOfBitsInValue);

	// Run the next processor task. Usually this is the next instruction as pointed to by the program counter, but
	// if there's an interrupt in progress, other tasks are also possible (e.g. waiting for a number to be 
	// entered).
	static void runNextTask();
	// Load machine code from an assembled source into instruction memory. Requires the input file to be a valid
	// file opened for biary reading.
	static void loadMachineCode(std::ifstream &codeFile);

private:
	// Functions to execute each of the instructions. Each takes a destination register id, two read registers a 
	// and b, a memory address offset for lw and sw, an immediate value, and a label to jump to. Most of the time
	// these parameters are not all needed so many are left blank. 
	static void ADD(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void SUB(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void MUL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void DIV(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void SLL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void SRL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void NOR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void OR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void AND(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void XOR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void LW(BYTE rdest, BYTE rreada, BYTE, BYTE offset, WORD, WORD);
	static void SW(BYTE, BYTE rreada, BYTE rreadb, BYTE offset, WORD, WORD);
	static void ADDI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void SLLI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void SRLI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void NORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void ORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void ANDI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void XORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void CMP(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD);
	static void JMP(BYTE, BYTE, BYTE, BYTE, WORD, WORD label);
	static void JEQ(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label);
	static void JLT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label);
	static void JGT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label);
	static void CALL(BYTE, BYTE, BYTE, BYTE, WORD, WORD label);
	static void JR(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD);
	static void RANDOM(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD);
	static void IN(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD);
	static void OUT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD);
	static void END(BYTE, BYTE, BYTE, BYTE, WORD, WORD);
	static void CHARSET(BYTE, BYTE rreada, BYTE, BYTE, WORD immed, WORD);
	static void KEYIN(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD);
	static void PXSET(BYTE, BYTE rreada, BYTE rreadb, BYTE, WORD immed, WORD);
	static void CLRSCRN(BYTE, BYTE, BYTE, BYTE, WORD, WORD);
private:
	// Vector of binary instructions.
	static std::vector<Instruction> instructionMemory;
	// Our current location in instruction memory.
	static WORD programCounter;
	// True iff an ?in interrupt was used.
	static bool waitingForInput;
	// Id of register to place input into.
	static BYTE inputResultRegID;
	// Number we're currently outputting to the screen.
	static WORD currentOutputNumber;
	// Current state of the keyboard.
	static WORD currentKeypadState;
	// Array of letters in the character out display.
	static std::string charDisplay;
	// Mersenne twister random number generator.
	static std::mt19937 mt;
	// True iff the program has crashed.
	static bool programHasCrashed;
	// Maps 6 bit opcodes into instruction functions.
	static const std::map<unsigned int, std::function<void(BYTE, BYTE, BYTE, BYTE, WORD, WORD)> > 
	opcodeToInstructionMap;
};

#endif
