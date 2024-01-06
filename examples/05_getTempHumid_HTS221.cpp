/* File: 05_getTempHumid_HTS221.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program collects measures from the HTS221 Humidity sensor.
 * This sensor provides temperature measurement in degrees Celsius and relative
 * humidity measurement.
 *
 * Function prototypes:
 *
 * double senseGetTemperatureFromHumidity();
 *   ^- temperature
 *
 * double senseGetHumidity();
 *   ^- humidity
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
	int c = 0;

	struct termios org_opts, new_opts;
	int res = 0;

	//----- store current settings -------------
	res = tcgetattr(STDIN_FILENO, &org_opts);
	assert(res == 0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL |
									 ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c = getchar();
	//------ restore current settings- ---------
	res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res == 0);

	return c;
}

int main() {
	double Temp, Humid;

	if (senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();

		Temp = senseGetTemperatureFromHumidity();
		cout << fixed << setprecision(2) << "Temp (from humid) = " << Temp
			 << "°C" << endl;

		Humid = senseGetHumidity();
		cout << fixed << setprecision(0) << "Humidity = " << Humid << "% rH"
			 << endl;

		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
