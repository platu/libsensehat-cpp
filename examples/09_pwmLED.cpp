/* File: 09_pwmLED.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates PWM functions
 *
 * Function prototypes:
 *
 * bool pwmInit(unsigned int chan);
 *         PWM channel 0 or 1 -^
 *
 * bool pwmPeriod(unsigned int chan, unsigned int period);
 *          PWM channel 0 or 1 -^       usec period -^
 *
 * bool pwmDutyCycle(unsigned int chan, unsigned int percent);
 *          PWM channel 0 or 1 -^   duty cycle 0 to 100% -^
 *
 * bool pwmEnable(unsigned int chan);
 * bool pwmDisable(unsigned int chan);
 *            PWM channel 0 or 1 -^
 *
 * _PWM_PIN_----_330_resistor_----_LED_----|GND
 *
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cstdlib>

#include <sensehat.h>

#define NB_CYCLE 5

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;       // system_clock, milliseconds

int main(int argc, char **argv) {
    int opt, count;
    char *eptr;
    unsigned int chan = 0, percent = 0;

    // command line arguments: -c 0 or 1
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        if (opt == 'c')
            chan = strtoul(optarg, &eptr, 10);
        else
            cerr << "Usage: " << argv[0] << " [-p] GPIO pin number." << endl;
    }

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;

        if (pwmInit(chan)) {
            // Set frequency to 100Hz -> period to 10000 usec
            pwmPeriod(chan, 10000);
            count = 0;
            // Enable PWM channel output
            pwmEnable(chan);
            do {
                for (percent = 0; percent <= 100; percent += 2) {
                    if ((percent % 10) == 0)
                        cout << "Duty cycle: " << percent << "%" << endl;
                    // Set increasing duty cycle
                    pwmDutyCycle(chan, percent);
                    sleep_for(milliseconds(10));
                }

                for (percent = 100; percent >= UINT_MAX; percent -= 2) {
                    if ((percent % 10) == 0)
                        cout << "Duty cycle: " << percent << "%" << endl;
                    // Set decreasing duty cycle
                    pwmDutyCycle(chan, percent);
                    sleep_for(milliseconds(10));
                }
                count++;
            } while (count < NB_CYCLE);
            // Disable PWM channel output
            pwmDisable(chan);
        }

        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
