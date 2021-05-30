# How to build and install this library on an Ubuntu server distro

The purpose of this experimental setup is to evaluate the reproducibility of
the development environment with a change in context.

Here we start from the [Ubuntu Server
21.04](https://ubuntu.com/download/raspberry-pi) base install and follow the
track to the libsensehat-cpp build.

## System groups to which the user belongs

The Ubuntu distro doesn't have as many system groups as the Raspberry Pi OS for
hardware communications. The `gpio` and `spi` groups don't exist and the `i2c`
bus is under the control of the `dialout` group.

On the joystick `input` side, there is no difference.

So, the normal user account must belong to the follwoing system groups:

```
$ id | egrep -o '(dialout|input|i2c|gpio|sudo)'
dialout
sudo
input
i2c
gpio
```

If it is not the case, you have to run a command like this: `sudo adduser
ubuntu input`. This adds the user `ubuntu` to the `input` system group. Then,
you have to log off and on back to see that the group assignment is correct
with the `id` command for instance.   

## Package dependencies

In order to build the libsensehat-cpp library, the 2c and PNG development
packages are required:

```
$ sudo apt install libi2c-dev libpng-dev libgpiod-dev
```

## RTIMULib2 build from source

This is the most difficult part of the job.

1. Start by the QT5 development package dependency

```
sudo apt install qtbase5-dev build-essential cmake
```

2. Then, clone the library git repository

```
git clone https://github.com/RTIMULib/RTIMULib2.git
```

3. Finally, build and install RTIMULib2

```
cd RTIMULib2/Linux
mkdir build && cd build
cmake -DQT5=1 ..
ionice -c3 make
sudo make install
sudo ldconfig
```

The make task takes a looooooonng time.

## libsensehat-cpp build and install

1. Clone the git repository

```
git clone https://github.com/platu/libsensehat-cpp.git
```

2. Build and install the library

```
cd libsensehat-cpp
make
```

You are now ready to run the example programs.

At the time of writing this, there is a conflict with another i2c component
named MS5611 which is not present on my RPi3 + Sense HAT.

The `I2C read error from 118, 162 - Failed to read MS5611 calibration data`
error message has no effect.

```
cat /sys/firmware/devicetree/base/model && echo
Raspberry Pi 3 Model B Rev 1.2
```

```
i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- 1c -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- UU -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- 5c -- -- 5f
60: -- -- -- -- -- -- -- -- -- -- 6a -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

```
./examples/02_setRGBpixelsRainbow.o
Settings file RTIMULib.ini loaded
Using fusion algorithm RTQF
IMU is opening
min/max compass calibration not in use
Ellipsoid compass calibration not in use
Accel calibration not in use
LSM9DS1 init complete
I2C read error from 118, 162 - Failed to read MS5611 calibration data
-------------------------------
Sense Hat initialization Ok.
1800 loops left / Elapsed time = 1121 ms
1600 loops left / Elapsed time = 1102 ms
1400 loops left / Elapsed time = 1119 ms
1200 loops left / Elapsed time = 1119 ms
1000 loops left / Elapsed time = 1119 ms
 800 loops left / Elapsed time = 1119 ms
 600 loops left / Elapsed time = 1119 ms
 400 loops left / Elapsed time = 1119 ms
 200 loops left / Elapsed time = 1119 ms

Waiting for keypress.
-------------------------------
Sense Hat shut down.
```
