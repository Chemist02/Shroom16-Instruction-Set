#include "PixelScreen.h"

std::vector<std::vector<bool>> PixelScreen::screen(SCREEN_HEIGHT, std::vector<bool>(SCREEN_WIDTH, false));

void PixelScreen::setPixel(unsigned int x, unsigned int y, bool state) {
	if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT) {
		throw std::runtime_error("Screen coordinates out of range!");
	}

	PixelScreen::screen[y][x] = state;
}

bool PixelScreen::getPixelState(unsigned int x, unsigned int y) {
	if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT) {
		throw std::runtime_error("Screen coordinates out of range!");
	}

	return PixelScreen::screen[y][x];
}
