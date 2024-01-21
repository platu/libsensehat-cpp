/* File: 03_flip_H_and_V.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseFlip_h() and senseFlip_v()
 * functions which respectively flip image on the LED matrix horizontally and
 * vertically.
 * When the redraw boolean parameter is set to true, the image is redrawn
 * immediately.
 *
 * Function prototypes:
 *
 * rgb_pixels_t senseFlip_h(bool);
 * rgb_pixels_t senseFlip_v(bool);
 *            redraw switch -^
 *
 * This program prints a red question mark on a white background then the image
 * is flipped after keypress.
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

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
		senseSetPixels(question_mark);
		cout << endl << "Waiting for keypress to flip horizontally." << endl;
		getch();
		senseFlip_h(true);
		cout << endl << "Waiting for keypress to flip vertically." << endl;
		getch();
		senseFlip_v(true);
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
