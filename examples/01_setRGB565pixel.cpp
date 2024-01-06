/* File: 01_setRGB565pixel.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetRGB565pixel() function that
 * prints one single pixel encoded in RGB565 format.
 * RGB565 encodes the three colors in a 16 bit integer.
 * Here we use a dedicated type named rgb565_pixel_t.
 *
 * Function prototype:
 *
 * bool senseSetRGB565pixel(int, int, rgb565_pixel_t);
 *  ^-- status            x -^ y -^  pixel color -^
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
	const rgb565_pixel_t red = 0xf800;
	const rgb565_pixel_t orange = 0xfc00;
	const rgb565_pixel_t yellow = 0xffe0;
	const rgb565_pixel_t green = 0x07e0;
	const rgb565_pixel_t cyan = 0x07ff;
	const rgb565_pixel_t blue = 0x001f;
	const rgb565_pixel_t purple = 0xf81f;
	const rgb565_pixel_t pink = 0x0fc10;

	const rgb565_pixel_t rainbow[8] = {red,	 orange, yellow, green,
									   cyan, blue,	 purple, pink};
	rgb565_pixel_t pix;

	int x, y;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		for (y = 0; y < 8; y++) {
			pix = rainbow[y];
			cout << hex << setw(4) << "[ " << pix << " ] " << flush;
			for (x = 0; x < 8; x++) {
				senseSetRGB565pixel(x, y, pix);
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
