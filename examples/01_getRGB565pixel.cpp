/* File: 01_getRGB565pixel.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * This example program illustrates the senseGetRGB565pixel() function that
 * reads one single pixel encoded in RGB565 format.
 * RGB565 encodes the three colors in a 16 bit integer.
 * Here we use a dedicated type named rgb565_pixel_t.
 *
 * Function prototype:
 *
 * rgb565_pixel_t senserGetRGB565pixel(int, int);
 *       ^-- pixel color             x -^ y -^
 *
 * The program starts by filling colors on the Sense Hat LED matrix,
 * then the user is asked to choose one row and one column to read the color
 * value of this pixel.
 */

#include <assert.h>
#include <termios.h>

#include <iomanip>
#include <iostream>
#include <limits>

#include <sensehat.h>

using namespace std;

// Function to capture a single keypress from the user
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

// Function to check if the entered coordinate is valid
bool isValidCoordinate(int coord) {
    bool valid = false;

    if (coord >= 0 && coord < 8) {
        valid = true;
    }

    return valid;
}

int main() {
    rgb565_pixel_t pix2read, color, mask = 0xf800;
    int x, y;
    int row, col;

    if (senseInit()) {
        cout << "-------------------------------" << endl
             << "Sense Hat initialization Ok." << endl;
        senseClear();

        // Based on the initial color value of mask, we fill the LED matrix
        for (y = 0; y < 8; y++) {
            color = mask;
            for (x = 0; x < 8; x++) {
                senseSetRGB565pixel(x, y, color);
                // We shift color to get the next color in RGB565 format
                color <<= 1;
            }
            mask >>= 2;
        }

        // Next we ask the user to enter pixel row and column
        do {
            cout << "Enter both the row and column numbers (0-7) of the pixel "
                    "to be read: ";
            cin >> row >> col;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (!isValidCoordinate(row)) {
                cout << "Invalid row number. Please enter a value between 0 "
                        "and 7."
                     << endl;
            }
            if (!isValidCoordinate(col)) {
                cout << "Invalid column number. Please enter a value between 0 "
                        "and 7."
                     << endl;
            }
        } while (!isValidCoordinate(row) || !isValidCoordinate(col));

        // Read the pixel in RGB565 format
        pix2read = senseGetRGB565pixel(col, row);
        cout << "Here is the color encoded in RGB565 format." << endl
             << "Hexadecimal:\t0x" << hex << setw(4) << pix2read << endl
             << "Decimal:\t" << dec << pix2read << endl;
        senseClear();
        // We set back the read pixel to verify
        senseSetRGB565pixel(col, row, pix2read);

        cout << endl << "Waiting for keypress." << endl;
        getch();
        senseShutdown();
        cout << "-------------------------------" << endl
             << "Sense Hat shut down." << endl;
    }

    return EXIT_SUCCESS;
}
