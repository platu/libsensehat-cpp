/******************************************************************************
LSM9DS1_Types.h
SFE_LSM9DS1 Library - LSM9DS1 Types and Enumerations
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 21, 2015
https://github.com/sparkfun/LSM9DS1_Breakout

This file defines all types and enumerations used by the LSM9DS1 class.

Development environment specifics:
	IDE: Arduino 1.6.0
	Hardware Platform: Arduino Uno
	LSM9DS1 Breakout Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __LSM9DS1_Types_H__
#define __LSM9DS1_Types_H__

#include "LSM9DS1_Registers.h"

// accel_scale defines all possible FSR's of the accelerometer:
enum accel_scale
{
	A_SCALE_2G,  // 00:  2g
	A_SCALE_16G, // 01:  16g
	A_SCALE_4G,  // 10:  4g
	A_SCALE_8G   // 11:  8g
};

// gyro_scale defines the possible full-scale ranges of the gyroscope:
enum gyro_scale
{
	G_SCALE_245DPS,  // 00:  245 degrees per second
	G_SCALE_500DPS,  // 01:  500 dps
	G_SCALE_2000DPS, // 11:  2000 dps
};

// mag_scale defines all possible FSR's of the magnetometer:
enum mag_scale
{
	M_SCALE_4GS,  // 00:  4Gs
	M_SCALE_8GS,  // 01:  8Gs
	M_SCALE_12GS, // 10:  12Gs
	M_SCALE_16GS, // 11:  16Gs
};

// gyro_odr defines all possible data rate/bandwidth combos of the gyro:
enum gyro_odr
{
	//! TODO
	G_ODR_PD,  // Power down (0)
	G_ODR_149, // 14.9 Hz (1)
	G_ODR_595, // 59.5 Hz (2)
	G_ODR_119, // 119 Hz (3)
	G_ODR_238, // 238 Hz (4)
	G_ODR_476, // 476 Hz (5)
	G_ODR_952  // 952 Hz (6)
};
// accel_oder defines all possible output data rates of the accelerometer:
enum accel_odr
{
	XL_POWER_DOWN, // Power-down mode (0x0)
	XL_ODR_10,	 // 10 Hz (0x1)
	XL_ODR_50,	 // 50 Hz (0x02)
	XL_ODR_119,	// 119 Hz (0x3)
	XL_ODR_238,	// 238 Hz (0x4)
	XL_ODR_476,	// 476 Hz (0x5)
	XL_ODR_952	 // 952 Hz (0x6)
};

// accel_abw defines all possible anti-aliasing filter rates of the accelerometer:
enum accel_abw
{
	A_ABW_408, // 408 Hz (0x0)
	A_ABW_211, // 211 Hz (0x1)
	A_ABW_105, // 105 Hz (0x2)
	A_ABW_50,  //  50 Hz (0x3)
};

// mag_odr defines all possible output data rates of the magnetometer:
enum mag_odr
{
	M_ODR_0625, // 0.625 Hz (0)
	M_ODR_125,  // 1.25 Hz (1)
	M_ODR_250,  // 2.5 Hz (2)
	M_ODR_5,	// 5 Hz (3)
	M_ODR_10,   // 10 Hz (4)
	M_ODR_20,   // 20 Hz (5)
	M_ODR_40,   // 40 Hz (6)
	M_ODR_80	// 80 Hz (7)
};

typedef struct
{
	// Gyroscope settings:
	uint8_t enabled;
	uint16_t scale; // Changed this to 16-bit
	uint8_t sampleRate;
	// New gyro stuff:
	uint8_t bandwidth;
	uint8_t lowPowerEnable;
	uint8_t HPFEnable;
	uint8_t HPFCutoff;
	uint8_t flipX;
	uint8_t flipY;
	uint8_t flipZ;
	uint8_t orientation;
	uint8_t enableX;
	uint8_t enableY;
	uint8_t enableZ;
	uint8_t latchInterrupt;
} gyroSettings;

typedef struct
{
	// Accelerometer settings:
	uint8_t enabled;
	uint8_t scale;
	uint8_t sampleRate;
	// New accel stuff:
	uint8_t enableX;
	uint8_t enableY;
	uint8_t enableZ;
	int8_t bandwidth;
	uint8_t highResEnable;
	uint8_t highResBandwidth;
} accelSettings;

typedef struct
{
	// Magnetometer settings:
	uint8_t enabled;
	uint8_t scale;
	uint8_t sampleRate;
	// New mag stuff:
	uint8_t tempCompensationEnable;
	uint8_t XYPerformance;
	uint8_t ZPerformance;
	uint8_t lowPowerEnable;
	uint8_t operatingMode;
} magSettings;

typedef struct
{
	// Temperature settings
	uint8_t enabled;
} tempSettings;

typedef struct
{
	gyroSettings gyro;
	accelSettings accel;
	magSettings mag;

	tempSettings temp;
} IMUSettings;

typedef enum {
	X_AXIS,
	Y_AXIS,
	Z_AXIS,
	ALL_AXIS
} lsm9ds1_axis;

typedef enum
{
	FIFO_OFF = 0,
	FIFO_THS = 1,
	FIFO_CONT_TRIGGER = 3,
	FIFO_OFF_TRIGGER = 4,
	FIFO_CONT = 6
} fifoMode_type;

#endif
