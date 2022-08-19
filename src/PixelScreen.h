#include <vector>
#include <stdexcept>

#define SCREEN_WIDTH 32u
#define SCREEN_HEIGHT 32u

#ifndef PIXEL_SCREEN_H
#define PIXEL_SCREEN_H

class PixelScreen {
public:
	static void setPixel(unsigned int x, unsigned int y, bool state);
	static bool getPixelState(unsigned int x, unsigned int y);

private:
	static std::vector<std::vector<bool>> screen;
};

#endif
