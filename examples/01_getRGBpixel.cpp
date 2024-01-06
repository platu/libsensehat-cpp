/* File: 01_getRGBpixel.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseGetRGBpixel() function that
 * reads one single pixel encoded in an array of three 8 bit integers.
 * The three integers represent the R(ed), G(reen), and B(lue) colors.
 * Here we use a dedicated type named rgb_pixel_t.
 *
 * Function prototype:
 *
 * rgb_pixel_t senserGetRGBpixel(int, int);
 *    ^-- pixel color          x -^ y -^
 *
 * The program starts by filling colors on the Sense Hat LED matrix,
 * then the user is asked to choose one row and one column to read the color
 * value of this pixel.
 */

#include <assert.h>
#include <termios.h>

#include <iomanip>
#include <iostream>
#include <limits>

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
	rgb565_pixel_t color, mask = 0xf800;
	rgb_pixel_t readpx;
	int x, y;
	int row, col;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		// First we fill the LED matrix
		for (y = 0; y < 8; y++) {
			color = mask;
			for (x = 0; x < 8; x++) {
				senseSetRGB565pixel(x, y, color);
				color <<= 1;
			}
			mask >>= 2;
		}

		// Next we ask the user to enter pixel row and column
		cout << "Enter the row and column numbers of the pixel to be read: ";
		cin >> row >> col;
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Finally, we read the pixel in RGB565 format
		readpx = senseGetRGBpixel(row, col);
		cout << "Here is the color encoded in RGB format." << endl
			 << "Hexadecimal:\t" << hex << setw(3) << (uint16_t)readpx.color[_R]
			 << ", " << (uint16_t)readpx.color[_G] << ", "
			 << (uint16_t)readpx.color[_B] << endl
			 << "Decimal:\t" << dec << setw(3) << (uint16_t)readpx.color[_R]
			 << ", " << (uint16_t)readpx.color[_G] << ", "
			 << (uint16_t)readpx.color[_B] << endl;
		senseClear();
		senseSetRGBpixel(row, col, readpx.color[_R], readpx.color[_G],
						 readpx.color[_B]);

		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
