/* File: 01_setRGBpixel.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetRGBpixel() function that
 * prints one single pixel encoded in an array of 3 8 bit integers.
 * One 8 bit integer is used for each R, G, and B colors.
 * Here we use a dedicated type named rgb_pixel_t.
 *
 * Function prototype:
 *
 * bool senseSetRGBpixel(int, int, uint8_t, uint8_t, uint8_t);
 *  ^-- status         x -^ y -^    pixel R, G, B colors -^
 *
 * The program prints one rainbow color per column on the Sense Hat LED matrix.
 */

#include <iostream>
#include <iomanip>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

using namespace std;

int getch() {
	int c = 0;

	struct termios org_opts, new_opts;
	int res = 0;

	//----- store current settings -------------
	res = tcgetattr(STDIN_FILENO, &org_opts);
	assert(res == 0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL |
									 ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c = getchar();
	//------ restore current settings- ---------
	res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res == 0);

	return c;
}

int main() {
	const rgb_pixel_t red = {.color = {255, 0, 0}};
	const rgb_pixel_t orange = {.color = {255, 128, 0}};
	const rgb_pixel_t yellow = {.color = {255, 255, 0}};
	const rgb_pixel_t green = {.color = {0, 255, 0}};
	const rgb_pixel_t cyan = {.color = {0, 255, 255}};
	const rgb_pixel_t blue = {.color = {0, 0, 255}};
	const rgb_pixel_t purple = {.color = {255, 0, 255}};
	const rgb_pixel_t pink = {.color = {255, 128, 128}};

	const rgb_pixel_t rainbow[8] = {red,  orange, yellow, green,
									cyan, blue,	purple, pink};
	rgb_pixel_t pix;

	int x, y;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		for (y = 0; y < 8; y++) {
			pix = rainbow[y];
			cout << "[" << static_cast<uint16_t>(pix.color[_R]) << ", "
				 << static_cast<uint16_t>(pix.color[_G]) << ", "
				 << static_cast<uint16_t>(pix.color[_B]) << "], " << flush;
			for (x = 0; x < 8; x++) {
				senseSetPixel(x, y, pix.color[_R], pix.color[_G],
							  pix.color[_B]);
			}
		}

		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
