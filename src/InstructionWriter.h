#include <functional>
#include "Instruction.h"
#include "Parser.h"
// Defines mnemonicToOpcodeMap and registerToBinaryMap.
#include "OpcodeRegisterMaps.h"

#ifndef INSTRUCTION_WRITER_H
#define INSTRUCTION_WRITER_H

// This class acts as a container for storing functions that convrt plaintext assembly into shroom16 machine code.
// Only one of these assembling functions is public, which determines which private function to call to format a
// given input instruction. This class also keeps track of all constants and labels in existance, and uses these
// when assembling. These are two public functions that allow constants and labels to be defined. All of these are
// static.

// This class also mainatains the following static maps:
// - A map mapping label strings (i.e. for jumps) onto unsigned ints representing instruction code addresses to 
// jump to.
// - A map mapping directive-defined constants onto thir respective numeric interger values.
class InstructionWriter {
public:
	// Given a parsed instruction parsedLine, output a machine code version of that instruction to the 
	// outInstruction Instruction object. Takes the following:
	// - The instruction object that will be output to, passed by reference.
	// - The line of code to be converted into machine code (passed through the parseLine function first, hence
	// is a vector of strings).
	static void writeInstruction(Instruction &outInstruction, const std::vector<std::string> &parsedLine);
	// Define a constant with name name and value value. Throws an exception if there is already a constant with
	// this name, if the constant name matches a mnemonic, if the value is non-numeric, or if name does not start with a letter. Takes the following:
	// - The name of the constant.
	// - The value of the constant as a string.
	static void defineConstant(const std::string &name, const std::string &value);
	// Define a label with name name and address address. Throws an exception if there is already a label with
	// this name or if the name does not start with a colon.
	static void defineLabel(const std::string &name, unsigned int address);

private:
	// HELPER FUNCTIONS.
	// Writes an 5 bit opcode to target given a mnemonic string.
	static void writeOpcode(Instruction &target, const std::string &mnemonic);

	// Writes a 5 bit register ID to target given a register string and a start index (i.e. where to start writing
	// the bits from).
	static void writeRegister(const std::string &regString, Instruction &target, unsigned int startInd);

	// Write an immediate value immediate to target given a start index and a number of bits of immediate to
	// write (size). The immediate value is passed as a string, and also checks for constants it may be if it
	// is non-numeric. Throws an exception if the value is non-numeric but also not defined as a constant.
	static void writeImmediateValue(const std::string &immediate, Instruction &target, unsigned int startInd,
		unsigned int size);

	// Write a label to target given its name as a string by looking it up in the label map. If the label
	// is not defined, throw an exception.
	static void writeLabel(const std::string &label, Instruction &target);

private:
	/* These functions all help us write categories of instruction with similar formats.*/
	// R type write function (for instructions that take 3 registers).
	static void writeRType3(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// R type write function (for instructions that take onee (one and only one) read register).
	static void writeRType1Read(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// R type write function (for instructions that take onee (one and only one) write register).
	static void writeRType1Write(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// I type write function (for instructions that take a destination register, a read register, and a 16-bit 
	// immediate value).
	static void writeIType(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// J type write function (for instructions that take a jump address (i.e. a label)).
	static void writeJType(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// Conditional jump type write function (for instructions that take a jump address (i.e. a label)).
	static void writeCondJType(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	/* The following write functions didn't seem to fit into any of the other categories, and thus exist on their
	   own here*/
	// Write function for save word.
	static void writeSW(Instruction &outInstruction, const std::vector<std::string> &parsedLine);


	// Write function for load word.
	static void writeLW(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// Write function for character set interrupt.
	static void writeCharset(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// Write function for pixel set interrupt.
	static void writePxset(Instruction &outInstruction, const std::vector<std::string> &parsedLine);

	// Placeholder write function taht does nothing, just used to populate the map for organiziation's sake.	
	static void writeNothing(Instruction, const std::vector<std::string>) { return; }

private:
	// Maps labels onto definitions (i.e. instrction memory addresses).
	static std::map<std::string, unsigned int> labelMap;
	// Maps constants onto integer definitions.
	static std::map<std::string, int> constantMap;
	// Maps mnemonics onto corresponding write functions (i.e. for each instruction, gives instructions on how to
	// format it into machine code).
	static const std::map<std::string, std::function<void(Instruction&, const std::vector<std::string>&)> > 
		mnemonicToWriteFuncts;

};

#endif
