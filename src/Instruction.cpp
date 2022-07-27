#include "Instruction.h"

// Return true iff the bit at location i is 1, else false. If i is out of range, throw exception.
bool Instruction::getBitState(unsigned int i) const {
	// Ensure i is within range.
	if (i >= INSTRUCTION_SIZE) {
		throw std::invalid_argument("Bit index out of range!");
	}

	// Extract desired bit.
	return (bool)(this->bits & (0x01 << i));
}

// Return this instruction formatted as a string of '1' and '0' characters. Output is little endian, so bit 
// index 0 is on the left of the string (string index zero) and is least significant.
std::string Instruction::formattedAsString() const {
	std::string formattedString;
	formattedString.resize(INSTRUCTION_SIZE);
	for (unsigned int i = 0; i < INSTRUCTION_SIZE; i++) {
		// Set char at i to 1 if the bit state is true, else 0.
		formattedString[i] = this->getBitState(i) ? '1' : '0';
	}
	return formattedString;
}

// Set bit at locaton i to state. If i is out of range, throw exception,
void Instruction::setBitState(unsigned int i, bool state) {
	// Ensure i is within range.
	if (i >= INSTRUCTION_SIZE) {
		throw std::invalid_argument("Bit index out of range!");
	}
	
	// Set bit to desired preference.
	this->bits = state ? this->bits | (0x01 << i) : this->bits & ~(0x01 << i);
}

// Copy the first upper minus lower + 1 bits from source into this instruction starting at bit lower (inclusive). If 
// eitherlocation is out of range or if upper < lower, throw exception.
void Instruction::setBitsInRange(unsigned int lower, unsigned int upper, unsigned int source) {
	// Ensure indices are valid.
	if (upper < lower || (upper - lower) + 1 > INSTRUCTION_SIZE || upper >= INSTRUCTION_SIZE) {
		throw std::invalid_argument("Bit indices out of range!");
	}

	// Repeatedly strip off least significant bit and place into corresponding location in this instruction.
	for (unsigned int i = lower; i <= upper; i++) {
		this->setBitState(i, (bool)(source & 0x01));
		source >>= 1;
	}
}

// Output the raw data in bits to the specified output file stream. Requires file stream to be valid.
void Instruction::writeToFile(std::ofstream &out) const {
	for (unsigned int i = 0; i < (unsigned int)(INSTRUCTION_SIZE / BYTE_SIZE); i++) {
		out.put((BYTE)(this->bits >> (BYTE_SIZE * i)));
	}
}
