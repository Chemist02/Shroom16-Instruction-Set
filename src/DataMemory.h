#include "RegisterFile.h"

// Size of data memory in 16-bit words.
#define DATA_MEMORY_SIZE 256u

#ifndef DATAMEMORY_H
#define DATAMEMORY_H

class DataMemory {
public:
	// Sets the value of the register with writeRegID to the value stored at the memory address stored in readRegID
	// plus the given offset. If this address is out of range (word indexed), throw an exception.
	static void readToRegister(BYTE writeRegID, BYTE readRegID, BYTE offset);

	// Writes the 16 bit value stored in the register with ID regWithDataID to the memory location at the address
	// stored in the register with ID regWithAddress, plus a immedate offset. If this address is out of range 
	// (word indexed), throw an exception.
	static void writeFromRegister(BYTE regWithAddressID, BYTE regWithDataID, BYTE offset);
private:
	// Array of words, where an index i represents the value at address i, where addresses are word-indexed.
	static std::vector<WORD> words;
};

#endif
