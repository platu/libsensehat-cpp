# Sense HAT joystick identification

## Event input device numbering

Depending on keyboard, mouse and other input devices plugged in the Raspberry
Pi, the event file handler number changes.

A first way to identify the Sense Hat joystick is to list all input devices.

In the following example, the joystick is held by `event0`.

```bash
cat /proc/bus/input/devices
I: Bus=0018 Vendor=0000 Product=0000 Version=0000
N: Name="Raspberry Pi Sense HAT Joystick"
P: Phys=rpi-sense-joy/input0
S: Sysfs=/devices/virtual/input/input0
U: Uniq=
H: Handlers=kbd event0
B: PROP=0
B: EV=100003
B: KEY=168000000000 10000000
```

## Joystick messages

The package `evtest` is useful to get all possible messages sent by the joystick.

```bash
sudo apt install evtest
```

```bash
evtest
No device specified, trying to scan all of /dev/input/event*
Not running as root, no devices may be available.
Available devices:
/dev/input/event0:	Raspberry Pi Sense HAT Joystick
Select the device event number [0-0]: 0
Input driver version is 1.0.1
Input device ID: bus 0x18 vendor 0x0 product 0x0 version 0x0
Input device name: "Raspberry Pi Sense HAT Joystick"
Supported events:
  Event type 0 (EV_SYN)
  Event type 1 (EV_KEY)
    Event code 28 (KEY_ENTER)
    Event code 103 (KEY_UP)
    Event code 105 (KEY_LEFT)
    Event code 106 (KEY_RIGHT)
    Event code 108 (KEY_DOWN)
Key repeat handling:
  Repeat type 20 (EV_REP)
    Repeat code 0 (REP_DELAY)
      Value    250
    Repeat code 1 (REP_PERIOD)
      Value     33
Properties:
Testing ... (interrupt to exit)
Event: time 1623135587.898300, type 1 (EV_KEY), code 28 (KEY_ENTER), value 1
Event: time 1623135587.898300, -------------- SYN_REPORT ------------
Event: time 1623135588.027093, type 1 (EV_KEY), code 28 (KEY_ENTER), value 0
Event: time 1623135588.027093, -------------- SYN_REPORT ------------

```
