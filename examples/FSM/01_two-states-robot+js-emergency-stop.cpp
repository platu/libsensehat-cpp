/* File: 01-two-states-robot.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program is a minimalistic robot simulator with two states:
 * - state RUN: the robot moves forward
 * - state STOP: the robot stops
 * Keyboard keys are used to change the state of the robot.
 * - 'r' key: RUN state
 * - 's' key: STOP state
 * - 'q' key: quit the program
 *
 * These functions are not part of the Sense Hat library. They are intended to
 * be used by students who wish to drive a robot with the arrow keys of the
 * console.
 */

#include <console_io.h>
#include <sensehat.h>

#include <iostream>
#include <thread>  // sleep_for, sleep_until

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;       // system_clock, milliseconds

enum State { RUN, STOP };
enum Event { RUN_KEY, STOP_KEY, EXIT, NO_EVENT };

// Function for state machine evolution
State evolve(State current_state, Event event) {
    State next_state;

    switch (event) {
        case RUN_KEY:
            next_state = RUN;
            break;
        case STOP_KEY:
            next_state = STOP;
            break;
        case EXIT:
            next_state = STOP;
            break;
        default:
            next_state = current_state;
    }

    return next_state;
}

// Function for robot action
void action(State current_state) {
    switch (current_state) {
        case RUN:
            std::cout << "Robot moves forward" << std::endl;
            // TODO: move the robot forward
            break;
        case STOP:
            std::cout << "Robot stops" << std::endl;
            // TODO: stop the robot
            break;
    }
}

// Main program
int main() {
    State current_state, next_state;
    Event event_code;
    bool quit;
    int key_count, time, cycle_count;
    stick_t joystick;
    char c;

    if (senseInit()) {
        std::cout << "-------------------------------" << endl
                  << "Sense Hat initialization Ok." << endl;

        //---------------------------------------------------------------------
        // Initializations
        current_state = STOP;
        next_state = STOP;
        event_code = NO_EVENT;
        quit = false;
        key_count = 0;
        time = 0;
        cycle_count = 0;
        clearConsole();  // Clear the console

        // Display instructions
        gotoxy(5, 2);
        std::cout << "=== ROBOT SIMULATOR ===" << std::endl;
        gotoxy(5, 3);
        std::cout << "Commands:" << std::endl;
        gotoxy(7, 4);
        std::cout << "'R' - Run (robot moves forward)" << std::endl;
        gotoxy(7, 5);
        std::cout << "'S' - Stop (robot stops)" << std::endl;
        gotoxy(7, 6);
        std::cout << "'Q' - Quit or press joystick" << std::endl;
        gotoxy(5, 8);
        std::cout << "Enter command > " << std::flush;

        //---------------------------------------------------------------------
        // Main loop
        do {
            // ---------------------------------------------------------------
            // Event detection
            key_count = keypressed();
            quit = senseGetJoystickEvent(joystick);

            // ---------------------------------------------------------------
            // Event coding
            // Event code is set to EXIT if joystick is pressed
            if (quit) {
                event_code = EXIT;
            }
            // Event codes for single key press
            if (key_count == 1) {
                c = std::cin.get();
                gotoxy(21, 8);  // Move cursor after prompt
                std::cout << "[" << c << "]" << std::flush;

                switch (toupper(c)) {
                    case 'R':
                        event_code = RUN_KEY;
                        break;
                    case 'S':
                        event_code = STOP_KEY;
                        break;
                    case 'Q':
                        event_code = EXIT;
                        quit = true;
                        break;
                }
            }

            if (event_code != NO_EVENT) {
                // -----------------------------------------------------------
                // State transition
                next_state = evolve(current_state, event_code);

                // -----------------------------------------------------------
                // Robot action if state has changed
                if (next_state != current_state) {
                    gotoxy(5, 10);  // Move cursor to column 5, raw 10
                    clearEOL();
                    std::cout << "Robot state: ";
                    std::cout.flush();

                    action(next_state);

                    current_state = next_state;
                }

                // -----------------------------------------------------------
                // Reset event code and key count
                event_code = NO_EVENT;
                key_count = 0;
            }

            // ---------------------------------------------------------------
            // Background task

            // Wait 20 ms before next iteration
            sleep_until(system_clock::now() + milliseconds(20));
            cycle_count++;

            // Print time every 50 cycles of 20 ms
            // This is the background task which illustrates the non-blocking
            // use of event driven functions
            if (cycle_count == 50) {
                cycle_count = 0;
                time += 1;
                gotoxy(5, 12);  // Move cursor to column 5, raw 12
                std::cout << "Running time: " << time << "s" << std::flush;
                clearEOL();
            }

        } while (!quit);
        std::cout << endl;

        senseShutdown();
        std::cout << "-------------------------------" << endl
                  << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
