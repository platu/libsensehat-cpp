/* File: 07_WaitForJoystick.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseGetJoystickEvent() function.
 *
 * Function prototypes:
 *
 * senseSetJoystickWaitTime(long int, long int);
 *           duration in sec -^         ^- duration int milliseconds
 *
 * bool senseGetJoystickEvent(stick_t *);
 *        updated struct members -^
 *
 * The stick_t struct has three members
 *		timestamp	seconds and microseconds float number
 *		action		KEY_ENTER, KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN
 *		state		KEY_RELEASED, KEY_PRESSED, KEY_HELD
 *
 * This program illustrates this non blocking function. If one click happens
 * during the time defined by senseSetJoystickWaitTime() then the action and
 * state are printed on terminal screen. If nothing happens on the joystick,
 * then a message is sent to the terminal screen evrey second.
 * In this example porgram, the time base is set to 20ms.
 *
 * Time counter is intitialized with 60 x (5 x 20ms) = 3000
 * One second corresponds to 50 time counter increments.
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
    int time, event_count;
    stick_t joystick;
    bool clicked = false;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        event_count = 0;
        cout << "Waiting for 60 seconds" << endl;
        for (time = 1; time <= 3000; time++) {
            // Set monitoring for 20ms
            senseSetJoystickWaitTime(0, 20);

            // non blocking function call
            clicked = senseGetJoystickEvent(joystick);
            if (clicked) {
                do {
                    event_count++;
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
                    clicked = senseGetJoystickEvent(joystick);
                } while (clicked);
                sleep_until(system_clock::now() + milliseconds(20));
            }
            // Print elapsed time in seconds
            else if (time % 50 == 0)
                cout << setw(3) << right << time / 50 << " seconds" << endl;
        }

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
