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
using namespace std::chrono;       // nanoseconds, system_clock, seconds

const double MAX_NORM = 10.0;  // Maximum norm of the acceleration vector
const int NUM_READINGS = 5;    // Number of readings to average

int main() {
    unsigned int time = 0;
    double x, y, z, a, a_min = MAX_NORM;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        for (time = 0; time < 60; time++) {
            // wait for 200ms
            sleep_for(milliseconds(200));
            cout << "Accelerometer in G." << endl;

            double x_sum = 0, y_sum = 0, z_sum = 0;
            for (int i = 0; i < NUM_READINGS; ++i) {
                if (senseGetAccelG(x, y, z)) {
                    x_sum += x;
                    y_sum += y;
                    z_sum += z;
                } else {
                    cout << "Error. No measures." << endl;
                    break;
                }
                // Small delay between readings
                sleep_for(milliseconds(10));
            }

            // Calculate the average
            x = x_sum / NUM_READINGS;
            y = y_sum / NUM_READINGS;
            z = z_sum / NUM_READINGS;

            cout << fixed << setprecision(6) << " x = " << x << " y = " << y
                 << " z = " << z;
            // Calclulate the norm of the acceleration vector
            a = sqrt(x * x + y * y + z * z);
            cout << "\t\t|a| = " << a << endl;
            if (a < a_min) a_min = a;
        }

        cout << "Minimum norm of the acceleration vector: " << a_min << endl;

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
