/* File: 02_setRGBLowLight.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetLowLight() function that
 * decreases the brightness of all LEDs in order to preserve battery life
 *
 * Function prototype:
 *
 * void senseSetLowLight(bool low);
 *         low light switch -^
 *
 * The program prints a red question mark on a white background with the low
 * light switch enabled. Then the pixel map is read in order to show that the
 * color factors are effectively changed.
 */

#include <iostream>
#include <iomanip>

#include <console_io.h>
#include <sensehat.h>

using namespace std;

int main() {
    unsigned int row, col;
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

        // Turn on light reduction
        senseSetLowLight(true);

        // Print all pixels
        senseSetPixels(question_mark);

        // Read all pixels
        question_mark = senseGetPixels();

        // Show that the color values have decreased
        for (row = 0; row < SENSE_LED_WIDTH; row++) {
            for (col = 0; col < SENSE_LED_WIDTH; col++)
                cout << "{ " << setw(3) << right
                     << (unsigned int)question_mark.array[row][col].color[_R]
                     << ", " << setw(3) << right
                     << (unsigned int)question_mark.array[row][col].color[_G]
                     << ", " << setw(3) << right
                     << (unsigned int)question_mark.array[row][col].color[_B]
                     << "}, ";
            cout << endl;
        }
        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
