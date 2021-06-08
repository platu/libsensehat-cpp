# Raspberry Pi 4 B with Sense HAT

When we use the Raspberry Pi 4 B in headless mode with Sense HAT, there is a
potential issue with the Sense HAT.

Therefore, here are a few informations.

## HDMI hotplug

In the `/boot/config.txt`, it is advised to uncomment the
`hdmi_force_hotplug=1` entry. You can confirm the parameter is active with the
following command.

```bash
grep hdmi_force_hotplug /boot/config.txt
hdmi_force_hotplug=1
```

## Sense HAT modules and device addresses

A few tests to check Sense HAT is there and available.

```bash
lsmod | grep sense
rpisense_js            20480  0
rpisense_fb            16384  0
rpisense_core          16384  2 rpisense_js,rpisense_fb
syscopyarea            16384  2 drm_kms_helper,rpisense_fb
sysfillrect            16384  2 drm_kms_helper,rpisense_fb
sysimgblt              16384  2 drm_kms_helper,rpisense_fb
fb_sys_fops            16384  2 drm_kms_helper,rpisense_fb

```

List of i2c-1 bus active addresses with `i2cdetect` from `i2c-tools` package.

```bash
i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- 1c -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- UU -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- 5c -- -- 5f
60: -- -- -- -- -- -- -- -- -- -- 6a -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

## GPIO states

The `raspi-gpio` allows to query any pin.

```bash
raspi-gpio get 13
GPIO 13: level=0 fsel=4 alt=0 func=PWM0_1 pull=DOWN
```
