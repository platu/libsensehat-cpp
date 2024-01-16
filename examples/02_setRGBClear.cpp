/* File: 02_setRGBClear.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseRGBClear() function that
 * clears all pixels at once with one defined color.
 * The color is set by 3 bytes (uint8_t types) passed as Red, Green and Blue
 * parameters.
 *
 * Function prototype:
 *
 * void senseSetRGBClear(uint8_t, uint8_t, uint8_t);
 *                         r -^      g-^      b-^
 *
 * The program prints a red question mark on a white background.
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <console_io.h>
#include <sensehat.h>

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;       // nanoseconds, system_clock, seconds

int main() {
    rgb565_pixel_t clr = 0xf800;
    rgb_pixel_t rgb;
    unsigned int count;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;

        for (count = 0; count < 16; count++) {
            rgb = senseUnPackPixel(clr);
            cout << count << " R: " << (unsigned)rgb.color[_R]
                 << " G: " << (unsigned)rgb.color[_G]
                 << " B: " << (unsigned)rgb.color[_B] << endl;
            senseRGBClear(rgb.color[_R], rgb.color[_G], rgb.color[_B]);

            clr >>= 1;
            if (clr == 0) clr = 0xf800;

            sleep_for(seconds(1));
        }
        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
