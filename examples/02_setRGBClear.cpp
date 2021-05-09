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
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c=getchar();
	//------ restore current settings- ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);

	return c;
}

int main() {

	uint8_t r = 255, g = 0, b = 0;
	unsigned int count;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		for(count = 0; count < 1024; count++) {
			sleep_for(milliseconds(10));

			senseRGBClear(r, g, b);

			if (r == 255 && g < 255 && b == 0)
				g += 1;

			if (g == 255 && r > 0 && b == 0)
				r -= 1;

			if (g == 255 && b < 255 && r == 0)
				b += 1;

			if (b == 255 && g > 0 && r == 0)
				g -= 1;

			if (b == 255 && r < 255 && g == 0)
				r += 1;

			if (r == 255 && b > 0 && g == 0)
				b -= 1;
		}
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
