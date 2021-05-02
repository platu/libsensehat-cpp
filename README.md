# Yet another (Raspberry Pi + SenseHat) library in C++ programming language

## Foreword 
This repository aims to resume the Python API in C++ programming language.

The functions provided by this library are intended for students who are taking their first steps in programming.

Therefore, we use a very small subset of C++ programming language.
* No classes. Okaaayy! Don't slap me, even virtually.
* Typed input/output through iostream. Almost avoids burdens of C stdio formatting.
* Use of IMU RTIMULib library already written in C++. Much more convenient to get magnetic field measures from LSM9DS1 registers.

## Install en build

1. Your Raspberry Pi user account must belong to a few system groups to access hardware devices and install the library files once it is compiled.
  ```
  $ id | grep -o '(i2c|sudo)'
  ```
