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
#include "Processor.h"
#include "Page437OutputScreen.h"

void runNoGUI(bool doStepMode, float minTimeBetweenInstructions) {

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
	screen.drawStringHoriz(35u, 1u, 6u, "ZX", sf::Color::Cyan);
	screen.setChar(31u, 1u, 27u, sf::Color::Cyan);
	screen.setChar(32u, 1u, 26u, sf::Color::Cyan);
	screen.setChar(33u, 1u, 24u, sf::Color::Cyan);
	screen.setChar(34u, 1u, 25u, sf::Color::Cyan);
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

void runGUI(bool doStepMode, float minTimeBetweenInstructions) {
	// Create window.
	sf::RenderWindow window(sf::VideoMode(516u, 516u), "Shroom16 Virtual Machine");
	// Create output screen to storee characters.
	Page437OutputScreen screen(43u, 43u, "assets/font.png");
	// 0 iff we're in normal view, 1 iff we're in memory view.
	unsigned int currentPageState = 0u;
	// Should we advance to the next step of the program, used to step mode.
	bool advanceToNextStep = false;
	while (window.isOpen()) {
		// Check for events.
		sf::Event event;
		while (window.pollEvent(event)) {
			// Close window if 'X' is pressed.
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			
			// Check for key presses.
			if (event.type == sf::Event::KeyPressed) {
				// Handle stepping.
				if (event.key.code == sf::Keyboard::Space) {
					advanceToNextStep = true;
				}

				// Handle page switching.
				if (event.key.code == sf::Keyboard::Num1) {
					currentPageState = 0u;
				}
				else if (event.key.code == sf::Keyboard::Num2) {
					currentPageState = 1u;
				}
			}
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
		if (doStepMode) {
			if (advanceToNextStep) {
				Processor::runNextTask();
				advanceToNextStep = false;
			}
		}
		else {
			Processor::runNextTask();
		}

		// Update the window with the current screen buffer.
		screen.updateWindow(window);
		// Clear out old screen buffer.
		screen.flushScreenBuffer();
	}

}

int main(int argc, char *argv[]) {
	// Ensure we were given at least an input file as an argument, if not end program.
	std::string usageMessage = " <input program> <optional arguments>\nOptional arguments:\n -t <time>       "
		"Specify minimum time between instructions.\n -n              Run in no-gui mode.\n"
		" -s              Run in step mode.";
	if (argc < 2) {
		std::cerr << "Error: invalid number of arguments!\nUsage: " << argv[0] << usageMessage << std::endl;
		return -1;
	}

	// Check if we're in step mode and/or nogui mode, or if a min time between instructions was given.
	// Step mode means that we wait for the enter key to be pressed between each instruction.
	bool stepMode = false;
	// No GUI mode disables the display window such that the only thing shown are register values output with the
	// ?out interrupt, straight into stdout.
	bool noGUIMode = false;
	float minTimeBetweenInstructions = 0.0;
	for (int i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "-s")) {
			stepMode = true;
		}
		else if (!strcmp(argv[i], "-n")) {
			noGUIMode = true;
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
		else {
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

	// Actually run program depending on settings.
	if (!noGUIMode) {
		runGUI(stepMode, minTimeBetweenInstructions);
	}
	else {
		runNoGUI(stepMode, minTimeBetweenInstructions);
	}

	return 0;
}
