/* File: 07_WaitForJoystick.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseWaitForJoystick() function.
 *
 * Function prototypes:
 *
 * stick_t senseWaitForJoystick()
 *    ^- struct returned
 *
 * The stick_t struct has three members
 *		timestamp	seconds and microseconds float number
 *		action		KEY_ENTER, KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN
 *		state		KEY_RELEASED, KEY_PRESSED, KEY_HELD
 *
 * This program shows that there are many events for a single action.
 * The use of this blocking function requires to evaluate a combination of the
 * two members of the type stick_t: action and state
 */

#include <iostream>
#include <iomanip>

#include <console_io.h>
#include <sensehat.h>

using namespace std;

int main() {
    const rgb_pixel_t red = {.color = {255, 0, 0}};
    const rgb_pixel_t orange = {.color = {255, 128, 0}};
    const rgb_pixel_t yellow = {.color = {255, 255, 0}};
    const rgb_pixel_t green = {.color = {0, 255, 0}};
    const rgb_pixel_t cyan = {.color = {0, 255, 255}};
    const rgb_pixel_t blue = {.color = {0, 0, 255}};
    const rgb_pixel_t purple = {.color = {255, 0, 255}};
    const rgb_pixel_t pink = {.color = {255, 128, 128}};

    const rgb_pixel_t rainbow[8] = {red,  orange, yellow, green,
                                    cyan, blue,   purple, pink};
    rgb_pixel_t pix;
    int row, col, index;
    int event_count, click;
    stick_t joystick;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        // light up first pixel
        row = col = index = 3;
        pix = rainbow[index];
        senseSetPixel(row, col, pix.color[_R], pix.color[_G], pix.color[_B]);
        click = 0;

        cout << "Waiting for 60 joystick events" << endl;
        event_count = 0;
        do {
            // blocking function call
            joystick = senseWaitForJoystick();
            click++;
            if (click % 4 == 0) {
                cout << "Event number " << event_count + 1 << endl;
                event_count++;

                // Turn off previous pixel
                senseSetPixel(row, col, 0, 0, 0);

                // Push to change color index
                if (joystick.action == KEY_ENTER &&
                    joystick.state == KEY_RELEASED) {
                    index++;
                    if (index == SENSE_LED_WIDTH) index = 0;
                    pix = rainbow[index];
                }

                // Activate the pixel above
                if (joystick.action == KEY_UP &&
                    joystick.state == KEY_RELEASED) {
                    row--;
                    if (row == 0) row = 7;
                }

                // Activate the pixel below
                if (joystick.action == KEY_DOWN &&
                    joystick.state == KEY_RELEASED) {
                    row++;
                    if (row == SENSE_LED_WIDTH) row = 0;
                }

                // Activate the pixel on the left
                if (joystick.action == KEY_LEFT &&
                    joystick.state == KEY_RELEASED) {
                    col--;
                    if (col == 0) col = 7;
                }

                // Activate the pixel on the right
                if (joystick.action == KEY_RIGHT &&
                    joystick.state == KEY_RELEASED) {
                    col++;
                    if (col == SENSE_LED_WIDTH) col = 0;
                }

                // Turn on new pixel
                senseSetPixel(row, col, pix.color[_R], pix.color[_G],
                              pix.color[_B]);
            }

        } while (event_count < 60);

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
