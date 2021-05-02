/* File: 03_flip_H_and_V.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseShowLetter() and
 * senseShowColoredLetter() functions which print a character on the LED
 * matrix.
 * 
 * When no color is specified, the character is printed with white foreground
 * and black background.
 *
 * Functions prototye:
 *
 * void senseShowLetter(char);
 *     character to print -^
 * void senseShowRGB565ColoredLetter(char, rgb565_pixel_t, rgb565_pixel_t);
 *                 character to print -^  foreground -^   background -^
 * void senseShowRGBColoredLetter(char, rgb_pixel_t, rgb_pixel_t);
 *             character to print -^ foreground -^ background -^
 *
 * This program asks the user to choose a character with the foreground and
 * background colors. Program ends with the 'q' character.
 */

#include <iostream>
#include <iomanip>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

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
	char c;
	unsigned int fg, bg;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
		do {
			cout << "Enter the character to print, followed by the foreground" << endl
				<< "and background colors according to the following list:" << endl
				<< "1:black" << endl
				<< "2:white" << endl
				<< "3:red" <<endl
				<< "4:orange" << endl
				<< "5:yellow" << endl
				<< "6:green" <<endl
				<< "7:cyan" << endl
				<< "8:blue" << endl
				<< "9:purple" << endl
				<< "10:pink" << endl
				<< "For example: a 2 1 prints the 'a' character white on black." << endl
				<< "The characater 'q' followed by 2 random colors ends the program." << endl
				<< "Up to you: ";
			cin >> c >> fg >> bg;
			senseShowRGBColoredLetter(c, c_set[fg-1], c_set[bg-1]);
		} while (c != 'q');
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
