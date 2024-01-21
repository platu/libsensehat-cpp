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

#include <termios.h>
#include <assert.h>

#include <console_io.h>
#include <sensehat.h>

using namespace std;

int main() {
    int event_count;
    stick_t joystick;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        cout << "Waiting for 60 joystick events" << endl;
        for (event_count = 0; event_count < 60; event_count++) {
            // blocking function call
            joystick = senseWaitForJoystick();
            cout << "Event number " << event_count << " -> ";

            // Identify action on stick
            switch (joystick.action) {
                case KEY_ENTER:
                    cout << "push  ";
                    break;
                case KEY_UP:
                    cout << "up    ";
                    break;
                case KEY_LEFT:
                    cout << "left  ";
                    break;
                case KEY_RIGHT:
                    cout << "right ";
                    break;
                case KEY_DOWN:
                    cout << "down  ";
                    break;
            }

            // Identify state of stick
            switch (joystick.state) {
                case KEY_RELEASED:
                    cout << "\treleased";
                    break;
                case KEY_PRESSED:
                    cout << "\tpressed";
                    break;
                case KEY_HELD:
                    cout << "\theld";
                    break;
            }
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
