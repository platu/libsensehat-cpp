#ifndef __SENSEHAT_H__
#define __SENSEHAT_H__

#include <cstdint>
#include <cstdbool>

#ifdef __cplusplus
extern "C" {
#endif	

    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>

#ifdef __cplusplus
}
#endif	

#include <png.h>

#include <RTIMULib.h>
#include <RTMath.h>
#define G_2_MPSS 9.80665

// The LED Array is handled by a dedicated microcontroller
// It must be updated in a single shot by writing
// 192 bytes starting at register 0
// The memory is laid out in each row like this:
// RRRRRRRRGGGGGGGGBBBBBBBB
// Each byte can have 64 unique levels
// Colors
#define COLORS 3
#define _R 0
#define _G 1
#define _B 2
// LED array width
#define SENSE_LED_WIDTH 8
// Number of pixels in the bitmap
#define SENSE_PIXELS (SENSE_LED_WIDTH * SENSE_LED_WIDTH)

// RGB 565 data types -> based on a single 16 bit integer
typedef uint16_t rgb565_pixel_t;
typedef struct { rgb565_pixel_t array [SENSE_LED_WIDTH][SENSE_LED_WIDTH]; } rgb565_pixels_t;

// RGB data types -> based on array of 3 bytes
typedef struct { uint8_t color [COLORS]; } rgb_pixel_t;
typedef struct { rgb_pixel_t array [SENSE_LED_WIDTH][SENSE_LED_WIDTH]; } rgb_pixels_t;

// Joystick codes 
#define KEY_ENTER 28
#define KEY_UP 103
#define KEY_LEFT 105
#define KEY_RIGHT 106
#define KEY_DOWN 108
#define KEY_RELEASED 0
#define KEY_PRESSED 1
#define KEY_HELD 2

// Joystick data type
typedef struct {
	float timestamp;
	int action, state;
} stick_t;

// Initialize before any job
bool senseInit();
// Shut down when job is done
void senseShutdown();

// Clear LED store and shut all the LEDs
void senseClear();

// Lower LED light intensity
void senseSetLowLight(bool low);

// Convert from array of 3 bytes to rgb565
rgb565_pixel_t sensePackPixel(rgb_pixel_t);
// Convert from rgb565 to an array of 3 bytes
rgb_pixel_t senseUnPackPixel(rgb565_pixel_t);

// Read one single pixel
rgb565_pixel_t senseGetRGB565pixel(unsigned int, unsigned int);
rgb_pixel_t senseGetRGBpixel(unsigned int, unsigned int);
const auto senseGetPixel = senseGetRGBpixel;

// Write one signle pixel
bool senseSetRGB565pixel(unsigned int, unsigned int, rgb565_pixel_t);
bool senseSetRGBpixel(unsigned int, unsigned int, uint8_t, uint8_t, uint8_t);
const auto senseSetPixel = senseSetRGBpixel;

// Read all pixels at once
rgb565_pixels_t senseGetRGB565pixels();
rgb_pixels_t senseGetRGBpixels();
const auto senseGetPixels = senseGetRGBpixels;

// Write all pixels at once
void senseSetRGB565pixels(rgb565_pixels_t );
void senseSetRGBpixels(rgb_pixels_t );
void senseRGBClear(uint8_t , uint8_t , uint8_t );
const auto senseSetPixels = senseSetRGBpixels;

// Horizontal and Vertical flip of all pixels
rgb_pixels_t senseFlip_h(bool);
rgb_pixels_t senseFlip_v(bool);

// Pixels rotation of 90, 180, and 270 degrees clockwise
rgb_pixels_t senseRotation(unsigned int);

// Print a single character with(out) foreground and background color
void senseShowRGB565ColoredLetter(char, rgb565_pixel_t, rgb565_pixel_t);
void senseShowRGBColoredLetter(char, rgb_pixel_t, rgb_pixel_t);
const auto senseShowColoredLetter = senseShowRGBColoredLetter;
void senseShowLetter(char);

// Print a scrolling text line with(out) foreground and background color
void senseShowRGB565ColoredMessage(char *, rgb565_pixel_t, rgb565_pixel_t);
void senseShowRGBColoredMessage(char *, rgb_pixel_t, rgb_pixel_t);
void senseShowMessage(char * );

// HTS221 Humidity sensor
bool senseGetTempHumid(double *, double *);
double senseGetHumidity();
double senseGetTemperatureFromHumidity();

// LPS25H Pressure sensor
bool senseGetTempPressure(double *, double *);
double senseGetPressure();
double senseGetTemperatureFromPressure();

// LSM9DS1 IMU
void senseSetIMUConfig(bool, bool, bool);

bool senseGetOrientationRadians(double *, double *, double *);
bool senseGetOrientationDegrees(double *, double *, double *);
double senseGetCompass();

bool senseGetGyroRadians(double *, double *, double *);
bool senseGetGyroDegrees(double *, double *, double *);

bool senseGetAccelG(double *, double *, double *);
bool senseGetAccelMPSS(double *, double *, double *);

// Joystick
stick_t senseWaitForJoystick();

void senseSetJoystickWaitTime(long int, long int);

bool senseGetJoystickEvent(stick_t *);

#endif // SENSEHAT_H
