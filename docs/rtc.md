# Using UPS (With RTC & Coulometer) For Raspberry Pi 4B/3B+/3B

The battery pack UPS board is referenced [EP-0118](https://wiki.52pi.com/index.php?title=EP-0118)

Here are the steps to use its RTC DS1307 chip as a system RTC

1. Detect the chip on I2C bus number 1 at 0x68 address

```
i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- 1c -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: 40 -- -- -- -- -- UU -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- 5c -- -- 5f
60: -- -- -- -- -- -- -- -- UU -- 6a -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

2. Edit the /boot/config.txt

  Add this new entry :

```
dtoverlay=i2c-rtc,ds1307
```

  In our setup, the entry `dtparam=i2c_arm=on` is already active.

3. Remove fake hardware clock set by default

```
sudo apt -y purge fake-hwclock
```

  Edit the `/lib/udev/hwclock-set` to comment out RTC initialization instructions

```
#!/bin/sh
# Reset the System Clock to UTC if the hardware clock from which it
# was copied by the kernel was in localtime.

dev=$1

#if [ -e /run/systemd/system ] ; then
#    exit 0
#fi

/sbin/hwclock --rtc=$dev --systz
/sbin/hwclock --rtc=$dev --hctosys
```

4. Reboot and check RTC is active

  Check kernel module is loaded

```
lsmod | grep rtc
rtc_ds1307             32768  0
regmap_i2c             16384  1 rtc_ds1307
```

  Check RTC setup at boot time

```
sudo dmesg | grep ' rtc'
[    8.138816] rtc-ds1307 1-0068: registered as rtc0
[    8.141436] rtc-ds1307 1-0068: setting system clock to 2022-12-06T13:12:43 UTC (1670332363)
```

Here we go !
