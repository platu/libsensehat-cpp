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

#include <iostream>
#include <thread> // sleep_for, sleep_until

#include <sensehat.h>
#include <console_io.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, milliseconds

// Main program
int main() {

	bool stop = false;
	int key_count = 0;
	char c;

	if(senseInit()) {
		std::cout << "-------------------------------" << std::endl
			<< "Sense Hat initialization Ok." << std::endl;

		// Console initialization
		clearConsole();
		gotoxy(1,2);
		std::cout << "Identify arrow keys" << std::endl;

		// Main task
		do {
			gotoxy(5,4);
			std::cout << '>';

			// Get the number of keys in the
			// keyboard buffer
			key_count = keypressed();

			// Print the key ascii code if a single key is pressed
			if (key_count == 1) {
				c = std::cin.get();
				std::cout << "key = [" << c << "]"; // Display the character
                clearEOL();
				// Stop the program if 'q' is pressed
				if (toupper(c) == 'Q') 
					stop = true;
			}

			sleep_until(system_clock::now() + milliseconds(20));

		} while (!stop);
		std::cout << endl;

		senseShutdown();
		std::cout << "-------------------------------" << endl
			<< "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}