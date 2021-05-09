/* File: 06_getIMUGyro_LSM9DS1.cpp
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
 * bool senseGetOrientationRadians(double *, double *, double *);
 *                             roll-^    pitch-^     yaw-^
 *
 * bool senseGetOrientationDegrees(double *, double *, double *);
 *                             roll-^    pitch-^     yaw-^
 *
 * The program simply calls the two functions
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int getch() {
	int c=0;

	struct termios org_opts, new_opts;
	int res=0;

	//----- store current settings -------------
	res=tcgetattr(STDIN_FILENO, &org_opts);
	assert(res==0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c=getchar();
	//------ restore current settings- ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);

	return c;
}

int main() {

	unsigned int time = 0;
	double x, y ,z;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
 
		for (time = 0; time < 60; time++) {
			// wait for 500ms
			sleep_for(milliseconds(500));
			cout << "Gyrometer in radians/s." << endl;
			if (senseGetGyroRadians(&x, &y, &z)) {
				cout << fixed << setprecision(6) 
					<< "Roll = " << x
					<< " Pitch = " << y
					<< " Yaw = " << z << endl;
			}
			else
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
