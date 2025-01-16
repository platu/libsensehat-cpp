# Yet another Raspberry Pi + Sense HAT C/C++ library

## Foreword

This repository aims to resume the [Python Sense
HAT](https://github.com/astro-pi/python-sense-hat) API in the C/C++ programming
language. The functions provided by this library are intended for students who
are taking their first steps in programming. Therefore, we use a very small
subset of the C++ programming language.

- No classes. Okaaaayyy! I know. Don't slap me, even virtually.
- Typed input/output through iostream. Almost avoids burdens of C stdio formatting.
- Use of IMU RTIMULib library already written in C++. Much more convenient to
  get magnetic field measures from LSM9DS1 registers.
- Use of [libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/) for GPIO functions.
- Use of sysfs for PWM0 and PWM1 output channels.

The code in this repository has been compiled from various other repositories.
It then developed into a full library with the addition of GPIO and PWM
functions.

- [libsense](https://github.com/moshegottlieb/libsense)
- [Raspberry Pi Sense-HAT add-on board](https://github.com/davebm1/c-sense-hat)
- [Sense Hat Unchained](https://github.com/bitbank2/sense_hat_unchained)

## Install and build

Open a terminal on your Raspberry Pi.

1. Your Raspberry Pi user account must belong to a few system groups to access
   hardware devices and install the library file once it is compiled.

The result of the `id` command below shows the user account belongs to the
required system groups.

```bash
id | grep -Eo '(input|i2c|gpio|spi|sudo|video)'
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

Depending on the number of example programs, the compilation may take some time.

You're all done ! Now you can open the example files and run your own tests.
There is a generic [Makefile](examples/Makefile) in the [examples](examples/)
directory which you can copy and modify to suit your needs.

## Start new project

To start a new project, you need to copy the [Makefile](labTemplate/Makefile)
file to your working directory.

```bash
mkdir myProject && cd myProject
cp $HOME/libsensehat-cpp/labTemplate/Makefile .
```

Then, you can copy the example program that is closest to your needs and adapt
it to your project.

```bash
cp $HOME/libsensehat-cpp/examples/01_setRGB565pixel.cpp .
```

Run `make` to compile the program.

```bash
make
```

```bash
g++ -Wall -Wextra -Wduplicated-cond -Wduplicated-branches -Wsign-conversion -Wlogical-op -Wuseless-cast -Werror -pedantic -std=gnu++2a -o 01_setRGB565pixel 01_setRGB565pixel.cpp -lsensehat-c++ -lpng -li2c -lm -lRTIMULib -lgpiod
```

Run the program.

```bash
./01_setRGB565pixel
```

```bash
Settings file not found. Using defaults and creating settings file
Detected LSM9DS1 at standard/standard address
Using fusion algorithm RTQF
Detected LPS25H at standard address
Sense Hat LED matrix points to device /dev/fb0
8x8, 16bpp
IMU is opening
min/max compass calibration not in use
Ellipsoid compass calibration not in use
Accel calibration not in use
LSM9DS1 init complete
Joystick points to device event4
-------------------------------
Sense Hat initialization Ok.
  [ f800 ]   [ fc00 ]   [ ffe0 ]   [ 7e0 ]   [ 7ff ]   [ 1f ]   [ f81f ]   [ fc10 ]
Waiting for keypress.
-------------------------------
Sense Hat shut down.
```

In addition to this manual method, students run the [getLab.sh](getLab.sh)
script to create a new project directory.
The script creates a new project directory in the `$HOME/cpp` location and
copies the Makefile and VScode settings files into it. Here is an example of how
to use the script.

```bash
$HOME/libsensehat-cpp/getLab.sh myProject
```

When using Visual Studio Code, the two compulsory extensions are:

- C++ Extension Pack
- Makefile Tools

## Example programs

Almost every function has its own example program that illustrates how it
works. Source file numbering in the directory named [examples](examples/)
designates the category of functions. Here is a list of these categories:

- 01 Get or set a single pixel
- 02 Get or set all pixels
- 03 Flip or rotate all pixels
- 04 Display a character or scroll a message
- 05 HTS221 Humidity sensor and LPS25H Pressure sensor
- 06 LSM9DS1 IMU Orientation and compass
- 07 Joystick events
- 08 GPIO read input or write output on Raspberry Pi pins subset
- 09 2 PWM channels
- 10 Color detection based on TCS34725 (work in progress ...)
- 11 Console keyboard events routines

## Library addons

Once the Sense HAT standard header is replaced by a stacking header, GPIO and
PWM pins are available.

<img src="https://inetdoc.net/images/sensehat_stacking.jpg" width="384px" />
