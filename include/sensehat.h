#ifndef __SENSEHAT_H__
#define __SENSEHAT_H__

#include <cstdint>
#include <cstdbool>

// I2C libraries -> Humidity/Pressure/Temperature
#ifdef __cplusplus
extern "C" {
#endif	

    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>

#ifdef __cplusplus
}
#endif	

// PNG library -> Letters
#include <png.h>

// IMU library -> Accel/Gyro/Magn
#include <RTIMULib.h>
#include <RTMath.h>
#define G_2_MPSS 9.80665

// Colors
#define COLORS 3
#define _R 0
#define _G 1
#define _B 2
// LED array width
#define SENSE_LED_WIDTH 8
// Number of pixels in the bitmap
#define SENSE_PIXELS (SENSE_LED_WIDTH * SENSE_LED_WIDTH)

/// \brief color attributes of a pixel encoded in an integer of rgb565_pixel_t type
/// \details RGB565 format represents the 3 colors in a 16 bit integer
/// \details The bits are arranged this way: RRRRRGGGGGGBBBBB
typedef uint16_t rgb565_pixel_t;

/// \brief led matrix 2 dimensional array of pixels encoded in rgb565_pixel_t type
typedef struct { rgb565_pixel_t array [SENSE_LED_WIDTH][SENSE_LED_WIDTH]; } rgb565_pixels_t;

/// \brief color attributes of a pixel encoded in an array of 3 bytes
/// \details the 3 bytes are in R, G, B order
typedef struct { uint8_t color [COLORS]; } rgb_pixel_t;

/// \brief led matrix 2 dimensional array with pixels encoded in rgb_pixel_t type
typedef struct { rgb_pixel_t array [SENSE_LED_WIDTH][SENSE_LED_WIDTH]; } rgb_pixels_t;

/// \brief Joystick codes and states
#define KEY_ENTER 28
#define KEY_UP 103
#define KEY_LEFT 105
#define KEY_RIGHT 106
#define KEY_DOWN 108
#define KEY_RELEASED 0
#define KEY_PRESSED 1
#define KEY_HELD 2

/// \typedef Joystick data type
/// \details timestamp: float with decimal part in milliseconds
/// action: KEY_ENTER, KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN
/// state: KEY_RELEASED, KEY_PRESSED, KEY_HELD
typedef struct {
	float timestamp;
	int action, state;
} stick_t;

/// \brief Initialize file handles and communications
/// \details led matrix framebuffer,
///  josytick input,
///  IMU calibration parameters,
///  and character set
/// \return bool false if something went wrong
bool senseInit();

/// \brief Close file handles and communications with Sense HAT
void senseShutdown();

/// \brief Clear LED store and shut all the LEDs
void senseClear();

/// \brief Lower LED light intensity
/// \param[in] low true if color values must be lowered to limit power consomption 
void senseSetLowLight(bool low);

/// \brief Convert from array of 3 color bytes to rgb565
/// \param[in] rgb R, G, B color array of rgb_pixel_t type
/// \return RGB565 encoded integer of rgb565_pixel_t type
rgb565_pixel_t sensePackPixel(rgb_pixel_t rgb);

/// \brief Convert from rgb565 to an array of 3 color bytes [R, G, B]
/// \param[in] rgb565 - encoded integer of rgb565_pixel_t type
/// \return R, G, B color array of rgb_pixel_t type
rgb_pixel_t senseUnPackPixel(rgb565_pixel_t rgb565);

/// \brief Read the color attributes of one single pixel from its coordinates
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \return color attrubutes of the pixel encoded in RGB565 format: rgb565_pixel_t type
rgb565_pixel_t senseGetRGB565pixel(unsigned int x, unsigned int y);

/// \brief Read the color attributes of one single pixel from its coordinates
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \return color attributes of the pixel encoded in a array of 3 bytes: rgb_pixel_t type
rgb_pixel_t senseGetRGBpixel(unsigned int x, unsigned int y);

/// \brief Read the color attributes of one single pixel from its coordinates
const auto senseGetPixel = senseGetRGBpixel;

/// \brief Write the color attributes of one single pixel in RGB565 format
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \return bool false if something went wrong
bool senseSetRGB565pixel(unsigned int x, unsigned int y, rgb565_pixel_t rgb565);

/// \brief Write the color attributes of one single pixel with the 3 RGB values
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \param[in] R red value
/// \param[in] G green value
/// \param[in] B blue value
/// \return bool false if something went wrong
bool senseSetRGBpixel(unsigned int x, unsigned int y, uint8_t R, uint8_t G, uint8_t B);

/// \brief Write the color attributes of one single pixel with the 3 RGB values
const auto senseSetPixel = senseSetRGBpixel;

/// \brief Read color attibutes of all pixels at once
/// \return 2 dimensional array of integers in RGB565 format: rgb565_pixel_t type
rgb565_pixels_t senseGetRGB565pixels();

/// \brief Read color attibutes of all pixels at once
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type
rgb_pixels_t senseGetRGBpixels();

/// \brief Read color attibutes of all pixels at once
const auto senseGetPixels = senseGetRGBpixels;

/// \brief Write color attributes of all pixels at once
/// \param rgb565_map 2 dimensional array of integers in RGB565 format: rgb565_pixel_t type 
void senseSetRGB565pixels(rgb565_pixels_t rgb565_map);

/// \brief Write color attributes of all pixels at once
/// \param rgb_map 2 dimensional array of RGB color attributes of rgb_pixel_t type 
void senseSetRGBpixels(rgb_pixels_t rgb_map);

/// \brief Set all pixels with the same color attributes
/// \param[in] R red value
/// \param[in] G green value
/// \param[in] B blue value
void senseRGBClear(uint8_t R, uint8_t G, uint8_t B);

/// \brief Set all pixels with the same color attributes
const auto senseSetPixels = senseSetRGBpixels;

/// \brief Horizontal flip of all pixels
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type 
rgb_pixels_t senseFlip_h(bool);

/// \brief Vertical flip of all pixels
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type 
rgb_pixels_t senseFlip_v(bool);

/// \brief Rotate all pixels clockwise with a defined angle value
/// \param[in] angle [90, 180, 270]
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type 
rgb_pixels_t senseRotation(unsigned int angle);

/// \brief Print a single character with foreground and background color
/// \param[in] c character to print
/// \param[in] fg foreground color encoded in RGB565 format
/// \param[in] bg background color encoded in RGB565 format 
void senseShowRGB565ColoredLetter(char c, rgb565_pixel_t fg, rgb565_pixel_t bg);

/// \brief Print a single character with foreground and background color
/// \param[in] c character to print
/// \param[in] fg foreground color of rgb_pixel_t type - array of 3 bytes 
/// \param[in] bg background color of rgb_pixel_t type - array of 3 bytes
void senseShowRGBColoredLetter(char c, rgb_pixel_t fg, rgb_pixel_t bg);

/// \brief Print a single character with foreground and background color
const auto senseShowColoredLetter = senseShowRGBColoredLetter;

/// \brief Print a single character with white foreground color on black background color
/// \param[in] c character to print
void senseShowLetter(char c);

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

#endif // __SENSEHAT_H__
