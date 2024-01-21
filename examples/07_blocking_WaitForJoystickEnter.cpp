/* File: 07_blocking_WaitForJoystickEnter.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseWaitForJoystick() function.
 *
 * Function prototype:
 *
 * bool senseWaitForJoystickEnter()
 *  ^- true if joystick button is pressed
 *
 * This program waits for the joystick button to be pressed
 */

#include <iostream>
#include <iomanip>

#include <sensehat.h>

using namespace std;

int main() {
	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		senseRGBClear(0, 204, 128);

		senseWaitForJoystickEnter();

		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
