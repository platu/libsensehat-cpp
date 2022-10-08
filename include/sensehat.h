#ifndef __SENSEHAT_H__
#define __SENSEHAT_H__

#include "../include/HTS221_Registers.h"
#include "../include/LPS25H_Registers.h"
#include "../include/LSM9DS1_Registers.h"
#include "../include/LSM9DS1_Types.h"
#include "../include/TCS34725_Registers.h"

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

// GPIOD library
#include <gpiod.h>

// Number of colors for a unique pixel
#define COLORS 3
// Red color index
#define _R 0
// Green color index
#define _G 1
// Blue color index
#define _B 2
// LED matrix width
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

/// \typedef GPIO pin values
/// \details on: high level / off: low level
typedef enum {on = 1, off = 0} gpio_state_t;
typedef enum {in = 1, out = 0} gpio_dir_t;

#define	GPIO_CONSUMER	"SenseHatLib"

/// \brief Initialize file handles and communications
/// \details led matrix framebuffer, josytick input, IMU calibration
/// parameters, character set, GPIO chip
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
rgb565_pixel_t senseGetRGB565pixel(int x, int y);

/// \brief Read the color attributes of one single pixel from its coordinates
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \return color attributes of the pixel encoded in a array of 3 bytes: rgb_pixel_t type
rgb_pixel_t senseGetRGBpixel(int x, int y);

/// \brief Read the color attributes of one single pixel from its coordinates
const auto senseGetPixel = senseGetRGBpixel;

/// \brief Write the color attributes of one single pixel in RGB565 format
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \return bool false if something went wrong
bool senseSetRGB565pixel(int x, int y, rgb565_pixel_t rgb565);

/// \brief Write the color attributes of one single pixel with the 3 RGB values
/// \param[in] x row number [0..7]
/// \param[in] y column number [0..7]
/// \param[in] R red value
/// \param[in] G green value
/// \param[in] B blue value
/// \return bool false if something went wrong
bool senseSetRGBpixel(int x, int y, uint8_t R, uint8_t G, uint8_t B);

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
/// \param[in] bool true to redraw all pixels
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type
rgb_pixels_t senseFlip_h(bool redraw);

/// \brief Vertical flip of all pixels
/// \param[in] bool true to redraw all pixels
/// \return 2 dimensional array of RGB color attributes of rgb_pixel_t type
rgb_pixels_t senseFlip_v(bool redraw);

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

/// \brief Print a scrolling text line with foreground and background color
/// \param[in] msg line to print
/// \param[in] fg foreground color encoded in RGB565 format
/// \param[in] bg background color encoded in RGB565 format
void senseShowRGB565ColoredMessage(std::string msg, rgb565_pixel_t fg, rgb565_pixel_t bg);

/// \brief Print a scrolling text line with foreground and background color
/// \param[in] msg line to print
/// \param[in] fg foreground color of rgb_pixel_t type - array of 3 bytes
/// \param[in] bg background color of rgb_pixel_t type - array of 3 bytes
void senseShowRGBColoredMessage(std::string msg, rgb_pixel_t fg, rgb_pixel_t bg);

/// \brief Print a scrolling text line with white foreground color on black background color
/// \param[in] msg line to print
void senseShowMessage(std::string msg);

// ----------------------
// HTS221 Humidity sensor
// ----------------------

/// \brief Get temperature in °C and relative humidity in % measures from the HTS221 sensor
/// \param[out] t_C temperature in °C
/// \param[out] h_R relative humidity in %
/// \return bool false if somnething went wrong
bool senseGetTempHumid(double &t_C, double &h_R);

/// \brief Get relative humidity measure in % from the HTS221 sensor
/// \return relative humidity in %
double senseGetHumidity();

/// \brief Get temperature measure in °C from the HTS221 sensor
/// \return temperature in °C
double senseGetTemperatureFromHumidity();

// ----------------------
// LPS25H Pressure sensor
// ----------------------

/// \brief Get temperature in °C and pressure in hPa measures from the LPS25H sensor
/// \param[out] t_C temperature in °C
/// \param[out] p_hPa pressure in hPa
/// \return bool false if somnething went wrong
bool senseGetTempPressure(double &t_C, double &p_hPa);

/// \brief Get pressure measure in hPa from the LPS25H sensor
/// \return pressure in hPa
double senseGetPressure();

/// \brief Get temperature measure in °C from the LPS25H sensor
/// \return temperature in °C
double senseGetTemperatureFromPressure();

// ----------------------
// LSM9DS1 IMU
// ----------------------

/// \brief IMU features intialization
/// \param[in] magn true to enable magnetometer
/// \param[in] gyro true to enable gyrometer
/// \param[in] accel true to enable accelerometer
void senseSetIMUConfig(bool magn, bool gyro, bool accel);

/// \brief Get IMU orientation in radians from the magnetometer
/// \param[out] pitch value in radians
/// \param[out] roll value in radians
/// \param[out] yaw value in radians
/// \return bool false if somnething went wrong
bool senseGetOrientationRadians(double &pitch, double &roll, double &yaw);

/// \brief Get IMU orientation in degrees from the magnetometer
/// \param[out] pitch value in degrees
/// \param[out] roll value in degrees
/// \param[out] yaw value in degrees
/// \return bool false if somnething went wrong
bool senseGetOrientationDegrees(double &pitch, double &roll, double &yaw);

/// \brief Get the direction of north in degrees from the magnetometer
/// \details Calls senseSetIMUConfig to disable gyrometer and accelerometer
/// \return direction of north in degrees
double senseGetCompass();

/// \brief Get gyrometer measurements along the 3 axis in radians/s
/// \param[out] pitch value in radians/s
/// \param[out] roll value in radians/s
/// \param[out] yaw value in radians/s
/// \return bool false if somnething went wrong
bool senseGetGyroRadians(double &pitch, double &roll, double &yaw);

/// \brief Get gyrometer measurements along the 3 axis in degrees/s
/// \param[out] pitch value in degrees/s
/// \param[out] roll value in degrees/s
/// \param[out] yaw value in degrees/s
/// \return bool false if somnething went wrong
bool senseGetGyroDegrees(double &picth, double &roll, double &yaw);

/// \brief Get accelerometer measurements along the 3 axis in g
/// \param[out] x value in g
/// \param[out] y value in g
/// \param[out] z value in g
/// \return bool false if somnething went wrong
bool senseGetAccelG(double &x, double &y, double &z);

/// \brief Get accelerometer measurements along the 3 axis in meter per second squared
/// \param[out] x value in mpss
/// \param[out] y value in mpss
/// \param[out] z value in mpss
/// \return bool false if somnething went wrong
bool senseGetAccelMPSS(double &x, double &y, double &z);

// ----------------------
// Joystick
// ----------------------

/// \brief Wait for any joystick event
/// \details This is a blocking function
/// \return stick_t structure contains timestamp, action and state of the joystick button
stick_t senseWaitForJoystick();

/// \brief Define the duration for monotoring joystick events
/// \param[in] sec duration in seconds
/// \param[in] msec duration in milliseconds
void senseSetJoystickWaitTime(long int sec, long int msec);

/// \brief Get joystick event if any
/// \param[out] ev stick_t structure containing timestamp, action and state of the joystick button
/// \return bool true if something happened
bool senseGetJoystickEvent(stick_t &ev);

/// \brief Wait for any joystick ENTER event
/// \details This is a blocking function
/// \return boolean true if the ENTER action happens on joystick
bool senseWaitForJoystickEnter();

// ----------------------
// GPIO pins
// ----------------------

/// \brief Setup GPIO pin configuration and direction
/// \param[in] pin number among 5, 6, 16, 17, 22, 26, 27
/// \param[in] direction: in or out
/// \return bool false if something goes wrong
bool gpioSetConfig(unsigned int pin, gpio_dir_t direction);

/// \brief Set GPIO pin output value to high level (on) or low level (off)
/// \param[in] pin number among 5, 6, 16, 17, 22, 26, 27
/// \param[in] value: on or off
/// \return bool false if something goes wrong
bool gpioSetOutput(unsigned int pin, gpio_state_t val);

/// \brief Get GPIO pin input value
/// \param[in] pin number among 5, 6, 16, 17, 22, 26, 27
/// \return int < 0 if something goes wrong
int gpioGetInput(unsigned int pin);

// ----------------------
// PWM channels
// ----------------------

/// \brief Initialize PWM channel
/// \param[in] channel number 0 or 1
/// \return boolean true if initialization is accepted. Beware of setup time at first run!
bool pwmInit(unsigned int chan);

/// \brief Set PWM channel period in usec
/// \param[in] channel number 0 or 1
/// \param[in] period in microseconds
/// \return boolean true if setup is accepted.
bool pwmPeriod(unsigned int chan, unsigned int period);

/// \brief Set PWM channel duty cycle in percent
/// \param[in] channel number 0 or 1
/// \param[in] duty cycle [0..100] %
/// \return boolean true if setup is accepted.
bool pwmDutyCycle(unsigned int chan, unsigned int percent);

/// \brief Set PWM state to enable or disable
/// \param[in] channel number 0 or 1
/// \param[in] state "0" or "1"
/// \return boolean true if setup is accepted.
bool pwmChangeState(unsigned int chan, std::string state);

/// \brief Enable PWM channel
/// \param[in] channel number 0 or 1
/// \return boolean true if setup is accepted.
bool pwmEnable(unsigned int chan);

/// \brief Disable PWM channel
/// \param[in] channel number 0 or 1
/// \return boolean true if setup is accepted.
bool pwmDisable(unsigned int chan);

// --------------------------
// TCS34725 color detection
// --------------------------

bool colorDetectInit(tcs34725IntegrationTime_t it, tcs34725Gain_t gain);
void colorDetectShutdown();


#endif // __SENSEHAT_H__
