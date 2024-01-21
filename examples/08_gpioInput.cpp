/* File: 08_gpioInput.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the gpioSetOutput() function which sets a
 * GPIO pin on or off.
 *
 * Function prototypes:
 *
 * bool gpioSetConfig(unsigned int pin, gpio_dir_t direction);
 *                 GPIO pin number -^   in/out -^
 *
 * int gpioGetInput(unsigned int pin);
 * ^- read val   GPIO pin number -^
 *
 * The program counts 10 events from input pin number
 * Available GPIO pin numbers: 5, 6, 16, 17, 22, 26, 27
 *
 * _GPIO_PIN_----_push_button_----_4.7k_resistor_----> 3.3V
 *
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <sensehat.h>

#define NB_EV 10

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;       // system_clock, milliseconds

int main(int argc, char **argv) {
    int opt, prev, val;
    char *eptr;
    unsigned int count, pin = 5;

    // command line arguments: -p 27 for pin 27
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        if (opt == 'p')
            pin = strtoul(optarg, &eptr, 10);
        else
            cerr << "Usage: " << argv[0] << " [-p] GPIO pin number." << endl;
    }

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;

        if (gpioSetConfig(pin, in)) {
            prev = 1;
            count = 0;
            do {
                val = gpioGetInput(pin);
                if (val != prev) {
                    cout << "GPIO pin: " << pin << " -> " << val << endl;
                    count++;
                }
                sleep_for(milliseconds(20));
                prev = val;
            } while (count < NB_EV);
        }

        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
