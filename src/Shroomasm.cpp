/*

  ^
   
  ...    ^
 ;   `,  ....
;       /     `.
;  ^-^ ;  ^o^   ;  HOWDY FRIEND!
 ; . . .; . . .    WE LOVE YOU VERY MUSH.
    ; ;    ; ;     PLEASE MAKE YOURSELF AT HOME;
     ; ;  / /      MYCELIUM IS YOURCELIUM.
     ; ; ; ;
     ; ;/  ;
 -^------^^---*-

*/

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <functional>
#include <string.h>
#include "Parser.h"
#include "Instruction.h"
#include "InstructionWriter.h"
#include "zlib.h"
#include "IMemSchemConstants.h"

// Passes over source code file recording constants and labels as they come up into their respective InstructionWriter
// static maps. This can be thought of the first pass over the source code. Takes in a ifstream referring to the source
// code file.
void findConstantsAndLabels(std::ifstream &sourceFile) {
	// Keeps track of which instruction we're currently on, starting at zero.
	unsigned int instructionNumber = 0;
	// Current line of file.
	std::string line;
	// Keeps track of line number.
	unsigned int lineNumber = 0;
	while (std::getline(sourceFile, line)) {
		// Increment line number.
		lineNumber++;

		// Remove comment from line.
		Parser::stripComment(line);

		// Parse line to examine its formatted components.
		std::vector<std::string> parsedLine;
		Parser::parseLine(line, parsedLine);

		// If line is empty, move on.
		if (parsedLine.size() == 0) {
			continue;
		}

		// Try to add label, if applicable.
		try {
			// If line is a label (labels start with ':'), add it along with current instruction number to
			// map, then move on.
			if (parsedLine[0][0] == ':') {
				InstructionWriter::defineLabel(parsedLine[0].substr(1, parsedLine[0].size() - 1), 
					instructionNumber);
				continue;
			}
		}
		catch (std::exception &e) {
			// If an excpetion was thrown as we we're trying to handle labels, print error message and 
			// halt.
			std::cerr << "Error on line " << lineNumber << ": " << e.what() << std::endl;
			exit(-1);
		}

		try {
			// If line is a directive, see if it's a constant definition, add to map if it is, then move on
			// either way.
			if (parsedLine[0][0] == ';') {
				if (parsedLine[0] == ";const") {
					// Check if complete constant definition was given, if not throw excpetion.
					if (parsedLine.size() != 3) {
						throw std::runtime_error("Invalid number of directive arguments.");
					}

					// Attempt to define constant.
					InstructionWriter::defineConstant(parsedLine[1], parsedLine[2]);
				}
				else {
					// Otherwise, we have an undefined directive.
					throw std::runtime_error("Unknown directive " + parsedLine[0] + ".");
				}
				continue;
			}
		}
		catch (std::exception &e) {
			// If an excpetion was thrown as we we're trying to handle constants, print error message and
			// halt.
			std::cerr << "Directive error on line " << lineNumber << ": " << e.what() << std::endl;
			exit(-1);
		}

		// Otherwise, line is a instruction that will make it to instruction memory, and we need to increment 
		// our position.
		instructionNumber++;
	}

	// Reset source code file stream.
	sourceFile.clear();
	sourceFile.seekg(0);
}

// Write machine code data to gzip file data buffer one byte at a time (from the zlib library) then, if all goes 
// according to plan, actually write the data buffer to the file. Requires that the outFile is a valid zlib gzip file.
// Throws an excpetion if byte of data could not be wirtten or if we cannot flush teh data buffer to the file.
void saveSchematic(const std::vector<Instruction> &machineCode, gzFile outFile) {
	// Write header of our schematic file, which sets everything up. This is the same for every program.
	for (BYTE byte : I_MEM_HEADER) {
		// Ensure one and only one (onee) byte is written to the file. If not throw an exception.
		if (gzwrite(outFile, (voidpc)&byte, 1u) != 1u) {	
			throw std::runtime_error("Issue writing machine code byte to gzip data buffer!");
		}
	}

	// Write the block data.
	// First grab the data from our header file.
	std::vector<BYTE> body(I_MEM_BODY);

	// Now, go through all of our instructions, modifying the block data as we go (in the body). I.e. for each bit,
	// either place a redstone torch if it's a one, or place air if it's zero.
	for (unsigned int i = 0; i < machineCode.size(); i++) {
		for (unsigned int b = 0; b < INSTRUCTION_SIZE; b++) {
			// Place a torch if bit is one, place air if bit is zero.
			body[BLOCK_DATA_INDEX_FROM_INSTR(i, b)] = 
				machineCode[i].getBitState(b) ? REDSTONE_TORCH_OFF : AIR;
		}
	}

	// Now actually write the block data body,
	for (BYTE byte : body) {
		// Ensure one and only one (onee) byte is written to the file. If not throw an exception.
		if (gzwrite(outFile, (voidpc)&byte, 1u) != 1u) {	
			throw std::runtime_error("Issue writing machine code byte to gzip data buffer!");
		}
	}

	// Write footer of our schematic file. This is the same for every file.
	for (BYTE byte : I_MEM_FOOTER) {
		// Ensure one and only one (onee) byte is written to the file. If not throw an exception.
		if (gzwrite(outFile, (voidpc)&byte, 1u) != 1u) {	
			throw std::runtime_error("Issue writing machine code byte to gzip data buffer!");
		}
	}
	
	// Actually flush data buffer to gzip file. Throw exception if we fail.
	if (gzflush(outFile, Z_FINISH) != Z_OK) {
		throw std::runtime_error("Issue writing gzip data buffer to file!");	
	}
}

int main(int argc, char *argv[]) {
	// Check proper command line argument format.
	std::string usagemessage = " <input file> <optional arguments>\nOptional arguments:\n -o <name>       "
		"Specify output file name.\n -g              Output a .schem file (Sponge ver. 3) to be pasted into "
		"in-game instruction memory (instead of a shroom16 binary file for use in the"
		" VM).\n -b              Output binary instructions to stdout before writing to a file (little "
		"endian).\n";
	// Check number of arguments.
	if (argc < 2) {
		std::cerr << "Error: please specify input file!\n" << "\nUsage: " << argv[0] << usagemessage;
		return -1;
	}
	
	// Attempt to open specified file.
	std::ifstream sourceFile(argv[1]);
	// Make sure input file is good.
	if (!sourceFile.good()) {
		std::cerr << "Error: invalid file " << argv[1] << "!\n" << "\nUsage: " << argv[0] << usagemessage;
		return -1;
	}

	// Name of output file.
	std::string outFileName = "out.shroombin";

	// true = output schematic file for use in game, false = output shroom16 binary file for use with vm.
	bool doOutputSchem = false;
	// true = output binary instructions to stdout before writing them to the file, false = don't do that.
	bool doDumpInstructions = false;
	// Check for flags.
	for (int i = 0; i < argc; i++) {
		// Returns 0 iff inputs are equal.
		// Check for -g flag.
		if (!strcmp(argv[i], "-g")) {
			doOutputSchem = true;
			if (outFileName == "out.shroombin") {
				outFileName = "out.schem";
			}
		}
		// Check for -o flag.
		else if (!strcmp(argv[i], "-o")) {
			// Make sure name was actually given.
			if (argc - 1 == i || !strcmp(argv[i + 1], "-g") || !strcmp(argv[i + 1], "-o")) {
				std::cerr << "Error: -o argument requires output file name!\n" << "\nUsage: " << argv[0]
					<< usagemessage;
				return -1;
			}
			else {
				// Set new outfile name.
				outFileName = argv[i + 1];
				break;
			}
		}
		// Check for binry output flag.
		else if (!strcmp(argv[i], "-b")) {
			doDumpInstructions = true;
		}
		// Check for invalid flags.
		else if (argv[i][0] == '-') {
			std::cerr << "Error: unknown flag " << argv[i] << "!\n" << "\nUsage: " << argv[0] 
				<< usagemessage;
			return -1;
		}
	}

	// First pass through file; find and define constants and labels.
	findConstantsAndLabels(sourceFile);

	// Second pass over file, actually translate insturctions into machine code.
	std::vector<Instruction> machineCode;
	// Keeps track of which instruction we're currently on, starting at zero.
	unsigned int instructionNumber = 0;
	// Current line of file.
	std::string line;
	// Keeps track of line number.
	unsigned int lineNumber = 0;
	// List of plaintext instructions which is filled if we do a binary dump.
	std::vector<std::string> instructionLines;
	while (getline(sourceFile, line)) {
		// Increment line number.
		lineNumber++;

		// If we've translated more instructions than will fit into instruction memory, print error message and 
		// halt program.
		if (instructionNumber >= INSTRUCTION_MEMORY_SIZE) {
			std::cerr << "Error: program too large! Max size is " << INSTRUCTION_MEMORY_SIZE << 
				" instructions!" << std::endl;
			exit(-1);
		}

		// Remove comment from line.
		Parser::stripComment(line);

		// Parse line to examine its formatted components.
		std::vector<std::string> parsedLine;
		Parser::parseLine(line, parsedLine);

		// If line is empty, a label, or a directive, move on.
		if (parsedLine.size() == 0) {
			continue;
		}
		if (parsedLine[0][0] == ':' || parsedLine[0][0] == ';') {
			continue;
		}

		// Attempt to translate line into machine code. If attempt fails, print error message and halt.
		Instruction translatedLine;
		try {
			InstructionWriter::writeInstruction(translatedLine, parsedLine);
		}
		catch (std::exception &e) {
			std::cerr << "Error on line " << lineNumber << ": " << e.what() << std::endl;
			exit(-1);
		}
		machineCode.push_back(translatedLine);
		if (doDumpInstructions) {
			instructionLines.push_back(line);
		}

		// If we made it this far, we know we're dealing with a instruction so we should increment.
		instructionNumber++;
	}

	// If we should dump our instructions to stdout, do it.
	if (doDumpInstructions) {
		for (unsigned int i = 0; i < machineCode.size(); i++) {
			std::cout << instructionLines[i] << ":    " << machineCode[i].formattedAsString() << std::endl;
		}
	}

	// Now that we have our machine code, check if we should make a shroom16 binary or a .schem for use in 
	// Minecraft.
	// Handle case for virtual machine.
	if (!doOutputSchem) {
		// Open output c++ file stream for creating assembled program for the virtual machine.
		std::ofstream outFile(outFileName, std::ios::binary);
		if (!outFile.good()) {
			std::cerr << "Error: Issue opening output file " << outFileName << "!\n";
			return -1;
		}

		for (Instruction instruction : machineCode) {
			instruction.writeToFile(outFile);
		}

		outFile.close();
	}
	// Handle case for in-game pasting.
	else {
		// Open output gzip file for creating schematics.	
		gzFile outFile = (gzFile)gzopen(outFileName.c_str(), "wb");
		// Make sure file was successfully opened.
		if (outFile == NULL) {
			std::cerr << "Error: Issue opening output file " << outFileName << "!\n";
			return -1;
		}

		// Fill used instructions with zeros.
		machineCode.resize(INSTRUCTION_MEMORY_SIZE);
		try {
			// Set up and write schematic file.
			saveSchematic(machineCode, outFile);
		}
		catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return -1;
		}
		// Close file handle to gzip file.
		gzclose(outFile);
	}

	// Close files.
	sourceFile.close();
	return 0;
}
