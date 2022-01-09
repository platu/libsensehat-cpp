# Use 2 PWM channels in addition to the Sense HAT

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

## Add a dtoverlay entry in /boot/config.txt

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

We can create a small shell script to be run at system startup through a
systemd unit.

1. Create the shell script

 ```bash
 cat << 'EOF' | sudo tee -a /usr/local/sbin/pwm-export.sh
 #!/bin/bash

 # file: /usr/local/sbin/pwm-export.sh
 #
 # Script run once at system startup by the pwm-export systemd service
 # https://github.com/platu/libsensehat-cpp/blob/main/docs/pwm.md

 for CHAN in 0 1
 do
	if [[ ! -d /sys/class/pwm/pwmchip0/pwm${CHAN} ]]
	then
		echo ${CHAN} > /sys/class/pwm/pwmchip0/export
		echo "PWM channel ${CHAN} exported" | systemd-cat -p info
	fi
 done
 EOF
 ```

2. It has to be executable

 ```bash
 sudo chmod +x /usr/local/sbin/pwm-export.sh
 ```

3. Create a systemd unit

 The unit file is named `pwm-export.service`

 ```bash
 cat << EOF | sudo tee -a /etc/systemd/system/pwm-export.service
 [Unit]
 Description=PWM channels export to sysfs
 After=local-fs.target

 [Service]
 ExecStart=/usr/local/sbin/pwm-export.sh
 Type=oneshot

 [Install]
 WantedBy=multi-user.target
 EOF
 ```

 This unit file has to be enabled

 ```bash
 sudo systemctl daemon-reload
 sudo systemctl enable pwm-export.service
 sudo systemctl start pwm-export.service
 ```

 Then we can check that the 2 channels were exported to sysfs

 ```bash
 systemctl status pwm-export.service
 ● pwm-export.service - PWM channels export to sysfs
    Loaded: loaded (/etc/systemd/system/pwm-export.service; enabled; vendor preset: enabled)
    Active: inactive (dead) since Mon 2021-06-14 16:31:24 CEST; 7min ago
   Process: 468 ExecStart=/usr/local/sbin/pwm-export.sh (code=exited, status=0/SUCCESS)
  Main PID: 468 (code=exited, status=0/SUCCESS)

 juin 14 16:31:23 picodev2 systemd[1]: Starting PWM channels export to sysfs...
 juin 14 16:31:24 picodev2 cat[497]: PWM channel 1 exported
 juin 14 16:31:24 picodev2 systemd[1]: pwm-export.service: Succeeded.
 juin 14 16:31:24 picodev2 systemd[1]: Started PWM channels export to sysfs.
 ```

## Check PWM channels parameters are available

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
