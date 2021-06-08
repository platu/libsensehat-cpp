# Add 2 PWM channels to the Sense HAT

According to [Sense HAT pinout page](https://en.pinout.xyz/pinout/sense_hat), 2
PWM output pins are available.

* Pin 12 / BCM 18 / PWM0
* Pin 33 / BCM 13 / PWM1

The reference web page is: [Using the Raspberry Pi hardware PWM timers](https://jumpnowtek.com/rpi/Using-the-Raspberry-Pi-Hardware-PWM-timers.html)

This library tests are run with Raspberry Pi OS ARM64.

```bash
uname -am
Linux picodev2 5.10.39-v8+ #1421 SMP PREEMPT Tue May 25 11:04:26 BST 2021 aarch64 GNU/Linux
```

## dtoverlay entry in /boot/config.txt

First, we have to add a dtoverlay entry to the `/boot/config.txt` file to be
able to manage the 2 PWM channels.

The following entry sets BCM 18 as PWM0 and BCM 13 as PWM1. Only BCM 13 has to
be changed from "default" pinout.

```bash
echo "dtoverlay=pwm-2chan,pin2=13,func2=4" | sudo tee --append /boot/config.txt
```

After reboot of the Raspberry Pi, we can check that PWM kernel module is loaded.

```bash
lsmod | grep pwm
pwm_bcm2835            16384  2
```

## Shell export versus library function

At the very first run it is advised to do the `export` job manually as it takes
"some time" to load the chip configuration within the `/sys/class/pwm/pwmchip0`
tree.

```bash
echo 0 /sys/class/pwm/pwmchip0/export
echo 1 /sys/class/pwm/pwmchip0/export
```

Once this is done, PWM functions are available for programming.

```bash
ls /sys/class/pwm/pwmchip0/pwm0
capture  duty_cycle  enable  period  polarity  power  uevent

ls /sys/class/pwm/pwmchip0/pwm1
capture  duty_cycle  enable  period  polarity  power  uevent
```

Then, we can run the `09_pwmLED` example program on both channels.

```bash
cd ~/libsensehat-cpp
./examples/09_pwmLED -c 0
./examples/09_pwmLED -c 1
```
