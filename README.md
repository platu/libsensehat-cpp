# Yet another (Raspberry Pi + SenseHat) library in C++ programming language

## Foreword 
This repository aims to resume the Python API in C++ programming language.

The functions provided by this library are intended for students who are taking their first steps in programming.

Therefore, we use a very small subset of C++ programming language.
* No classes. Okaaayy! Don't slap me, even virtually.
* Typed input/output through iostream. Almost avoids burdens of C stdio formatting.
* Use of IMU RTIMULib library already written in C++. Much more convenient to get magnetic field measures from LSM9DS1 registers.

## Install and build

Open a terminal on your Raspberry Pi.

1. Your Raspberry Pi user account must belong to a few system groups to access hardware devices and install the library file once it is compiled.
The result of the `id` command below shows the user account belongs to the required system groups.
 ```
 $ id | egrep -o '(i2c|gpio|spi|sudo)'
 sudo
 gpio
 i2c
 spi
 ```

 Check that the sense-hat packages are already there.
 ```
 $ apt search sense-hat | grep install

 python-sense-hat/testing,now 2.2.0-1 armhf  [installé]
 python3-sense-hat/testing,now 2.2.0-1 armhf  [installé]
 sense-hat/testing,now 1.2 all  [installé]
 ```

2. Install development library packages
 ```
 $ sudo apt install libi2c-dev
 ```

4. Clone this repository
 ```
 $ git clone https://github.com/platu/libsensehat-cpp.git
 ```

3. Build the library and compile the example programs
 ```
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

You're done ! It is now time to open example files and run your own tests.

![SenseHat Rainbow](https://inetdoc.net/images/sensehat.jpg | width=384)
