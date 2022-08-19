#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Size of instruction memory of the target machine in 4-byte instructions.
#define INSTRUCTION_MEMORY_SIZE 256u
//Instruction size in bits.
#define INSTRUCTION_SIZE 32u
//Byte size in bits.
#define BYTE_SIZE 8u

typedef unsigned char BYTE;

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

// Represents a 32-bit (4 byte) instruction as an unsigned integer. A location of a bit n in the abstract instruction is
// considered the nth bit of the bits unsigned int, starting from the little end. Instruction begins with all zeros.
class Instruction {
public:
	// Return true iff the bit at location i is 1, else false. If i is out of range, throw exception.
	bool getBitState(unsigned int i) const;
	// Return this instruction formatted as a string of '1' and '0' characters. Output is little endian, so bit 
	// index 0 is on the left of the string (string index zero) and is least significant.
	std::string formattedAsString() const;
	// Returns the data in this instruction from the lower bound to the high bound, inclusive. If either location 
	// is out of range or if upper < lower, throw exception.
	unsigned int getBitsInRange(unsigned int lower, unsigned int upper);

	// Set bit at locaton i to state. If i is out of range, throw exception.
	void setBitState(unsigned int i, bool state);
	// Copy the first upper minus lower + 1 bits from source into this instruction starting at bit lower 
	// (inclusive). If either location is out of range or if upper < lower, throw exception.
	void setBitsInRange(unsigned int lower, unsigned int upper, unsigned int source);

	// Output the raw data in bits to the specified output file stream. Requires file stream to be valid.
	void writeToFile(std::ofstream &out) const;
private:
	// Represents actual bits of instruction. We're using an unsigned int since it has a size of at least 4 bytes,
	// which should provide sufficient room for our 32 bit instructions.
	unsigned int bits = 0u;	
};

#endif
