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
#include <sys/ioctl.h>
#include <termios.h>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, milliseconds

/// \brief Arrow key code list
enum arrowKey {UP, DOWN, LEFT, RIGHT, OTHER};

/// \brief Clear the console screen and place the cursor at the top left
void clearConsole() {
	cout << "\x1b[2J\x1b[0;0f" << flush;
}

/// \brief Clear the line from the cursor position to the end of line
void clearEOL() {
	cout << "\x1b[K" << flush;
}

/// \brief Set cursor position to (x,y) in the console
/// \param x Horizontal position or column
/// \param y Vertical position or row
void gotoxy(int x, int y) {
	cout << "\x1b[" << y << ';' << x << 'f' << flush;
}

/// \brief Non-blocking keyboard input detection
/// \return Number of bytes waiting in the keyboard buffer
int keypressed() {

	static const int STDIN = 0;
	static bool initialized = false;
	termios term;
	int bytesWaiting;

	if (! initialized) {
		// Deactivate buffered input
		tcgetattr(STDIN, &term);
		term.c_lflag &= (tcflag_t)~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		// Synchronize with C I/O
		cin.sync_with_stdio();

		initialized = true;
	}

	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

/// \brief Return the arrow key code from the keyboard buffer if any
/// \return arrowKey code among UP, DOWN, LEFT, RIGHT, OTHER
arrowKey getArrowKey() {

	char c;
	arrowKey code;

	// Read first character
	c = getchar();
	// Test for escape sequence
	if (c == '\x1b') {
		// Read second character '\\'
		c = getchar();
		// Read third character '['
		c = getchar();
		// Read fourth character with arrow key code
		switch(c) {
			case 'A':
				code = UP;
				break;
			case 'B':
				code = DOWN;
				break;
			case 'C':
				code = RIGHT;
				break;
			case 'D':
				code = LEFT;
				break;
			default:
				code = OTHER;
		}
	}
	fflush(stdin);
	return code;
}

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
