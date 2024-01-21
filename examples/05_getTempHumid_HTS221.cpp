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

#include <console_io.h>
#include <sensehat.h>

using namespace std;

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
