/* File: 03_rotate.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseRotation() function that rotates
 * the image on the LED matrix by increment of 90 degrees clockwise.
 * The angle parameter values are: 90, 180, and 270 degrees.
 * Any ohter value have no effetc.
 *
 * Function prototype:
 *
 * rgb_pixels_t senseRotation(unsigned int);
 *                     rotation angle -^
 *
 * This program prints a red question mark on a white background then the user
 * is asked to give the rotation angle. The program ends with angle value 0.
 */

#include <iostream>
#include <iomanip>

#include <console_io.h>
#include <sensehat.h>

using namespace std;

int main() {
	const rgb_pixel_t R = {.color = {255, 0, 0}};	   // Red
	const rgb_pixel_t W = {.color = {255, 255, 255}};  // White

	rgb_pixels_t question_mark = {.array = {{W, W, W, R, R, W, W, W},
											{W, W, R, W, W, R, W, W},
											{W, W, W, W, W, R, W, W},
											{W, W, W, W, R, W, W, W},
											{W, W, W, R, W, W, W, W},
											{W, W, W, R, W, W, W, W},
											{W, W, W, W, W, W, W, W},
											{W, W, W, R, W, W, W, W}}};
	unsigned int angle;
	rgb_pixels_t rgb_test_pixels;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
		senseSetPixels(question_mark);
		do {
			cout << "Enter the rotation angle [0, 90, 180, 270]" << endl
				 << "The value 0 ends the program." << endl
				 << "Angle: ";
			cin >> angle;
			rgb_test_pixels = senseRotation(angle);
			senseSetPixels(rgb_test_pixels);
		} while (angle != 0);
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
