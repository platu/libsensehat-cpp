#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

//! \brief Arrow key code list
enum arrowKey {UP, DOWN, LEFT, RIGHT, OTHER};

//! \brief Clear the console screen and place the cursor at the top left
void clearConsole() {
	std::cout << "\x1b[2J\x1b[0;0f" << std::flush;
}

//! \brief Clear the line from the cursor position to the end of line
void clearEOL() {
	std::cout << "\x1b[K" << std::flush;
}

//! \brief Set cursor position to (x,y) in the console
//! \param x Horizontal position or column
//! \param y Vertical position or row
void gotoxy(int x, int y) {
	std::cout << "\x1b[" << y << ';' << x << 'f' << std::flush;
}

//! \brief Non-blocking keyboard input detection
//! \return Number of bytes waiting in the keyboard buffer
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
		std::cin.sync_with_stdio();

		initialized = true;
	}

	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

//! \brief Return the arrow key code from the keyboard buffer if any
//! \return arrowKey code among UP, DOWN, LEFT, RIGHT, OTHER enum values
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

//! \brief Wait for a single key press and return the key code
//! \return Key code of the pressed key
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