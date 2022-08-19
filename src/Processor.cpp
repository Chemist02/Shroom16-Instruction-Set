#include "Processor.h"

// Vector of binary instructions.
std::vector<Instruction> Processor::instructionMemory(INSTRUCTION_MEMORY_SIZE);

// Our current location in instruction memory.
WORD Processor::programCounter = 0;

// True iff an ?in interrupt was used.
bool Processor::waitingForInput = false;

// Number we're currently outputting to the screen.
WORD Processor::currentOutputNumber = 0;

// Current state of the keyboard.
WORD Processor::currentKeypadState = 0;

// Array of letters in the character out display.
std::string Processor::charDisplay = "                ";

// Maps 6 bit opcodes into instruction functions.

const std::map<unsigned int, std::function<void(BYTE, BYTE, BYTE, BYTE, WORD, WORD)> > 
Processor::opcodeToInstructionMap = {
	{0b000000, Processor::ADD},
	{0b000001, Processor::SUB},
	{0b000010, Processor::MUL},
	{0b000011, Processor::DIV},
	{0b000100, Processor::SLL},
	{0b000101, Processor::SRL},
	{0b000110, Processor::NOR},
	{0b000111, Processor::OR},
	{0b001000, Processor::AND},
	{0b001001, Processor::XOR},
	{0b001010, Processor::LW},
	{0b001011, Processor::SW},
	{0b001100, Processor::ADDI},
	{0b001101, Processor::SLLI},
	{0b001110, Processor::SRLI},
	{0b001111, Processor::NORI},
	{0b010000, Processor::ORI},
	{0b010001, Processor::ANDI},
	{0b010010, Processor::XORI},
	{0b010011, Processor::CMP},
	{0b010100, Processor::JMP},
	{0b010101, Processor::JEQ},
	{0b010110, Processor::JLT},
	{0b010111, Processor::JGT},
	{0b011000, Processor::CALL},
	{0b011001, Processor::JR},
	{0b011010, Processor::RANDOM},
	{0b011011, Processor::IN},
	{0b011100, Processor::OUT},
	{0b011101, Processor::END},
	{0b011110, Processor::CHARSET},
	{0b011111, Processor::KEYIN},
	{0b100000, Processor::PXSET},
	{0b100001, Processor::CLRSCRN}
};

// Seed our random number generator with the current number of seconds since the Unix epoch.
std::mt19937 Processor::mt(time(nullptr));

// Run the next processor task. Usually this is the next instruction as pointed to by the program counter, but
// if there's an interrupt in progress, other tasks are also possible (e.g. waiting for a number to be 
// entered).
void Processor::runNextTask() {
	// If we're waiting for input, don't do anything.
	if (waitingForInput) {
		return;
	}

	// Load our next instruction.
	Instruction toExecute = Processor::instructionMemory[programCounter];
	
	// Load the opcode from the instruction.
	unsigned int opcode = toExecute.getBitsInRange(0, 5);

	// Actually execute our instruction.
	Processor::opcodeToInstructionMap.find(opcode)->second(
		toExecute.getBitsInRange(6, 10),		
		toExecute.getBitsInRange(11, 15),		
		toExecute.getBitsInRange(16, 20),		
		toExecute.getBitsInRange(21, 28),		
		toExecute.getBitsInRange(16, 31),		
		toExecute.getBitsInRange(16, 24)		
	);	
}

// Load machine code from an assembled source into instruction memory. Requires the input file to be a valid
// file opened for binary reading.
void Processor::loadMachineCode(std::ifstream &codeFile) {
	BYTE nextByte;
	unsigned int byteCounter = 0u;
	Instruction instructionSoFar;
	while (codeFile >> nextByte) {
		instructionSoFar.setBitsInRange(byteCounter * 8u, byteCounter * 8u + 8u, (unsigned int)nextByte);
		byteCounter++;

		// If we're done reading this instruction, write and reset.
		if (byteCounter >= 4) {	
			Processor::instructionMemory.push_back(instructionSoFar);
			instructionSoFar.setBitsInRange(0u, 31u, 0u);
			byteCounter = 0u;
		}
	}
}

// Functions to execute each of the instructions. Each takes a destination register id, two read registers a 
// and b, a memory address offset for lw and sw, an immediate value, and a label to jump to. Most of the time
// these parameters are not all needed so many are left blank. 
void Processor::ADD(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) + RegisterFile::read(rreadb));
}

void Processor::SUB(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) - RegisterFile::read(rreadb));
}

void Processor::MUL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	int result = (int)RegisterFile::read(rreada) * (int)RegisterFile::read(rreadb);
	RegisterFile::write(rdest, (WORD)result);
	RegisterFile::write(WR, (WORD)result);
}

void Processor::DIV(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, (WORD)(RegisterFile::read(rreada) / RegisterFile::read(rreadb)));
	RegisterFile::write(WR, RegisterFile::read(rreada) % RegisterFile::read(rreadb));
}

void Processor::SLL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) << RegisterFile::read(rreadb));
}

void Processor::SRL(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) >> RegisterFile::read(rreadb));
}

void Processor::NOR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, ~(RegisterFile::read(rreada) | RegisterFile::read(rreadb)));
}

void Processor::OR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) | RegisterFile::read(rreadb));
}

void Processor::AND(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) & RegisterFile::read(rreadb));
}

void Processor::XOR(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) ^ RegisterFile::read(rreadb));
}

void Processor::LW(BYTE rdest, BYTE rreada, BYTE, BYTE offset, WORD, WORD) {
	DataMemory::readToRegister(rdest, rreada, offset);
}

void Processor::SW(BYTE, BYTE rreada, BYTE rreadb, BYTE offset, WORD, WORD) {
	DataMemory::writeFromRegister(rreadb, rreada, offset);
}

void Processor::ADDI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) + immed);
}

void Processor::SLLI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) << immed);
}

void Processor::SRLI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) >> immed);
}

void Processor::NORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, ~(RegisterFile::read(rreada) | immed));
}

void Processor::ORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) | immed);
}

void Processor::ANDI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) & immed);
}

void Processor::XORI(BYTE rdest, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	RegisterFile::write(rdest, RegisterFile::read(rreada) ^ immed);
}

void Processor::CMP(BYTE rdest, BYTE rreada, BYTE rreadb, BYTE, WORD, WORD) {
	WORD numA = RegisterFile::read(rreada);
	WORD numB = RegisterFile::read(rreadb);
	WORD result = 0b0;
	// By the trichotmony axiom, else case must be numA > numB.
	if (numA == numB) {
		result = 0b001;
	}
	else if (numA < numB) {
		result = 0b010;
	}
	else {
		result = 0b100;
	}
	RegisterFile::write(rdest, result);
}

void Processor::JMP(BYTE, BYTE, BYTE, BYTE, WORD, WORD label) {
	Processor::programCounter = label;
}

void Processor::JEQ(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label) {
	if (RegisterFile::read(rreada) == 0b001) {
		Processor::programCounter = label;
	}
}

void Processor::JLT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label) {
	if (RegisterFile::read(rreada) == 0b010) {
		Processor::programCounter = label;
	}
}

void Processor::JGT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD label) {
	if (RegisterFile::read(rreada) == 0b100) {
		Processor::programCounter = label;
	}
}

void Processor::CALL(BYTE, BYTE, BYTE, BYTE, WORD, WORD label) {
	RegisterFile::write(CA, programCounter + 1);
	Processor::programCounter = label;
}

void Processor::JR(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD) {
	Processor::programCounter = RegisterFile::read(rreada);
}

void Processor::RANDOM(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, (WORD)Processor::mt());
}

void Processor::IN(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD) {
	Processor::waitingForInput = true;
}

void Processor::OUT(BYTE, BYTE rreada, BYTE, BYTE, WORD, WORD) {
	Processor::currentOutputNumber = RegisterFile::read(rreada);
}

void Processor::END(BYTE, BYTE, BYTE, BYTE, WORD, WORD) {
	exit(0);
}

void Processor::CHARSET(BYTE, BYTE rreada, BYTE, BYTE, WORD immed, WORD) {
	// If index is out of range, throw exception.
	WORD index = RegisterFile::read(rreada);
	if (index < 0 || index > 15) {
		throw std::runtime_error("Character display index out of range!");
	}

	Processor::charDisplay[index] = immed;
}

void Processor::KEYIN(BYTE rdest, BYTE, BYTE, BYTE, WORD, WORD) {
	RegisterFile::write(rdest, Processor::currentKeypadState);
}

void Processor::PXSET(BYTE, BYTE rreada, BYTE rreadb, BYTE, WORD immed, WORD) {
	PixelScreen::setPixel(RegisterFile::read(rreada), RegisterFile::read(rreadb), (bool)immed);
}

void Processor::CLRSCRN(BYTE, BYTE, BYTE, BYTE, WORD, WORD) {
	for (unsigned int y = 0; y < SCREEN_HEIGHT; y++) {
		for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
			PixelScreen::setPixel(x, y, false);
		}
	}
}
