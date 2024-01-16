/* File: 02_setRGBpixelsQuestionMark.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetRGBpixels() function that
 * prints all pixels at once.
 * One pixel RGB colors are encoded in an array of three bytes defined by the
 * rgb_pixel_t type.
 * RGB colors of all pixels are stored in a 8x8 array of rgb_pixel_t elements.
 * The complete pixel map is defined by the rgb_pixels_t type.
 *
 * Function prototype:
 *
 * void senseSetRGBpixels(rgb_pixels_t );
 *             all pixels map -^
 *
 * The program prints a red question mark on a white background.
 */

#include <iostream>
#include <iomanip>

#include <console_io.h>
#include <sensehat.h>

using namespace std;

int main() {
    const rgb_pixel_t R = {.color = {255, 0, 0}};      // Red
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

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
            << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
