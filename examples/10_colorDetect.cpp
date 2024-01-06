/* File: 10_colorDetect.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates color detection with the TCS34725 Adafruit
 * module
 *
 * Function prototypes:
 *
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cstdlib>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;	   // system_clock, milliseconds

int main() {
	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;

		if (colorDetectInit(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X)) {
			cout << "Color detection initialization." << endl;
		}

		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
