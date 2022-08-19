#include "Page437OutputScreen.h"

std::vector<sf::Texture> Page437OutputScreen::characters;

// Constructs a screen with a specified width and hight in characters. Also takes a path to an image
// containing all of the code page 437 characters. If we can't load our font, throw an exception.
Page437OutputScreen::Page437OutputScreen(unsigned int width, unsigned int height, const std::string &fontPath) {
	if (Page437OutputScreen::characters.size() == 0) {
		// Load our font.
		sf::Image fontImage;
		if (!fontImage.loadFromFile(fontPath)) {
			throw std::runtime_error("Issue trying to load code page Page437 image!");
		}

		// Break down image into textures, if we haven't already.
		for (unsigned int y = 0; y < FONT_IMAGE_HEIGHT; y++) {
			for (unsigned int x = 0; x < FONT_IMAGE_WIDTH; x++) {
				// Get the character "sub-image".
				sf::Image charImage;
				charImage.create(FONT_WIDTH, FONT_HEIGHT);
				for (unsigned int yPrime = 0; yPrime < FONT_HEIGHT; yPrime++) {
					for (unsigned int xPrime = 0; xPrime < FONT_WIDTH; xPrime++) {
						charImage.setPixel(xPrime, yPrime, fontImage.getPixel((x * FONT_WIDTH) + xPrime, (y * FONT_HEIGHT) + yPrime));
					}
				}

				// Place the sub image into the texture vector (i.e. load it into the GPU).
				sf::Texture charTexture;
				charTexture.loadFromImage(charImage);
				Page437OutputScreen::characters.push_back(charTexture);
			}
		}
	}

	// Create our screen buffer, and set our sprite positions.
	this->screenBuffer.resize(height);
	for (unsigned int y = 0; y < this->screenBuffer.size(); y++) {
		this->screenBuffer[y].resize(width);
		for (unsigned int x = 0; x < this->screenBuffer[y].size(); x++) {
			this->screenBuffer[y][x].setTexture(Page437OutputScreen::characters[0]);
			sf::FloatRect spriteBounds = this->screenBuffer[y][x].getGlobalBounds();
			this->screenBuffer[y][x].setPosition(spriteBounds.width * (float)x, spriteBounds.height * (float)y);	
		}
	}
}

// Set a character in the screen at (x, y) to the specified character, as defined in code page 437.
// If we go out of bounds, throw exception. Requires that the character be defined in code page 437.
void Page437OutputScreen::setChar(unsigned int x, unsigned int y, char c, const sf::Color &color) {
	// Check if we're in bounds, else throw exception. Unsigned, so only need to check greater than.
	if (x >= this->screenBuffer[y].size() || y >= this->screenBuffer.size()) {
		throw std::runtime_error("Character out of range of the screen!");
	}

	// Actually write character texture.
	this->screenBuffer[y][x].setTexture(Page437OutputScreen::characters[c]);
	this->screenBuffer[y][x].setColor(color);
}

// Draw a string to the screen with the speciifed length. If this drawing will result in going out of bounds,
// throw exception.
void Page437OutputScreen::drawStringHoriz(unsigned int x, unsigned int y, unsigned int length, const std::string &s, const sf::Color &color) {
	try {
		for (unsigned int i = 0; i < s.size(); i++) {
			this->setChar(x + i, y, s[i], color);
		}
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Draw a string to the screen with the speciifed length. If this drawing will result in gojng out of bounds,
// throw exception.
void Page437OutputScreen::drawStringVert(unsigned int x, unsigned int y, unsigned int length, const std::string &s, const sf::Color &color) {
	try {
		for (unsigned int i = 0; i < s.size(); i++) {
			this->setChar(x, y + i, s[i], color);
		}
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
}

// Flush screen buffer with blank chars.
void Page437OutputScreen::flushScreenBuffer() {
	for (unsigned int y = 0; y < this->screenBuffer.size(); y++) {
		for (unsigned int x = 0; x < this->screenBuffer.size(); x++) {
			this->setChar(x, y, ' ', sf::Color::White);
		}
	}
}

// Draw the screen buffer vector to the SFML window.
void Page437OutputScreen::updateWindow(sf::RenderWindow &window) {
	// Clear out old window.
	window.clear();
	
	for (unsigned int y = 0; y < this->screenBuffer.size(); y++) {
		for (unsigned int x = 0; x < this->screenBuffer.size(); x++) {
			window.draw(this->screenBuffer[y][x]);
		}
	}

	// Display new window.
	window.display();
}
