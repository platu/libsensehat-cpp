/* File: 07_WaitForJoystick.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseWaitForJoystick() function.
 *
 * Function prototypes:
 * 
 * stick_t senseWaitForJoystick()
 *    ^- struct returned 
 *
 * The stick_t struct has three members
 *		timestamp	seconds and microseconds float number
 *		action		KEY_ENTER, KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN
 *		state		KEY_RELEASED, KEY_PRESSED, KEY_HELD
 *
 * This program shows that there are many events for a single action.
 * The use of this blocking function requires to evaluate a combination of the
 * two members of the type stick_t: action and state 
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

int getch() {
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

	int time, event_count;
	stick_t joystick;
	bool clicked = false;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
 
		event_count = 0;
		cout << "Waiting for 60 seconds" << endl;
		for(time = 1; time <= 60; time++) {

			// Set monitoring for 1 second
			senseSetJoystickWaitTime(1, 0);

			// non blocking function call
			clicked = senseGetJoystickEvent(&joystick);
			if (clicked) {
				do {
					event_count++;
					cout << "Event number " << event_count << " -> ";

					// Identify action on stick
					switch (joystick.action) {
						case KEY_ENTER:	cout << "push  "; break;
						case KEY_UP:	cout << "up    "; break;
						case KEY_LEFT:	cout << "left  "; break;
						case KEY_RIGHT:	cout << "right "; break;
						case KEY_DOWN:	cout << "down  "; break;
					}

					// Identify state of stick
					switch(joystick.state) {
						case KEY_RELEASED:	cout << "\treleased"; break;
						case KEY_PRESSED:	cout << "\tpressed"; break;
						case KEY_HELD:		cout << "\theld"; break;
					}
					cout << endl;
					clicked = senseGetJoystickEvent(&joystick);
				} while (clicked);
				sleep_until(system_clock::now() + seconds(1));
			}
			else
				cout << setw(3) << right << time << " seconds" << endl;
		}

		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
