# Yet another Raspberry Pi + Sense HAT library in C/C++ programming language

## Foreword 

This repository aims to resume the [Python Sense
HAT](https://github.com/astro-pi/python-sense-hat) API in C/C++ programming
language. The functions provided by this library are intended for students who
are taking their first steps in programming. Therefore, we use a very small
subset of C++ programming language.

* No classes. Okaaaayyy! I know. Don't slap me, even virtually.
* Typed input/output through iostream. Almost avoids burdens of C stdio formatting.
* Use of IMU RTIMULib library already written in C++. Much more convenient to
  get magnetic field measures from LSM9DS1 registers.
* Use of [libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/) for GPIO functions.
* Use of sysfs for PWM0 and PWM1 output channels.

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
 id | egrep -o '(input|i2c|gpio|spi|sudo|video)'
 ```
 ```bash=
 sudo
 input
 gpio
 i2c
 spi
 video
 ```

  Check that the sense-hat packages are already there.
 ```bash
 apt search sense-hat | grep install
 ```

 ```bash=
 python3-sense-hat/stable,stable,now 2.6.0-1 all  [installé, automatique]
 sense-hat/stable,stable,now 1.4 all  [installé]
 ```

2. Install development library packages

 ```bash
 sudo apt install libi2c-dev libpng-dev libgpiod-dev
 ```

3. Clone this repository

 ```bash
 git clone https://github.com/platu/libsensehat-cpp.git
 ```

4. Build the library and compile the example programs

 ```bash
 cd libsensehat-cpp/ && make
 ```

 Depending on the number of example programs, compilation may take some time.

 You're done ! It is now time to open example files and run your own tests.
 There is a generic [Makefile](examples/Makefile) in the [examples](examples/)
 directory that you can copy and adapt to your needs.

 ```bash
 ./examples/01_setRGB565pixel
 ```

 ```bash=
 Settings file RTIMULib.ini loaded
 Using fusion algorithm RTQF
 Sense Hat led matrix points to device /dev/fb0
 8x8, 16bpp
 IMU is opening
 Using min/max compass calibration
 Using ellipsoid compass calibration
 Accel calibration not in use
 LSM9DS1 init complete
 Joystick points to device event2
 -------------------------------
 Sense Hat initialization Ok.
   [ f800 ]   [ fc00 ]   [ ffe0 ]   [ 7e0 ]   [ 7ff ]   [ 1f ]   [ f81f ]   [ fc10 ] 
 Waiting for keypress.
 -------------------------------
 Sense Hat shut down.
 ```

<img src="https://inetdoc.net/images/sensehat.jpg" width="384px" />

## Example programs

Almost every function has its own example program that illustrates how it
works. Source file numbering in the directory named [examples](examples/)
designates the category of functions. Here is a list of these categories:

* 01 Get or set a single pixel
* 02 Get or set all pixels
* 03 Flip or rotate all pixels
* 04 Display a character or scroll a message
* 05 HTS221 Humidity sensor and LPS25H Pressure sensor
* 06 LSM9DS1 IMU Orientation and compass
* 07 Joystick events
* 08 GPIO read input or write output on Raspberry Pi pins subset
* 09 2 PWM channels 
* 10 Color detection based on TCS34725 (work in progress ...)
* 11 Console keyboard events routines

## Library addons

Once the Sense HAT standard header is replaced by a stacking header, GPIO and
PWM pins are available.

<img src="https://inetdoc.net/images/sensehat_stacking.jpg" width="384px" />
