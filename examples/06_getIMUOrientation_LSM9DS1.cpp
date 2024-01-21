/* File: 06_getIMUOrientation_LSM9DS1.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program collects orientation measures from the LSM9DS1 IMU
 * sensor.
 *
 * Function prototypes:
 *
 * void senseSetIMUConfig(bool compass_enabled, bool gyro_enabled, bool
 * accel_enabled);
 *
 * bool senseGetOrientationRadians(double &pitch, double &roll, double &yaw);
 *
 * bool senseGetOrientationDegrees(double &pitch, double &roll, double &yaw);
 *
 * The program simply calls the senseGetOrientationDegrees() function and print
 * the roll, picth and yaw measures.
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
	double x, y, z;
	unsigned int time = 0;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		senseSetIMUConfig(true, true, true);

		for (time = 0; time < 30; time++) {
			sleep_for(milliseconds(500));

			cout << "Orientation in degrees:\t";
			if (senseGetOrientationDegrees(x, y, z)) {
				cout << fixed << setprecision(6) << "Roll=\t" << x
					 << " Pitch=\t" << y << " Yaw=\t" << z << endl;
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
