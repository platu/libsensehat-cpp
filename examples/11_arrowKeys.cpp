/* File: 11_arrowKeys.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the use of the console arrow keys.
 *
 * These functions are not part of the Sense Hat library. They are intended to
 * be used by students who wish to drive a robot with the arrow keys of the
 * console.
 */

#include <iostream>
#include <thread>  // sleep_for, sleep_until

#include <sensehat.h>
#include <console_io.h>

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;       // system_clock, milliseconds

// Define display zones
const int STATUS_ROW = 10;
const int ACTION_ROW = 12;

// Main program
int main() {
    bool stop = false;
    int key_count = 0;
    char c;

    if (senseInit()) {
        std::cout << "-------------------------------" << std::endl
                  << "Sense Hat initialization Ok." << std::endl;

        // Console initialization
        clearConsole();

        // Display header and instructions
        gotoxy(1, 1);
        std::cout << "========================================" << std::endl;
        std::cout << "        Arrow Keys Detection Demo       " << std::endl;
        std::cout << "========================================" << std::endl;
        gotoxy(1, 5);
        std::cout << "Instructions:" << std::endl;
        std::cout << "  - Use arrow keys to navigate" << std::endl;
        std::cout << "  - Press 'Q' to quit" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Display initial status
        gotoxy(1, STATUS_ROW);
        std::cout << "Status: Waiting for input...";
        gotoxy(1, ACTION_ROW);
        std::cout << "Action: ";
        std::cout << std::flush;

        // Main task
        do {
            // Get the number of keys in the keyboard buffer
            key_count = keypressed();

            // Print the key ascii code if a single key is pressed
            if (key_count == 1) {
                c = std::cin.get();

                // Clear and update status line
                gotoxy(1, STATUS_ROW);
                clearEOL();
                std::cout << "Status: Key pressed";

                // Clear and display action
                gotoxy(1, ACTION_ROW);
                clearEOL();
                std::cout << "Action: Key = [" << c
                          << "] (ASCII: " << static_cast<int>(c) << ")";
                std::cout << std::flush;

                // Stop the program if 'q' is pressed
                if (toupper(c) == 'Q') {
                    stop = true;
                    gotoxy(1, ACTION_ROW);
                    clearEOL();
                    std::cout << "Action: Quitting..." << std::flush;
                }
            }
            // Detect arrow keys if the key count is greater than 1
            else if (key_count > 1) {
                // Clear and update status line
                gotoxy(1, STATUS_ROW);
                clearEOL();
                std::cout << "Status: Arrow key detected";

                // Clear and display action
                gotoxy(1, ACTION_ROW);
                clearEOL();
                std::cout << "Action: ";

                switch (getArrowKey()) {
                    case UP:
                        std::cout << "↑ UP - Accelerate";
                        break;
                    case DOWN:
                        std::cout << "↓ DOWN - Slow down";
                        break;
                    case RIGHT:
                        std::cout << "→ RIGHT - Turn right";
                        break;
                    case LEFT:
                        std::cout << "← LEFT - Turn left";
                        break;
                    default:
                        std::cout << "? Unknown key sequence";
                        break;
                }
                std::cout << std::flush;
            }

            sleep_until(system_clock::now() + milliseconds(20));

        } while (!stop);

        // Clean exit display
        gotoxy(1, 14);
        std::cout << "========================================" << std::endl;

        senseShutdown();
        std::cout << "Sense Hat shut down." << std::endl;
        std::cout << "========================================" << std::endl;

        gotoxy(1, 17);  // Position cursor after output
    }

    return EXIT_SUCCESS;
}
