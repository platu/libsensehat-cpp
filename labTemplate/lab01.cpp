/* File: lab01.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * Student lab template source file
 *
 * This program waits for the joystick button to be pressed
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, seconds, milliseconds


int main() {
	// Declare your variables below !
	const rgb_pixel_t R = { .color = {255, 0, 0} }; // Red
	const rgb_pixel_t W = { .color = {255, 255, 255} }; // White

	rgb_pixels_t question_mark_RedOnWhite = { .array = {
		{ W, W, W, R, R, W, W, W },
		{ W, W, R, W, W, R, W, W },
		{ W, W, W, W, W, R, W, W },
		{ W, W, W, W, R, W, W, W },
		{ W, W, W, R, W, W, W, W },
		{ W, W, W, R, W, W, W, W },
		{ W, W, W, W, W, W, W, W },
		{ W, W, W, R, W, W, W, W } }
	};

	int count;
	// End of variables declarations

	if(senseInit()) {
		cout << "Sense Hat initialization Ok." << endl;
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Insert your code below !

		count = 0;

		do {
			senseSetRGBpixels(question_mark_RedOnWhite);

			sleep_for(seconds(1));

			question_mark_RedOnWhite = senseFlip_v(false);

			count = count + 1;
		} while (count < 5);

		cout << "Press joystick button to quit." << endl;
		senseWaitForJoystickEnter();

		// Insert your code above !
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		senseShutdown();
		cout << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
