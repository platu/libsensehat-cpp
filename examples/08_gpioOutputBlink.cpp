/* File: 08_gpioOutputBlink.cpp
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
 * bool gpioSetOutput(unsigned int pin, gpio_t val);
 *                 GPIO pin number -^   on/off -^
 *
 * The program set output on/off 10 times. This is the typical blink led test.
 * Available GPIO pin numbers: 5, 6, 16, 17, 22, 26, 27
 *
 * _GPIO_PIN_----_LED_----_330_resistor_---|GND
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

gpio_state_t toggle(gpio_state_t v) {
    if (v == on)
        return off;
    else
        return on;
}

int main(int argc, char **argv) {
    int opt;
    char *eptr;
    unsigned int count, pin = 5;
    gpio_state_t val = on;

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

        if (gpioSetConfig(pin, out)) {
            for (count = 0; count < NB_EV; count++) {
                cout << "GPIO pin: " << pin << " -> " << val << endl;
                gpioSetOutput(pin, val);

                sleep_for(milliseconds(500));

                val = toggle(val);
            }
        }

        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
