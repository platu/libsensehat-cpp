/* File: 09_pwmLED.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates PWM functions
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
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, milliseconds

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

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		if (pwmInit(chan)) {
			pwmPeriod(chan, 10000);
			count = 0;
			pwmEnable(chan);
			do {
				for (percent = 0; percent <= 100; percent +=2) {
					if ((percent % 10) == 0)
						cout << "Duty cycle: " << percent << "%" << endl;
					pwmDutyCycle(chan, percent);
					sleep_for(milliseconds(10));
				}

				for (percent = 100; percent >= UINT_MAX; percent -=2) {
					if ((percent % 10) == 0)
						cout << "Duty cycle: " << percent << "%" << endl;
					pwmDutyCycle(chan, percent);
					sleep_for(milliseconds(10));
				}
				count++;
			} while (count < NB_CYCLE);
			pwmDisable(chan);
		}

		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
