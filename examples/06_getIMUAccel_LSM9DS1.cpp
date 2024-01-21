/* File: 06_getIMUAccel_LSM9DS1.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program collects orientation measures from the LSM9DS1 IMU
 * sensor.
 *
 * Function prototypes:
 *
 * void senseSetIMUConfig(bool,          bool,           bool);
 *         compass_enabled-^ gyro_enabled-^ accel_enabled-^
 *
 * bool senseGetOrientationRadians(double &pitch, double &roll, double &yaw);
 *
 * bool senseGetOrientationDegrees(double &pitch, double &roll, double &yaw);
 *
 * The program simply calls one of the two functions
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <console_io.h>
#include <sensehat.h>

using namespace std;
using namespace std::this_thread;  // sleep_for, sleep_until
using namespace std::chrono;	   // nanoseconds, system_clock, seconds

int main() {
	unsigned int time = 0;
	double x, y, z;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		for (time = 0; time < 60; time++) {
			// wait for 500ms
			sleep_for(milliseconds(500));
			cout << "Accelerometer in G." << endl;
			if (senseGetAccelG(x, y, z)) {
				cout << fixed << setprecision(6) << "x = " << x << " y = " << y
					 << " z = " << z << endl;
			} else
				cout << "Error. No measures." << endl;
		}

		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
