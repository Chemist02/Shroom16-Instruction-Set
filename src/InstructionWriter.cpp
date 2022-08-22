#include "InstructionWriter.h"

// Maps labels onto definitions (i.e. instrction memory addresses).
std::map<std::string, unsigned int> InstructionWriter::labelMap;
// Maps constants onto integer definitions.
std::map<std::string, int> InstructionWriter::constantMap;
// Maps mnemonics onto corresponding write functions (i.e. for each instruction, gives instructions on how to
// format it into machine code).
const std::map<std::string, std::function<void(Instruction&, const std::vector<std::string>&)> > 
		InstructionWriter::mnemonicToWriteFuncts = {
	{"add", InstructionWriter::writeRType3},
	{"sub", InstructionWriter::writeRType3},
	{"mul", InstructionWriter::writeRType3},
	{"div", InstructionWriter::writeRType3},
	{"sll", InstructionWriter::writeRType3},
	{"srl", InstructionWriter::writeRType3},
	{"nor", InstructionWriter::writeRType3},
	{"or", InstructionWriter::writeRType3},
	{"and", InstructionWriter::writeRType3},
	{"xor", InstructionWriter::writeRType3},
	{"lw", InstructionWriter::writeLW},
	{"sw", InstructionWriter::writeSW},
	{"addi", InstructionWriter::writeIType},
	{"slli", InstructionWriter::writeIType},
	{"srli", InstructionWriter::writeIType},
	{"nori", InstructionWriter::writeIType},
	{"ori", InstructionWriter::writeIType},
	{"andi", InstructionWriter::writeIType},
	{"xori", InstructionWriter::writeIType},
	{"cmp", InstructionWriter::writeRType3},
	{"jmp", InstructionWriter::writeJType},
	{"jeq", InstructionWriter::writeCondJType},
	{"jlt", InstructionWriter::writeCondJType},
	{"jgt", InstructionWriter::writeCondJType},
	{"call", InstructionWriter::writeJType},
	{"jr", InstructionWriter::writeRType1Read},
	{"random", InstructionWriter::writeRType1Write},
	{"?in", InstructionWriter::writeRType1Write},
	{"?out", InstructionWriter::writeRType1Read},
	{"?end", InstructionWriter::writeNothing},
	{"?charset", InstructionWriter::writeCharset},
	{"?keyin", InstructionWriter::writeRType1Write},
	{"?pxset", InstructionWriter::writePxset},
	{"?clrscrn", InstructionWriter::writeNothing}
};

// Given a parsed instruction parsedLine, output a machine code version of that instruction to the 
// outInstruction Instruction object. Throw exception if any of the instruction is not valid.
void InstructionWriter::writeInstruction(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure instruction is populated.
	if (parsedLine.size() < 1) {
		throw std::runtime_error("Tried to translate blank instruction.");
	}
	
	// Ensure mnemonic is defined.
	if (InstructionWriter::mnemonicToWriteFuncts.find(parsedLine[0]) == InstructionWriter::mnemonicToWriteFuncts.end()) {
		throw std::runtime_error("Invalid instruction mnemonic " + parsedLine[0] + ".");
	}
	
	// Actually attempt to write the function, catching and propagating any excpetions that are thrown.
	try {
		// Write opcode.
		InstructionWriter::writeOpcode(outInstruction, parsedLine[0]);
		// Write the rest of the instruction.
		InstructionWriter::mnemonicToWriteFuncts.find(parsedLine[0])->second(outInstruction, parsedLine);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Define a constant with name name and value value. Throws an exception if there is already a constant with
// this name, if the constant name matches a mnemonic, if the value is non-numeric, or if name does not start with a letter. Takes the following:
// - The name of the constant.
// - The value of the constant as a string.
void InstructionWriter::defineConstant(const std::string &name, const std::string &value) {
	// Check if constant is already defined. If it is, throw an excpetion.
	if (InstructionWriter::constantMap.find(name) != InstructionWriter::constantMap.end()) {
		throw std::runtime_error("Constant " + name + " already defined.");
	}
	// Check if constant name starts with a letter, else throw exception.
	if (!Parser::isAlphabetical(name[0])) {
		throw std::runtime_error("Constants must begin with a letter.");
	}
	// Check if constant name matches a mnemonic, if it does throw exception.
	if (mnemonicToOpcodeMap.find(name) != mnemonicToOpcodeMap.end()) {
		throw std::runtime_error("Constants cannot match instruction mnemonics.");
	}
	// Check if value is numeric, if it's not throw an excpetion.
	if (!Parser::isNumeric(value)) {
		throw std::runtime_error("Constant value " + value + " is not numeric.");
	}
	//Otherwise, go ahead and define it!
	InstructionWriter::constantMap[name] = std::stoi(value);
}

// Define a label with name name and address address. Throws an exception if there is already a label with
// this name or if name is empty.
void InstructionWriter::defineLabel(const std::string &name, unsigned int address) {
	// Check if label is already defined. If it is, throw an excpetion.
	if (InstructionWriter::labelMap.find(name) != InstructionWriter::labelMap.end()) {
		throw std::runtime_error("Label " + name + " already defined.");
	}
	// Esnure the name is non-empty.
	if (name.size() == 0) {
		throw std::runtime_error("Label name must be non-empty.");
	}
	//Otherwise, go ahead and define it!
	InstructionWriter::labelMap[name] = address;
}

// HELPER FUNCTIONS.
// Writes an 5 bit opcode to target given a mnemonic string.
void InstructionWriter::writeOpcode(Instruction &target, const std::string &mnemonic) {
	// Ensure instruction is defined in the instruction set. If it's not, throw excepetion.
	if (mnemonicToOpcodeMap.find(mnemonic) == mnemonicToOpcodeMap.end()) {
		throw std::runtime_error("Invalid instruction mnemonic " + mnemonic + ".");
	}
	target.setBitsInRange(0, 5, mnemonicToOpcodeMap.find(mnemonic)->second);
}

// Writes a 5 bit register ID to target given a register string and a start index (i.e. where to start writing
// the bits from).
void InstructionWriter::writeRegister(const std::string &regString, Instruction &target, 
		unsigned int startInd) {
	// Ensure register exists/is defined. If it's not, throw exception.
	if (registerToBinaryMap.find(regString) == registerToBinaryMap.end()) {
		throw std::runtime_error("Invalid register " + regString + ".\nReggie the register is very sad :(((");
	}
	target.setBitsInRange(startInd, startInd + 4, registerToBinaryMap.find(regString)->second);
}

// Write an immediate value immediate to target given a start index and a number of bits of immediate to
// write (size). The immediate value is passed as a string, and also checks for constants it may be if it
// is non-numeric. Throws an exception if the value is non-numeric but also not defined as a constant.
void InstructionWriter::writeImmediateValue(const std::string &immediate, Instruction &target, 
		unsigned int startInd, unsigned int size) {
	// Determine what value to write by checking if it's numeric or a constant.
	int toWrite = 0;
	// If it is numeric, we're dealing with a straight number.
	if (Parser::isNumeric(immediate)) {
		toWrite = std::stoul(immediate);
	}
	// Else it's either a constant or invalid.
	else {
		// Check it it's in the constant map. If not, throw exception.
		if (InstructionWriter::constantMap.find(immediate) == InstructionWriter::constantMap.end()) {
			throw std::runtime_error("Undefined constant " + immediate + ".");
		}
		// Otherwise, it is and we should find the value to write.
		toWrite = InstructionWriter::constantMap.find(immediate)->second;
	}
	
	// Now that we know the value, go right ahead and write it to the instruction.
	target.setBitsInRange(startInd, (startInd + size) - 1, toWrite);	
}

// Write a label to target given its name as a string by looking it up in the label map. If the label
// is not defined, throw an exception.
void InstructionWriter::writeLabel(const std::string &label, Instruction &target) {
	// Try to fetch label to write from our label map. If we can't find it, it's undefined and we should 
	// throw an exception.
	unsigned int addressToWrite = 0;
	if (InstructionWriter::labelMap.find(label) == InstructionWriter::labelMap.end()) {
		throw std::runtime_error("Undefined label " + label + ".");
	}
	addressToWrite = InstructionWriter::labelMap.find(label)->second;

	// Instruction memory is only 512 instructions large, so jump addresses only need to be 9 bits.
	target.setBitsInRange(16, 24, addressToWrite);
}

/* These functions all help us write categories of instruction with similar formats.*/
// R type write function (for instructions that take 3 registers).
void InstructionWriter::writeRType3(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (4 including mnemonic).
	if (parsedLine.size() != 4) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write registers.
		// Destination.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 6);
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[2], outInstruction, 11);
		// Read register 2.
		InstructionWriter::writeRegister(parsedLine[3], outInstruction, 16);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// R type write function (for instructions that take onee (one and only one) read register).
void InstructionWriter::writeRType1Read(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (2 including mnemonic).
	if (parsedLine.size() != 2) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write registers.
		// Destination section is padded with zeros.
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 11);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// R type write function (for instructions that take onee (one and only one) write register).
void InstructionWriter::writeRType1Write(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (2 including mnemonic).
	if (parsedLine.size() != 2) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write registers.
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 6);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// I type write function (for instructions that take a destination register, a read register, and a 16-bit 
// immediate value).
void InstructionWriter::writeIType(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (4 including mnemonic).
	if (parsedLine.size() != 4) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {	
		// Write registers.
		// Destination.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 6);
		// Read register.
		InstructionWriter::writeRegister(parsedLine[2], outInstruction, 11);

		// Write immediate value.
		// Length of immediate value to write. 
		unsigned int length = 16u;
		InstructionWriter::writeImmediateValue(parsedLine[3], outInstruction, 16, length);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// J type write function (for instructions that take a jump address (i.e. a label)).
void InstructionWriter::writeJType(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (2 including mnemonic).
	if (parsedLine.size() != 2) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write jump address.
		InstructionWriter::writeLabel(parsedLine[1], outInstruction);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Conditional jump type write function (for instructions that take a jump address (i.e. a label)).
void InstructionWriter::writeCondJType(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (3 including mnemonic).
	if (parsedLine.size() != 3) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write compare result regsiter address.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 11);
		// Write jump address.
		InstructionWriter::writeLabel(parsedLine[2], outInstruction);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

/* The following write functions didn't seem to fit into any of the other categories, and thus exist on their
   own here*/
// Write function for save word.
void InstructionWriter::writeSW(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (4 including mnemonic).
	if (parsedLine.size() != 4) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write regiters.
		// Destination section undefined and thus padded with zeros.
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[2], outInstruction, 11);
		// Read register 2.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 16);

		//Write immediate offset value. 
		InstructionWriter::writeImmediateValue(parsedLine[3], outInstruction, 21, 8);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Write function for load word.
void InstructionWriter::writeLW(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (4 including mnemonic).
	if (parsedLine.size() != 4) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write regiters.
		// Destination section
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 6);
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[2], outInstruction, 11);
		// Read register 2 section padded with zeros.

		//Write immediate offset value. 
		InstructionWriter::writeImmediateValue(parsedLine[3], outInstruction, 21, 8);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Write function for character set interrupt.
void InstructionWriter::writeCharset(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (3 including mnemonic).
	if (parsedLine.size() != 3) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write regiters.
		// Destination section undefined and thus padded with zeros.
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 11);
		
		//Write character code as immediate value. 
		InstructionWriter::writeImmediateValue(parsedLine[2], outInstruction, 16, 8);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Write function for pixel set interrupt.
void InstructionWriter::writePxset(Instruction &outInstruction, const std::vector<std::string> &parsedLine) {
	// Ensure a sufficient number of arguments was given (4 including mnemonic).
	if (parsedLine.size() != 4) {
		throw std::runtime_error("Invalid number of arguments.");
	}

	try {
		// Write regiters.
		// Destination section undefined and thus padded with zeros.
		// Read register 1.
		InstructionWriter::writeRegister(parsedLine[1], outInstruction, 11);
		// Read register 2.
		InstructionWriter::writeRegister(parsedLine[2], outInstruction, 16);	
		//Write on/off status as immediate value. 
		InstructionWriter::writeImmediateValue(parsedLine[3], outInstruction, 21, 1);
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}
