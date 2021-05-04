/* File: 05_getTempHumid_HTS221.cpp
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

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

using namespace std;

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

	double x, y ,z;

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
 
		// wait 250ms
		usleep(250 * 1000);
		cout << "Orientation in radians." << endl;
		if (senseGetOrientationRadians(&x, &y, &z)) {
			cout << fixed << setprecision(6) 
				<< "Roll = " << x
				<< " Pitch = " << y
				<< " Yaw = " << z << endl;
			}
		else
			cout << "Error. No measures." << endl;

		// wait 250ms
		usleep(250 * 1000);
		cout << "Orientation in degrees." << endl;
		if (senseGetOrientationDegrees(&x, &y, &z)) {
			cout << fixed << setprecision(6) 
				<< "Roll = " << x
				<< " Pitch = " << y
				<< " Yaw = " << z << endl;
			}
		else
			cout << "Error. No measures." << endl;

		cout << "Compass angle to north in degrees." << endl;
		cout << fixed << setprecision(2) << senseGetCompass() << endl; 


		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
