#include <SFML/Graphics.hpp>
#include <string>
#include <stdexcept>

#define FONT_WIDTH 12u
#define FONT_HEIGHT 12u
#define FONT_IMAGE_WIDTH 16u
#define FONT_IMAGE_HEIGHT 16u
#define CHAR_FULL 219u

#ifndef OUTPUTSCREEN_H
#define OUTPUTSCREEN_H

// Class that represents an output screen that outputs using code page 437 characters. This screen object has
// a specified width in height (in characters). When a character is drawn, this drawing does not take place immediately. 
// Rather, it is written to a buffer in this object, and then drawn to the SFML window when updateWindow() is called. 
// The top left corner of the screen in defined as (0, 0), and y values increase from top to bottom and x values 
// increase from left to right.
class Page437OutputScreen {
public:
	// Constructs a screen with a specified width and hight in characters. Also takes a path to an image
	// containing all of the code page 437 characters. If an object of this type has already been created, throw
	// an exception.
	Page437OutputScreen(unsigned int width, unsigned int height, const std::string &fontPath);
	
	// Set a character in the screen at (x, y) to the specified character, as defined in code page Page437.
	// If we go out of bounds, throw exception. Requires that the character be defined in code age Page437.
	void setChar(unsigned int x, unsigned int y, unsigned char c, const sf::Color &color);
	// Draw a string to the screen with the speciifed length. If this drawing will result in gojng out of bounds,
	// throw exception.
	void drawStringHoriz(unsigned int x, unsigned int y, unsigned int length, const std::string &s, const sf::Color &color);
	// Draw a string to the screen with the speciifed length. If this drawing will result in gojng out of bounds,
	// throw exception.
	void drawStringVert(unsigned int x, unsigned int y, unsigned int length, const std::string &s, const sf::Color &color);
	// Flush screen buffer with blacks chars.
	void flushScreenBuffer();

	// Draw the screen buffer vector to a SFML window.
	void updateWindow(sf::RenderWindow &window);
private:
	// Vector of all of the code page 437 character texture from the source image.
	static std::vector<sf::Texture> characters;
	// Vector storing all of the sprites making up the screen. Is not actually displayed until updateWindow() is 
	// called.
	std::vector<std::vector<sf::Sprite>> screenBuffer;
};

#endif
