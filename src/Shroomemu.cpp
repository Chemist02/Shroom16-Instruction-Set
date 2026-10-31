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

#include <SFML/Graphics.hpp>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

#include "Processor.h"
#include "Page437OutputScreen.h"
#include "PlatformSpecificAPIs.h"

#define BACKSPACE 8

#define LEFTHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
#define RIGHTHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)
#define UPHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)
#define DOWNHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)
#define ZHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)
#define XHELD sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)

std::mutex processorMutex;
bool doForceNextInstructionWhileBroken = false;
bool doForceKillCommandThread = false;

void forceKillCommandThread() {
	doForceKillCommandThread = true;
	closeStdIn();
}

void runNoGUI(float minTimeBetweenInstructions) {
}

void drawNormalViewLabels(Page437OutputScreen &screen) {
	screen.drawStringHoriz(0u, 0u, 16u, "CHARACTER-OUT---", sf::Color::Green);

	if (!Processor::isWaitingForInput()) {
		screen.drawStringHoriz(17u, 0u, 6u, "NUM-IN", sf::Color::Green);
	}
	else {
		screen.drawStringHoriz(17u, 0u, 6u, "NUM-IN", sf::Color::Blue);
	}

	screen.drawStringHoriz(24u, 0u, 6u, "NUMOUT", sf::Color::Green);
	screen.drawStringHoriz(31u, 0u, 6u, "KEYPAD", sf::Color::Green);
	screen.drawStringHoriz(38u, 0u, 5u, "PAG-1", sf::Color::Green);

	for (unsigned int i = 0; i <= 33; i++) {
		screen.setChar(i, 3u, CHAR_FULL + 1, sf::Color::Green);
		screen.setChar(i, 36u, CHAR_FULL + 4, sf::Color::Green);
		if (i < 33 && i > 0) {
			screen.setChar(0u, 3u + i, CHAR_FULL + 2, sf::Color::Green);
			screen.setChar(33u, 3u + i, CHAR_FULL + 3, sf::Color::Green);
		}
	}

	screen.drawStringHoriz(10u, 3u, 14u, "|PIXEL-SCREEN|", sf::Color::Green);
	screen.drawStringHoriz(34u, 3u, 9u, "REGISTERS", sf::Color::Green);
	screen.drawStringHoriz(34u, 4u, 3u, "$00", sf::Color::Magenta);
	screen.drawStringHoriz(34u, 5u, 3u, "$fp", sf::Color::Magenta);
	screen.drawStringHoriz(34u, 6u, 3u, "$sp", sf::Color::Magenta);
	screen.drawStringHoriz(34u, 7u, 3u, "$ca", sf::Color::Magenta);
	for (unsigned int i = 0; i <= 26; i++) {	
		screen.drawStringHoriz(34u, 8u + i, 4u, "$g" + std::to_string(i) + " ", sf::Color::Magenta);
	}
	screen.drawStringHoriz(34u, 35u, 3u, "$wr", sf::Color::Magenta);

	screen.drawStringHoriz(0u, 38u, 3u, "PC-", sf::Color::Green);
	screen.drawStringHoriz(4u, 38u, 11u, "INSTRUCTION", sf::Color::Green);
}

void drawNormalModeData(Page437OutputScreen &screen) {
	// Character display.	
	screen.drawStringHoriz(0u, 1u, 16u, Processor::getCharDisplay(), sf::Color::Yellow);
	// Number out display.
	screen.drawStringHoriz(24u, 1u, 6u, std::to_string(Processor::getCurrentOutputNumber()), sf::Color::Yellow);
	// Pixel screen.
	for (unsigned int y = 0; y < SCREEN_HEIGHT; y++) {
		for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
			unsigned char charToSetTo = PixelScreen::getPixelState(y, x) ? CHAR_FULL : ' ';
			screen.setChar(1u + x, 4u + y, charToSetTo, sf::Color::Yellow);
		}
	}

	// Key in display.
	WORD keyInState = Processor::getCurrentKeypadState();
	screen.setChar(31u, 1u, 27u, sf::Color::Cyan);
	screen.setChar(32u, 1u, 26u, sf::Color::Cyan);
	screen.setChar(33u, 1u, 24u, sf::Color::Cyan);
	screen.setChar(34u, 1u, 25u, sf::Color::Cyan);
	screen.setChar(35u, 1u, 'Z', sf::Color::Cyan);
	screen.setChar(36u, 1u, 'X', sf::Color::Cyan);

	switch (keyInState) {
		case 0b0000000000000001:
			screen.setChar(31u, 1u, 27u, sf::Color::Yellow);
			break;
		case 0b0000000000000010:
			screen.setChar(32u, 1u, 26u, sf::Color::Yellow);
			break;
		case 0b0000000000000100:
			screen.setChar(33u, 1u, 24u, sf::Color::Yellow);
			break;
		case 0b0000000000001000:
			screen.setChar(34u, 1u, 25u, sf::Color::Yellow);
			break;
		case 0b0000000000010000:
			screen.setChar(33u, 1u, 24u, sf::Color::Yellow);
			screen.setChar(32u, 1u, 26u, sf::Color::Yellow);
			break;
		case 0b0000000000100000:
			screen.setChar(33u, 1u, 24u, sf::Color::Yellow);
			screen.setChar(31u, 1u, 27u, sf::Color::Yellow);
			break;
		case 0b0000000001000000:
			screen.setChar(34u, 1u, 25u, sf::Color::Yellow);
			screen.setChar(32u, 1u, 26u, sf::Color::Yellow);
			break;
		case 0b0000000010000000:
			screen.setChar(34u, 1u, 25u, sf::Color::Yellow);
			screen.setChar(31u, 1u, 27u, sf::Color::Yellow);
			break;
	}

	if (keyInState & 0b0000000100000000) {
		screen.setChar(35u, 1u, 'Z', sf::Color::Yellow);	
	}

	if (keyInState & 0b0000001000000000) {
		screen.setChar(36u, 1u, 'X', sf::Color::Yellow);	
	}

	// Register values.
	for (unsigned int i = 0; i < NUMBER_OF_REGISTERS; i++) {
		// Find hex equivalent.
		std::ostringstream hexStream;
		hexStream << std::hex << RegisterFile::read(i);
		screen.drawStringHoriz(39u, 4u + i, 4u, hexStream.str(), sf::Color::Yellow);
	}

	// Program counter.
	std::ostringstream hexStream;
	hexStream << std::hex << Processor::getProgramCounter();
	screen.drawStringHoriz(0u, 39u, 3u, hexStream.str(), sf::Color::Yellow);
	screen.drawStringHoriz(4u, 39u, 32u, Processor::getNextInstruction().formattedAsString(), sf::Color::Yellow);
}


void drawDataViewLabels(Page437OutputScreen &screen) {
	screen.drawStringHoriz(0u, 0u, 4u, "DATA", sf::Color::Green);
	screen.drawStringHoriz(0u, 1u, 11u, "MEMORY-VIEW", sf::Color::Green);
	screen.drawStringHoriz(38u, 0u, 5u, "PAG-2", sf::Color::Green);
	for (unsigned int i = 0; i <= 7; i++) {
		screen.setChar(3u + i * 5, 3u, (char)i + 48, sf::Color::Green);
	}
	for (unsigned int i = 0; i < 32; i++) {	
		((i & 0x1) == 0) ? screen.setChar(1u, 4u + i, '0', sf::Color::Green) : screen.setChar(1u, 4u + i, '8', sf::Color::Green);
	}

	for (unsigned int i = 0; i <= 15; i++) {
		std::ostringstream hexStream;
		hexStream << std::hex << i;
		screen.drawStringVert(0u, 4u + i * 2, 2u, hexStream.str() + hexStream.str(), sf::Color::Green);
	}
	screen.drawStringHoriz(0u, 38u, 3u, "PC-", sf::Color::Green);
	screen.drawStringHoriz(4u, 38u, 11u, "INSTRUCTION", sf::Color::Green);
}

void drawDataModeData(Page437OutputScreen &screen) {
	// Draw data memory contents.
	for (unsigned int i = 0; i < DATA_MEMORY_SIZE; i++) {
		unsigned int row = (unsigned int)(((float)i / (float)DATA_MEMORY_SIZE) * 32.0f);
		unsigned int col = i % 8u;
		std::ostringstream hexStream;
		hexStream << std::hex << DataMemory::getWord((WORD)i);
		screen.drawStringHoriz(3u + col * 5, 4u + row, 4u, hexStream.str(), sf::Color::Yellow);
	}

	// Draw program counter.
	std::ostringstream hexStream;
	hexStream << std::hex << Processor::getProgramCounter();
	screen.drawStringHoriz(0u, 39u, 3u, hexStream.str(), sf::Color::Yellow);
	screen.drawStringHoriz(4u, 39u, 32u, Processor::getNextInstruction().formattedAsString(), sf::Color::Yellow);
}

WORD getKeypadState() {
	WORD state = 0b0;
	if (LEFTHELD) {
		if (UPHELD) {
			state = 0b0000000000100000;
		}
		else if (DOWNHELD) {
			state = 0b0000000010000000;
		}
		else if (!RIGHTHELD) {
			state = 0b0000000000000001;
		}
	}
	else if (RIGHTHELD) {
		if (UPHELD) {
			state = 0b0000000000010000;
		}
		else if (DOWNHELD) {
			state = 0b0000000001000000;
		}
		else if (!LEFTHELD) {
			state = 0b0000000000000010;
		}
	}
	else if (UPHELD && !DOWNHELD) {
		state = 0b0000000000000100;
	}
	else if (DOWNHELD && !UPHELD) {
		state = 0b0000000000001000;	
	}

	if (ZHELD) {
		state |= 0b0000000100000000;
	}

	if (XHELD) {
		state |= 0b0000001000000000;
	}
	return state;
}

void runGUI(float minTimeBetweenInstructions) {
	// Create window.
	sf::RenderWindow window(sf::VideoMode({516u, 516u}), "Shroom16 Emulator");
	// Create output screen to storee characters.
	Page437OutputScreen screen(43u, 43u, "assets/font.png");
	// 0 iff we're in normal view, 1 iff we're in memory view.
	unsigned int currentPageState = 0u;
	// Clock to keep track of how many second sprogram has been running.
	sf::Clock clock;
	// Time of last instruction execution.
	sf::Time timeOfLastInstructionExecution = clock.getElapsedTime();
	// Current num in text.
	std::string numInText = "";
	while (window.isOpen()) {

		// Check for events.
		while (const std::optional event = window.pollEvent()) {
			// Close window if 'X' is pressed.
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			
			// Check for key presses.
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				std::lock_guard<std::mutex> lock(processorMutex);
				// Handle page switching.
				if (keyPressed->scancode == sf::Keyboard::Scancode::Num1) {
					currentPageState = 0u;
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2 && !Processor::isWaitingForInput()) {
					currentPageState = 1u;
				}

				// Handle number input.
				if (keyPressed->scancode == sf::Keyboard::Scancode::Enter && Processor::isWaitingForInput()) {
					Processor::inputNumber(numInText);
					numInText = "";

				}
			}

			// Check for text input.
			const auto* textEntered = event->getIf<sf::Event::TextEntered>();
			{
				std::lock_guard<std::mutex> lock(processorMutex);
				if (textEntered && currentPageState == 0u && Processor::isWaitingForInput() && !Processor::getIsBroken()) {
					if (textEntered->unicode == BACKSPACE && numInText.size() > 0) {
						numInText.pop_back();
					}
					else if (textEntered->unicode >= 32 && textEntered->unicode < 128) {
						numInText += static_cast<char>(textEntered->unicode);
					}
				}
			}
		}

		// Handle keypad input.
		{
		std::lock_guard<std::mutex> lock(processorMutex);
		Processor::setCurrentKeypadState(getKeypadState());
		}

		// Draw input string.
		if (currentPageState == 0u) {
			screen.drawStringHoriz(17u, 1u, 6u, numInText, sf::Color::Yellow);
		}

		// Draw current labels and data to screen.
		switch(currentPageState) {
			// Default view.
			case 0u:
				// Draw data labels.
				drawNormalViewLabels(screen);
				// Now add in the actual data.
				drawNormalModeData(screen);
				break;
			case 1u:				
				// Draw data labels.
				drawDataViewLabels(screen);
				// Now add in the actual data.
				drawDataModeData(screen);
				break;
			default:
				exit(-1);
				break;
		}

		// Execute the next task.
		if ((clock.getElapsedTime() - timeOfLastInstructionExecution).asSeconds() >= minTimeBetweenInstructions) {
			std::lock_guard<std::mutex> lock(processorMutex);
			if (Processor::getIsBroken() && doForceNextInstructionWhileBroken) {
				Processor::setIsBroken(false);
			}
			Processor::runNextTask();
			if (doForceNextInstructionWhileBroken) {
				Processor::setIsBroken(true);
				doForceNextInstructionWhileBroken = false;
			}
			timeOfLastInstructionExecution = clock.getElapsedTime();
		}

		// Update the window with the current screen buffer.
		screen.updateWindow(window);
		// Clear out old screen buffer.
		screen.flushScreenBuffer();
		std::this_thread::yield();
	}

	forceKillCommandThread();

}

// Debugger that runs on its own thread getting input from stdin.
void runCommandLoop() {
	std::string rawInput;
	std::cout << "Shroomemu command console (type 'help' for command list):" << std::endl;
	while (true) {
		if (doForceKillCommandThread) {
			return;
		}

		std::cout << ">> ";
		std::getline(std::cin, rawInput);

		if (doForceKillCommandThread) {
			return;
		}

		std::vector<std::string> command;
		std::stringstream commandStream(rawInput);
		std::string fragment;
		while (commandStream >> fragment) {
			command.push_back(fragment);
		}

		if (command.size() == 0) {
			continue;
		}

		std::lock_guard<std::mutex> lock(processorMutex);
		if (command[0] == "help") {
		
		}
		else if (command[0] == "break") {
			if (command.size() != 2) {
				std::cout << "'break' expects exactly one argument" << std::endl;
				continue;
			}

			try {
				Processor::addBreakpoint(std::stoi(command[1]));
			}
			catch (std::exception &e) { 
				std::cerr << "Error: break command expects single line number." << std::endl;
				continue;
			}
		}
		else if (command[0] == "next") {
			if (command.size() != 1) {
				std::cout << "'next' expects no arguments" << std::endl;
				continue;
			}
			doForceNextInstructionWhileBroken = true;
		}
		else if (command[0] == "pause") {
			if (command.size() != 1) {
				std::cout << "'pause' expects no arguments" << std::endl;
				continue;
			}
			Processor::setIsBroken(true);
		}
		else if (command[0] == "continue") {
			if (command.size() != 1) {
				std::cout << "'continue' expects no arguments" << std::endl;
				continue;
			}
			Processor::setIsBroken(false);
		}
		else if (command[0] == "rewind") {
			if (command.size() != 1) {
				std::cout << "'rewind' expects no arguments" << std::endl;
				continue;
			}
			Processor::setIsBroken(true);
			Processor::resetProgramCounter();
		}
		else if (command[0] == "delete") {
			if (command.size() != 2) {
				std::cout << "'delete' expects exactly one argument" << std::endl;
				continue;
			}
		
			try {
				Processor::deleteBreakpoint(std::stoi(command[1]));
			}
			catch (std::exception &e) { 
				std::cerr << "Error: delete command expects single line number." << std::endl;
				continue;
			}
		}
		else {
			std::cout << "Unknown command " << command[0] << "; type 'help' for list of commands." << std::endl;
		}
	}
}

int main(int argc, char *argv[]) {
	// Ensure we were given at least an input file as an argument, if not end program.
	std::string usageMessage = " <input program> <optional arguments>\nOptional arguments:\n -t <time>       "
		"Specify minimum time between instructions (in seconds).\n -n              Run in no-gui mode.\n"
		" -b              Break immediately upon starting program.";
	if (argc < 2) {
		std::cerr << "Error: invalid number of arguments!\nUsage: " << argv[0] << usageMessage << std::endl;
		return -1;
	}

	// Check if we're in nogui mode, or if a min time between instructions was given.
	// No GUI mode disables the display window such that the only thing shown are register values output with the
	// ?out interrupt, straight into stdout.
	bool noGUIMode = false;
	float minTimeBetweenInstructions = 0.0;
	for (int i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "-n")) {
			noGUIMode = true;
		}
		else if (!strcmp(argv[i], "-b")) {
			std::cout << "breaking" << std::endl;
			Processor::setIsBroken(true);
		}
		else if (!strcmp(argv[i], "-t")) {
			try {
				if (argc - 1 > i) {
					minTimeBetweenInstructions = std::stof(std::string(argv[i + 1]));
				}
				else {
					throw std::invalid_argument("Not enough arguments!");
				}
			}
			catch (std::exception &e) { 
				std::cerr << "Error: -t flag expects floating point time value.\nUsage: " << argv[0] 
					<< usageMessage << std::endl;
				return -1;
			}
		}
		else if (strcmp(argv[i - 1], "-t")) {
			std::cerr << "Error: unknown flag " << argv[i] << "\nUsage: " << argv[0] << usageMessage 
				<< std::endl;
			return -1;
		}
	}

	// Try to open machine code file.
	std::ifstream codeFile(argv[1], std::ios::in|std::ios::binary);
	// Make sure file was properly opened.
	if (!codeFile.good()) {
		std::cerr << "Error: invalid input file " << argv[1] << "\nUsage: " << argv[0] << usageMessage 
			<< std::endl;
		return -1;
	}

	// Now that we know we have a good file, load instructions into instruction memory.
	Processor::loadMachineCode(codeFile);

	std::thread commandThread(runCommandLoop);

	// Actually run program depending on settings.
	if (!noGUIMode) {
		runGUI(minTimeBetweenInstructions);
	}
	else {
		runNoGUI(minTimeBetweenInstructions);
	}
	commandThread.join();

	return 0;
}
