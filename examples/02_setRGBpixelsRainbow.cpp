/* File: 02_setRGBpixelsRainbow.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseSetPixels() function that
 * prints all pixels at once.
 * One pixel RGB colors are encoded in an array of three bytes. This array is
 * defined by the rgb_pixel_t type.
 * All pixels are stored in a 8x8 array of rgb_pixel_t elements.
 * The complete pixel map is defined by the rgb_pixels_t type.
 *
 * Function prototype:
 *
 * void senseSetPixels(rgb_pixels_t );
 *             all pixels map -^
 *
 * The program prints many many pixel maps with small color increments which
 * simulate ceiling ambient light system.
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
    rgb_pixels_t rainbow = {.array = {{{255, 0, 0},
                                       {255, 0, 0},
                                       {255, 87, 0},
                                       {255, 196, 0},
                                       {205, 255, 0},
                                       {95, 255, 0},
                                       {0, 255, 13},
                                       {0, 255, 122}},
                                      {{255, 0, 0},
                                       {255, 96, 0},
                                       {255, 205, 0},
                                       {196, 255, 0},
                                       {87, 255, 0},
                                       {0, 255, 22},
                                       {0, 255, 131},
                                       {0, 255, 240}},
                                      {{255, 105, 0},
                                       {255, 214, 0},
                                       {187, 255, 0},
                                       {78, 255, 0},
                                       {0, 255, 30},
                                       {0, 255, 140},
                                       {0, 255, 248},
                                       {0, 152, 255}},
                                      {{255, 233, 0},
                                       {178, 255, 0},
                                       {70, 255, 0},
                                       {0, 255, 40},
                                       {0, 255, 148},
                                       {0, 253, 255},
                                       {0, 144, 255},
                                       {0, 34, 255}},
                                      {{170, 255, 0},
                                       {61, 255, 0},
                                       {0, 255, 48},
                                       {0, 255, 157},
                                       {0, 243, 255},
                                       {0, 134, 255},
                                       {0, 26, 255},
                                       {83, 0, 255}},
                                      {{52, 255, 0},
                                       {0, 255, 57},
                                       {0, 255, 166},
                                       {0, 235, 255},
                                       {0, 126, 255},
                                       {0, 17, 255},
                                       {92, 0, 255},
                                       {201, 0, 255}},
                                      {{0, 255, 66},
                                       {0, 255, 174},
                                       {0, 226, 255},
                                       {0, 117, 255},
                                       {0, 8, 255},
                                       {100, 0, 255},
                                       {210, 0, 255},
                                       {255, 0, 192}},
                                      {{0, 255, 183},
                                       {0, 217, 255},
                                       {0, 109, 255},
                                       {0, 0, 255},
                                       {110, 0, 255},
                                       {218, 0, 255},
                                       {255, 0, 183},
                                       {255, 0, 74}}}};

    uint8_t red, green, blue;
    int x, y, loop;

    steady_clock::time_point begin, end;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();
        begin = steady_clock::now();
        for (loop = 2000; loop > 0; loop--) {
            for (y = 0; y < SENSE_LED_WIDTH; y++)
                for (x = 0; x < SENSE_LED_WIDTH; x++) {
                    red = rainbow.array[x][y].color[_R];
                    green = rainbow.array[x][y].color[_G];
                    blue = rainbow.array[x][y].color[_B];

                    if ((red == 255) && (green < 255) && (blue == 0)) green++;
                    if ((green == 255) && (red > 0) && (blue == 0)) red--;
                    if ((green == 255) && (blue < 255) && (red == 0)) blue++;
                    if ((blue == 255) && (green > 0) && (red == 0)) green--;
                    if ((blue == 255) && (red < 255) && (green == 0)) red++;
                    if ((red == 255) && (blue > 0) && (green == 0)) blue--;

                    rainbow.array[x][y].color[_R] = red;
                    rainbow.array[x][y].color[_G] = green;
                    rainbow.array[x][y].color[_B] = blue;
                }
            senseSetPixels(rainbow);
            sleep_until(system_clock::now() + milliseconds(1));
            if (loop < 2000 && loop % 200 == 0) {
                cout << setw(4) << loop << " loops left / ";
                end = steady_clock::now();
                cout << "Elapsed time = "
                     << duration_cast<milliseconds>(end - begin).count()
                     << " ms" << endl;
                begin = steady_clock::now();
            }
        }

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
