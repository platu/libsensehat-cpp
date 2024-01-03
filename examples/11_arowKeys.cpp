/* File: 11_arrowKeys.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the use of the console arrow keys.
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

	if(senseInit()) {
		cout << "-------------------------------" << endl
			<< "Sense Hat initialization Ok." << endl;

	    	// Console initialization
	    	clearConsole();
	    	gotoxy(1,2);
	    	cout << "Identify arrow keys" << endl;

		// Main task
		do {
			gotoxy(5,4);
			cout << '.';

			// Detect if a key is pressed then get the arrow key code from the
			// keyboard buffer
			if (keypressed()) {
				switch(getArrowKey()) {
					case UP:
						cout << "accelerate";
						break;
					case DOWN:
						cout << "slow down";
						break;
					case RIGHT:
						cout << "turn right";
						break;
					case LEFT:
						cout << "turn left";
						break;
					default:
						cout << "unknown" << endl;
						stop = true;
						break;
				}
				clearEOL();
			}

			sleep_until(system_clock::now() + milliseconds(20));

		} while (!stop);

		senseShutdown();
		cout << "-------------------------------" << endl
			<< "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
