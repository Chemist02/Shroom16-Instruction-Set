#include "RegisterFile.h"

std::vector<WORD> RegisterFile::registers(NUMBER_OF_REGISTERS, 0);

// Returns the 16 bit word stored at the register with regID. Throw exception if regsiter is out of range.
WORD RegisterFile::read(BYTE regID) {
	// Ensure register ID is valid/in range.
	if ((unsigned int)regID >= NUMBER_OF_REGISTERS) {
		throw std::invalid_argument("Invalid read register ID.");
	}

	// Now that we know we're good, return the value.
	return RegisterFile::registers[(unsigned int)regID];
}

// Writes a 16 bit value to the register with regID. If the regID is that of an immutable register, do nothing.
// Throw exception if register is out of range.
void RegisterFile::write(BYTE regID, WORD value) {
	// Ensure register ID is valid/in range.
	if ((unsigned int)regID >= NUMBER_OF_REGISTERS) {
		throw std::invalid_argument("Invalid write register ID.");
	}

	// If we're dealing with an immutable register, do nothing (either $0 or $wr).
	if (regID == 0b00000u || regID == 0b11111u) {
		return;
	}

	// Now that we know we're good, set the value.
	RegisterFile::registers[(unsigned int)regID] = value;
}
