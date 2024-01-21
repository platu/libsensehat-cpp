/* File: 06_setPixelToNorth.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program collects direction to north with the senseGetCompass()
 * function.
 *
 * Function prototypes:
 *
 * void senseSetIMUConfig(bool compass_enabled, bool gyro_enabled, bool
 * accel_enabled);
 *
 * double senseGetCompass()
 *   ^- angle in degrees [0..360]
 *
 * This program reverses the direction so the illuminated led always appears to
 * point north.
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

#define LEDNB 28

int main() {
    unsigned int time = 0;
    // list of leds located at the edge of the square
    const int led_loop[LEDNB] = {4,  5,  6,  7,  15, 23, 31, 39, 47, 55,
                                 63, 62, 61, 60, 59, 58, 57, 56, 48, 40,
                                 32, 24, 16, 8,  0,  1,  2,  3};
    float led_degree_ratio = LEDNB / 360.0;
    double direction;
    int led_index, prev_x, x, prev_y, y, offset;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        // turn on all sensors for Compass
        senseSetIMUConfig(true, true, true);

        for (time = 0; time < 60; time++) {
            sleep_for(milliseconds(500));
            // reverse direction
            direction = 360 - senseGetCompass();
            cout << "Compass angle to north in degrees:\t" << fixed
                 << setprecision(2) << direction << endl;

            // select the led
            led_index = (int)floor(led_degree_ratio * direction);
            offset = led_loop[led_index];

            // extract coordinates
            y = offset / 8;  // row
            x = offset % 8;  // column

            // turns off the previous led
            if (x != prev_x || y != prev_y)
                senseSetRGB565pixel(prev_x, prev_y, 0);

            // turn on the new led
            senseSetRGB565pixel(x, y, 255);

            prev_x = x;
            prev_y = y;
        }

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
