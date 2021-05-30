/* File: 02_setRGBClear.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseRGBClear() function that
 * clears all pixels at once with one defined color.
 * The color is set by 3 bytes (uint8_t types) passed as Red, Green and Blue
 * parameters. 
 *
 * Function prototype:
 *
 * void senseSetRGBClear(uint8_t, uint8_t, uint8_t);
 *                         r -^      g-^      b-^
 *
 * The program prints a red question mark on a white background.
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int getch( ) {
	int c=0;

	struct termios org_opts, new_opts;
	int res=0;

	//----- store current settings -------------
	res=tcgetattr(STDIN_FILENO, &org_opts);
	assert(res==0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= (tcflag_t)~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c=getchar();
	//------ restore current settings- ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);

	return c;
}

int main() {

	rgb565_pixel_t clr = 0xf800;
	rgb_pixel_t rgb;
	unsigned int count;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		for(count = 0; count < 64; count++) {

			rgb = senseUnPackPixel(clr);
			senseRGBClear(rgb.color[_R], rgb.color[_G], rgb.color[_B]);

			clr >>= 1;
			if (clr == 0)
				clr = 0xf800;

			sleep_for(milliseconds(200));
		}
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
