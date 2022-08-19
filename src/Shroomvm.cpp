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
#include "Processor.h"
#include "Page437OutputScreen.h"

void runNoGUI(bool doStepMode, float minTimeBetweenInstructions) {

}

void drawNormalViewLabels(Page437OutputScreen &screen) {
	screen.drawStringHoriz(0u, 0u, 13u, "CHARACTER-OUT", sf::Color::Green);
	screen.drawStringHoriz(16u, 0u, 6u, "NUM-IN", sf::Color::Green);
	screen.drawStringHoriz(23u, 0u, 6u, "NUMOUT", sf::Color::Green);
	screen.drawStringHoriz(30u, 0u, 6u, "KEYPAD", sf::Color::Green);
	screen.drawStringHoriz(37u, 0u, 5u, "PRESS", sf::Color::Green);
	screen.drawStringHoriz(0u, 3u, 12u, "PIXEL-SCREEN", sf::Color::Green);
	screen.drawStringHoriz(33u, 3u, 9u, "REGISTERS", sf::Color::Green);
}

void runGUI(bool doStepMode, float minTimeBetweenInstructions) {
	// Create window.
	sf::RenderWindow window(sf::VideoMode(516u, 468u), "Shroom16 Virtual Machine");
	// Create output screen to storee characters.
	Page437OutputScreen screen(43u, 39u, "assets/font.png");
	// 0 iff we're in normal view, 1 iff we're in memory view.
	unsigned int currentPageState = 0u;
	while (window.isOpen()) {
		// Check for events.
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// Draw current labels and data to screen.
		switch(currentPageState) {
			// Default view.
			case 0u:
				// Draw data labels.
				drawNormalViewLabels(screen);
			break;
			case 1u:
			break;
			default:
				exit(-1);
			break;
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
	std::ifstream codeFile(argv[1], std::ios::binary);
	// Make sure file was properly opened.
	if (!codeFile.good()) {
		std::cerr << "Error: invalid input file " << argv[1] << "\nUsage: " << argv[0] << usageMessage 
			<< std::endl;
		return -1;
	}

	// Now that we know we have a good file, load instructions into instruction memory.
	//Processor::loadMachineCode(codeFile);

	// Actually run program depending on settings.
	if (!noGUIMode) {
		runGUI(stepMode, minTimeBetweenInstructions);
	}
	else {
		runNoGUI(stepMode, minTimeBetweenInstructions);
	}

	return 0;
}
