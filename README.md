# Yet another (Raspberry Pi + Sense HAT) library in C/C++ programming language

## Foreword 

This repository aims to resume the [Python Sense
HAT](https://github.com/astro-pi/python-sense-hat) API in C++ programming
language. The functions provided by this library are intended for students who
are taking their first steps in programming. Therefore, we use a very small
subset of C++ programming language.

* No classes. Okaaaayyy! I know. Don't slap me, even virtually.
* Typed input/output through iostream. Almost avoids burdens of C stdio formatting.
* Use of IMU RTIMULib library already written in C++. Much more convenient to
  get magnetic field measures from LSM9DS1 registers.

The code in this repository has started as a compilation from different other
repositories. It then evolved with the addition of GPIO and PWM functions to
become a full-fledged library. Many thanks are due to the developers of the
following projects:

* [libsense](https://github.com/moshegottlieb/libsense)
* [Raspberry Pi Sense-HAT add-on board](https://github.com/davebm1/c-sense-hat)
* [Sense Hat Unchained](https://github.com/bitbank2/sense_hat_unchained)

## Install and build

Open a terminal on your Raspberry Pi.

1. Your Raspberry Pi user account must belong to a few system groups to access
   hardware devices and install the library file once it is compiled.

The result of the `id` command below shows the user account belongs to the
required system groups.
 ```bash
 $ id | egrep -o '(input|i2c|gpio|spi|sudo)'
 sudo
 input
 gpio
 i2c
 spi
 ```

  Check that the sense-hat packages are already there.
 ```bash
 $ apt search sense-hat | grep install

 python-sense-hat/testing,now 2.2.0-1 armhf  [installé]
 python3-sense-hat/testing,now 2.2.0-1 armhf  [installé]
 sense-hat/testing,now 1.2 all  [installé]
 ```

2. Install development library packages
 ```bash
 $ sudo apt install libi2c-dev libpng-dev libgpiod-dev
 ```

3. Clone this repository
 ```bash
 $ git clone https://github.com/platu/libsensehat-cpp.git
 ```

4. Build the library and compile the example programs
 ```bash
 $ cd libsensehat-cpp/
 pi@rpixx:~/libsensehat-cpp $ make
 g++ -Wall -Wextra -Werror -pedantic -std=gnu++2a -c -fPIC -o src/sensehat.o src/sensehat.cpp -lpng
 g++ -Wall -Wextra -Werror -pedantic -std=gnu++2a -g -shared -Wl,-soname,libsensehat-c++.so -o lib/libsensehat-c++.so.0 src/sensehat.o
 sudo cp lib/libsensehat-c++.so.0 /usr/local/lib/libsensehat-c++.so.0
 sudo sh -c "cd /usr/local/lib && ln -sf libsensehat-c++.so.0 libsensehat-c++.so"
 sudo ldconfig
 cd examples && make
 make[1] : on entre dans le répertoire « /home/etu/libsensehat-cpp/examples »
 g++ -Wall -Wextra -Werror -pedantic -std=gnu++2a -o 01_setRGBpixel.o 01_setRGBpixel.cpp -lsensehat-c++ -lpng -li2c -lm -lRTIMULib
 ```
 Depending on the number of example programs, compilation may take some time.

You're done ! It is now time to open example files and run your own tests. There is a generic [Makefile](examples/Makefile) in the [examples](examples/) directory that you can copy and adapt to your needs.

<img src="https://inetdoc.net/images/sensehat.jpg" width="384px" />

## Example programs

Almost every function has its own example program that illustrates how it works. Source file numbering in the directory named [examples](examples/) designates the category of functions. Here is a list of these categories:
* 01 Get or set a single pixel
* 02 Get or set all pixels
* 03 Flip or rotate all pixels
* 04 Display a character or scroll a message
* 05 HTS221 Humidity sensor and LPS25H Pressure sensor
* 06 LSM9DS1 IMU Orientation and compass
* 07 Joystick events
* 08 GPIO read input or write output on Raspberry Pi pins subset

## Library addons

Once the Sense HAT standard header is replaced by a stacking header, GPIO and PWM pins are available.

<img src="https://inetdoc.net/images/sensehat_stacking.jpg" width="384px" />
