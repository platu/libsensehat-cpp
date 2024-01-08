/* File: 00_non-blocking-kb.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the use of keyboard keys through RPi
 * console.
 *
 * These functions are not part of the Sense Hat library. They are intended to
 * be used by students who wish to drive a robot with the arrow keys of the
 * console.
 */

#include <console_io.h>
#include <sensehat.h>

#include <iostream>
#include <thread>  // sleep_for, sleep_until

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;	   // system_clock, milliseconds

// Main program
int main() {
	int key_count, time, cycle_count;
	char c;
	stick_t joystick;
	bool stop;

	if (senseInit()) {
		std::cout << "-------------------------------" << endl
				  << "Sense Hat initialization Ok." << endl;

		clearConsole();	 // Clear the console
		std::cout << "Keyboard key event detection" << endl
				  << "Press joystick to quit." << endl;

		//---------------------------------------------------------------------
		// Initializations
		stop = false;
		key_count = 0;
		time = 0;
		cycle_count = 0;

		//---------------------------------------------------------------------
		// Main task
		do {
			gotoxy(5, 4);  // Move cursor to column 5, raw 4
			std::cout << '>';

			// ---------------------------------------------------------------
			// Collect events by scanning keyboard and joystick

			// Get the number of keys in the keyboard buffer
			// (0 if no key pressed)
			// This is the keyboard event detection function
			key_count = keypressed();

			// Stop boolean is set to true if joystick is pressed
			stop = senseGetJoystickEvent(joystick);

			// Print the key ascii code if a single key is pressed
			// An event has been detected
			if (key_count == 1) {
				c = std::cin.get();
				std::cout << "key = [" << c << "]";	 // Display the character
				clearEOL();
			}

			// ---------------------------------------------------------------
			// Wait 20 ms before next iteration
			sleep_for(milliseconds(20));
			cycle_count++;

			// Print time every 50 cycles of 20 ms
			// This is the background task which illustrates the non-blocking
			// use of event driven functions
			if (cycle_count == 50) {
				cycle_count = 0;
				time += 1;
				gotoxy(5, 6);  // Move cursor to column 5, raw 6
				std::cout << "time = " << time << "s";
				clearEOL();
			}

		} while (!stop);
		std::cout << endl;

		senseShutdown();
		std::cout << "-------------------------------" << endl
				  << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
