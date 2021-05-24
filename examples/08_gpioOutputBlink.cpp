/* File: 08_gpioOutputBlink.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the gpioSetOutput() function which sets a
 * GPIO pin on or off.
 * Pin values are defined by the gpio_t type.
 *
 * Function prototype:
 *
 * bool gpioSetOutput(unsigned int pin, gpio_t val);
 *                 GPIO pin number -^   on/off -^
 *
 * The program set output on/off 10 times. This is the typical blink led test.
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, milliseconds

gpio_t toggle(gpio_t v) {
	if (v == on)
		return off;
	else
		return on;
}
	
int main() {

	unsigned int count;
	gpio_t val = off;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		for(count = 0; count < 10; count++) {

			gpioSetOutput(26, val);
			val = toggle(val);

			sleep_for(milliseconds(500));
		}
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
