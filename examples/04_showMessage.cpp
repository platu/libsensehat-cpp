/* File: 04_showMessage.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseShowRGBcoloredMessage() function
 * that scrolls a character string on the LED matrix.
 * 
 * When the fucntion senseShowMessage() is called with no color specified,
 * the character string is printed with white foreground and black background.
 *
 * Function prototypes:
 *
 * void senseShowMessage(char *);
 *       string to scroll -^
 *
 * void senseShowRGB565ColoredMessage(char *, rgb565_pixel_t, rgb565_pixel_t);
 *                     string to scroll -^  foreground -^   background -^
 *
 * void senseShowRGBColoredMessage(char *, rgb_pixel_t, rgb_pixel_t);
 *                  string to scroll -^ foreground -^ background -^
 *
 * This program asks the user to enter a character string followed by the
 * foreground and background colors. Program ends with the 'q' character.
 */

#include <iostream>
#include <iomanip>
#include <string>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

#define NBCHARS 80

using namespace std;

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

	const rgb_pixel_t black = { .color = {0, 0, 0} };
	const rgb_pixel_t white = { .color = {255, 255, 255} };
	const rgb_pixel_t red = { .color = {255, 0, 0} };
	const rgb_pixel_t orange = { .color = {255, 128, 0} };
	const rgb_pixel_t yellow = { .color = {255, 255, 0} };
	const rgb_pixel_t green = { .color = {0, 255, 0} };
	const rgb_pixel_t cyan = { .color = {0, 255, 255} };
	const rgb_pixel_t blue = { .color = {0, 0, 255} };
	const rgb_pixel_t purple = { .color = {255, 0, 255} };
	const rgb_pixel_t pink = { .color = {255, 128, 128} };

	const rgb_pixel_t c_set[10] = {black, white, red, orange, yellow, green, cyan, blue, purple, pink};
	string msg;
	char *scroll;
	unsigned int fg, bg;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		// Ensure the scroll message is not too long
		msg.resize(NBCHARS);
		cout << "The characater 'q' ends the program." << endl;
		do {
			cout << "First, enter the message to scroll: ";
			getline(cin, msg);
			if (msg != "q") {
				cout << "Second, choose the foreground and background colors" << endl
					<< "according to the following list:" << endl
					<< "1:\tblack" << endl
					<< "2:\twhite" << endl
					<< "3:\tred" <<endl
					<< "4:\torange" << endl
					<< "5:\tyellow" << endl
					<< "6:\tgreen" <<endl
					<< "7:\tcyan" << endl
					<< "8:\tblue" << endl
					<< "9:\tpurple" << endl
					<< "10:\tpink" << endl
					<< "For instance: 2 1 prints the 'a' character white on black." << endl
					<< "Choose 2 colors: ";
				cin >> fg >> bg;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				// msg string to scroll char *
				scroll = strdup(msg.c_str());
				// start scrolling on pixel array
				senseShowRGBColoredMessage(scroll, c_set[fg-1], c_set[bg-1]);
				free(scroll);
			}
		} while (msg != "q");
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}