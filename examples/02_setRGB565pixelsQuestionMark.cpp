/* File: 02_setRGB565pixelsQuestionMark.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetRGB565pixels() function that
 * prints all pixels at once.
 * One pixel colors are encoded in RGB565 format. This format fits into a 16
 * bit integer defined by the rgb565_pixel_t type.
 * All pixels are stored in a 8x8 array of rgb565_pixel_t elements.
 * The complete pixel map is defined by the rgb565_pixels_t type.
 *
 * Function prototype:
 *
 * void senseSetRGB565pixels(rgb565_pixels_t );
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
    const rgb565_pixel_t R = 0xf800;  // Red
    const rgb565_pixel_t W = 0xffff;  // White

    rgb565_pixels_t question_mark = {.array = {{W, W, W, R, R, W, W, W},
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
        senseSetRGB565pixels(question_mark);
        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
