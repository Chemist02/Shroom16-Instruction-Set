#include "DataMemory.h"

std::vector<WORD> DataMemory::words(DATA_MEMORY_SIZE, 0u);

// Sets the value of the register with writeRegID to the value stored at the memory address stored in readRegID
// plus the given offset. If this address is out of range (word indexed), throw an exception.
void DataMemory::readToRegister(BYTE writeRegID, BYTE readRegID, BYTE offset) {
	try {
		// Calculate our final memory address.
		unsigned int address = RegisterFile::read(readRegID);
		address += (unsigned int)offset;

		// Ensure address is in range.
		if (address >= DATA_MEMORY_SIZE) {
			throw std::runtime_error("Data memory read address out of range!");
		}

		// If it is, we can proceed.
		RegisterFile::write(writeRegID, DataMemory::words[address]);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Writes the 16 bit value stored in the register with ID regWithDataID to the memory location at the address
// stored in the register with ID regWithAddress, plus a immedate offset. If this address is out of range 
// (word indexed), throw an exception.
void DataMemory::writeFromRegister(BYTE regWithAddressID, BYTE regWithDataID, BYTE offset) {
	try {
		// Calculate our final memory address.
		unsigned int address = RegisterFile::read(regWithAddressID);
		address += (unsigned int)offset;

		// Ensure address is in range.
		if (address >= DATA_MEMORY_SIZE) {
			throw std::runtime_error("Data memory read address out of range!");
		}

		// Get data that we're ging to write into memory.
		WORD data = RegisterFile::read(regWithDataID);

		// Actually perform the write.
		DataMemory::words[address] = data;
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}
