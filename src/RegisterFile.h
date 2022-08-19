#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "Instruction.h"

// Number of registers in the register file.
#define NUMBER_OF_REGISTERS 32u
// ID of the weird register for returns of certain instructions.
#define WR 0b11111
// ID of the call address register.
#define CA 0b00011
// Each word is represented as a 16 bit signed integer.
typedef std::int16_t WORD;

#ifndef REGISTERFILE_H
#define REGISTERFILE_H

// Represents a register file where each register has an associated number and, using this number ID, a register's 
// value can be read or overwritten. Each register is a 16 bit word.
class RegisterFile {
public:
	// Returns the 16 bit word stored at the register with regID. Throw exception if regsiter is out of range.
	static WORD read(BYTE regID);
	// Writes a 16 bit value to the register with regID. If the regID is that of animmutable register, do nothing.
	// Throw exception if register is out of range.
	static void write(BYTE regID, WORD value);
private:
	// Array of words, where an index i represents the register with id i.
	static std::vector<WORD> registers;
};

#endif
