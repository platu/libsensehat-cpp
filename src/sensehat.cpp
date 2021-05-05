#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <cstdint>
#include <inttypes.h>
#include <cstdbool>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "../include/HTS221_Registers.h"
#include "../include/LPS25H_Registers.h"
#include "../include/LSM9DS1_Registers.h"
#include "../include/LSM9DS1_Types.h"
#include "../include/sensehat.h"

#define I2C_BUFFER_SIZE 512
#define I2C_ADAPTER 1 
// 8x8 LED array mapped to a microcontroller
#define I2C_LED_DEVICE 0x46
// Buffer size in bytes for the bitmap
#define LEDSTORESIZE (SENSE_PIXELS * 3)
static uint8_t LEDStore[LEDSTORESIZE];

// I2C file handles
#define FILENAMELENGTH 32
static int ledFile = -1; // LED matrix
static bool lowLight_switch = false;
static bool lowLight_state = false;

#define MIN(a,b) (((a)<(b))?(a):(b))
const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

// I2C devices addresses
#define HTS221_ADDRESS		0x5f
#define LPS25H_ADDRESS		0x5c
#define LSM9DS1_ADDRESS_G	0x6a
#define LSM9DS1_ADDRESS_M	0x1c

// Text dictionnary and corresponding pixel maps
#define NBCHARS 92
static const char txtDictFilename[] = "/usr/local/lib/sense_hat_text.txt";
static const char txtPNGFilename[] = "/usr/local/lib/sense_hat_text.png";
static char txtDict[NBCHARS];
static ssize_t txtDictLen;
// PNG pointers and rows
static png_structp png_ptr;
static png_infop png_info_ptr;
static png_bytepp png_rows;

// IMU setup instantiation
static RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
static RTIMU *imu = RTIMU::createIMU(settings);
static RTPressure *pressure = RTPressure::createPressure(settings);

/*
static int i2cRead(int iHandle, uint8_t addr, uint8_t *buf, int iLen) {
	int rc;

	rc = write(iHandle, &addr, 1);
	if (rc == 1)
	{
		rc = read(iHandle, buf, iLen);
	}
	return rc;
}
*/

int i2cWrite(int iHandle, uint8_t addr, uint8_t *buf, int iLen) {
	uint8_t tmp[I2C_BUFFER_SIZE];
	int rc;

	if (iLen > I2C_BUFFER_SIZE-1 || iLen < 1 || buf == NULL)
		return -1; // invalid write

	tmp[0] = addr; // send the register number first 
	memcpy(&tmp[1], buf, iLen); // followed by the data
	rc = write(iHandle, tmp, iLen+1);
	return rc-1;
}

void senseClear() {
// Turn off all LEDs
	memset(LEDStore, 0, sizeof(LEDStore));
	i2cWrite(ledFile, 0, LEDStore, sizeof(LEDStore));
}

void senseSetLowLight(bool low) {
// Lower led light intensity
	if (low)
		lowLight_switch = true;
	else
		lowLight_switch = false;
	lowLight_state = false;
}

rgb_pixel_t _lowLightDimmer(rgb_pixel_t px) {
// Internal
// Reduce light intensity if lowLight_switch is true.
	uint8_t w;

	px.color[_R] = gamma8[px.color[_R]];
	px.color[_G] = gamma8[px.color[_G]];
	px.color[_B] = gamma8[px.color[_B]];
	w = MIN(px.color[_R], MIN(px.color[_G], px.color[_B])) / 3;
	px.color[_R] -= w;
	px.color[_G] -= w;
	px.color[_B] -= w;

	return px;
}

bool senseInit() {
// Initialization
	char filename[FILENAMELENGTH];
	bool retOk = true;
	// Dictionnary file
	int txtFile =-1;
	// PNG parameters
	FILE * pngFile;
	png_uint_32 png_width;
	png_uint_32 png_height;
	int png_bit_depth;
	int png_color_type;
	int png_interlace_method;
	int png_compression_method;
	int png_filter_method;

	// I2C bus
	snprintf(filename, FILENAMELENGTH-1, "/dev/i2c-%d", I2C_ADAPTER);
	// LED matrix
	ledFile = open(filename, O_RDWR);
	if (ledFile < 0) {
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(ledFile, I2C_SLAVE_FORCE, I2C_LED_DEVICE) < 0) {
        printf("Unable to open LED device as slave \n%s\n", strerror(errno));
		retOk = false;
	}

	// Image text dictionnary
	txtFile = open(txtDictFilename, O_RDONLY);
	if (txtFile < 0) {
		printf("Failed to open image text dictionnary.\n%s\n", strerror(errno));
		retOk = false;
	}
	else {
		txtDictLen = read(txtFile, txtDict, NBCHARS);
		close(txtFile);
	}

	// PNG image file
	pngFile = fopen(txtPNGFilename, "rb");
	if (! pngFile) {
		printf("Failed to open PNG image file.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (! (png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
		printf("Cannot create PNG read structure.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if(! (png_info_ptr = png_create_info_struct (png_ptr))) {
		printf("Cannot create PNG info structure.\n%s\n", strerror(errno));
		retOk = false;
	}
	else {
		png_init_io (png_ptr, pngFile);
		png_read_png (png_ptr, png_info_ptr, 0, 0);
		png_get_IHDR (png_ptr, png_info_ptr, &png_width, &png_height, &png_bit_depth,
					&png_color_type, &png_interlace_method, &png_compression_method,
					&png_filter_method);
		png_rows = png_get_rows(png_ptr, png_info_ptr);
		fclose(pngFile);
	}

	// IMU 
    std::cout << "IMU is opening" << std::endl; 
	
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)){
       std::cout << "Error, couldn't open IMU" << std::endl; 
       retOk = false;
    }
    // Initialise the imu object
    imu->IMUInit();

	//  set up pressure sensor
	if (pressure != NULL)
        pressure->pressureInit();

    // Set the Fusion coefficient
    imu->setSlerpPower(0.02);
    // Enable the sensors
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true); 

	senseClear();
	
	return retOk;
}

void senseShutdown() {

	senseClear();
	// Close all I2C file handles
	if (ledFile != -1) {
		close(ledFile);
		ledFile = -1;
	}
}

uint16_t sensePackPixel(rgb_pixel_t rgb) {
// Internal. Encodes [R,G,B] array into 16 bit RGB565
	uint16_t r, g, b;

	r = (uint16_t)(rgb.color[_R] >> 3) & 0x1f;
	g = (uint16_t)(rgb.color[_G] >> 2) & 0x3f;
	b = (uint16_t)(rgb.color[_B] >> 3) & 0x1f;
	return r << 11 | g << 5 | b;
}

rgb_pixel_t senseUnPackPixel(uint16_t rgb565) {
// Internal. Decodes 16 bit RGB565 into [R,G,B] array
	rgb_pixel_t pix;

	pix.color[_R] = (uint8_t) ((rgb565 & 0xf800) >> 11) << 3; // Red
	pix.color[_G] = (uint8_t) ((rgb565 & 0x7e0) >> 5) << 2; // Green
	pix.color[_B] = (uint8_t) ((rgb565 & 0x1f)) << 3; // Blue
	return pix;
}

void senseRGBClear(uint8_t r, uint8_t g, uint8_t b) {
// Turns on all pixels with the same RGB color
	int i, x, y;

	rgb_pixel_t rgb = { .color = {r, g, b}  };
	rgb565_pixel_t rgb565 = sensePackPixel(rgb);

	for(x = 0; x < SENSE_LED_WIDTH; x++)
		for(y = 0; y < SENSE_LED_WIDTH; y++) {
			i = (x*24)+y; // offset into array
			LEDStore[i] = (uint8_t)((rgb565 >> 10) & 0x3e); // Red
			LEDStore[i+8] = (uint8_t)((rgb565 >> 5) & 0x3f); // Green
			LEDStore[i+16] = (uint8_t)((rgb565 << 1) & 0x3e); // Blue
		}
	i2cWrite(ledFile, 0, LEDStore, sizeof(LEDStore));
}

bool senseSetRGB565pixel(unsigned int x, unsigned int y, rgb565_pixel_t rgb565) {
	int i;
	bool retOk = false;

	if (x < SENSE_LED_WIDTH && y < SENSE_LED_WIDTH)
	{
		i = (x*24)+y; // offset into array
		LEDStore[i] = (uint8_t)((rgb565 >> 10) & 0x3e); // Red
		LEDStore[i+8] = (uint8_t)((rgb565 >> 5) & 0x3f); // Green
		LEDStore[i+16] = (uint8_t)((rgb565 << 1) & 0x3e); // Blue
		i2cWrite(ledFile, 0, LEDStore, sizeof(LEDStore)); // send the whole array at once
		retOk = true;
	}
	return retOk;
}

bool senseSetRGBpixel(unsigned int x, unsigned int y, uint8_t red, uint8_t green, uint8_t blue) {
	rgb565_pixel_t rgb565;
	rgb_pixel_t pix;
	bool retOk = false;

	if (x < SENSE_LED_WIDTH && y < SENSE_LED_WIDTH)
	{
		pix.color[_R] = red;
		pix.color[_G] = green;
		pix.color[_B] = blue;
		rgb565 = sensePackPixel(pix);
		
		retOk = senseSetRGB565pixel(x, y, rgb565);

	}
	return retOk;
}

void senseSetRGB565pixels(rgb565_pixels_t pixelArray) {
	int x, y, i;
	rgb565_pixel_t rgb565;
	rgb_pixel_t temp;

	for(x = 0; x < SENSE_LED_WIDTH; x++)
		for(y = 0; y < SENSE_LED_WIDTH; y++) {
			if (lowLight_switch && !lowLight_state) {
				temp = senseUnPackPixel(pixelArray.array[x][y]);
				temp = _lowLightDimmer(temp);
				pixelArray.array[x][y] = sensePackPixel(temp);
				if (x == SENSE_LED_WIDTH)
					lowLight_state = true; // the brightness of all LEDs is reduced
			}
			rgb565 = pixelArray.array[x][y];
			i = (x*24)+y; // offset into array
			LEDStore[i] = (uint8_t)((rgb565 >> 10) & 0x3e); // Red
			LEDStore[i+8] = (uint8_t)((rgb565 >> 5) & 0x3f); // Green
			LEDStore[i+16] = (uint8_t)((rgb565 << 1) & 0x3e); // Blue
		}
	i2cWrite(ledFile, 0, LEDStore, sizeof(LEDStore)); // send the whole array at once
}

void senseSetRGBpixels(rgb_pixels_t pixelArray) {
	int x, y, i;
	uint16_t rgb565;

	for(x = 0; x < SENSE_LED_WIDTH; x++)
		for(y = 0; y < SENSE_LED_WIDTH; y++) {
			if (lowLight_switch && !lowLight_state) {
				pixelArray.array[x][y] = _lowLightDimmer(pixelArray.array[x][y]);
				if (x == SENSE_LED_WIDTH)
					lowLight_state = true; // the brightness of all LEDs is reduced
			}
			rgb565 = sensePackPixel(pixelArray.array[x][y]);
			i = (x*24)+y; // offset into array
			LEDStore[i] = (uint8_t)((rgb565 >> 10) & 0x3e); // Red
			LEDStore[i+8] = (uint8_t)((rgb565 >> 5) & 0x3f); // Green
			LEDStore[i+16] = (uint8_t)((rgb565 << 1) & 0x3e); // Blue
		}
	i2cWrite(ledFile, 0, LEDStore, sizeof(LEDStore)); // send the whole array at once
}

rgb565_pixel_t senseGetRGB565pixel(unsigned int x, unsigned int y) {
	unsigned int i;
	rgb_pixel_t pix = { .color = {0, 0, 0} };
	rgb565_pixel_t rgb565pix;

	if (x < SENSE_LED_WIDTH && y < SENSE_LED_WIDTH) {
		i = (x*24)+y; // offset into array
		pix.color[_R] = LEDStore[i] << 2; // Red
		pix.color[_G] = LEDStore[i+8] << 2; // Green
		pix.color[_B] = LEDStore[i+16] << 2; // Blue
	}
	rgb565pix = sensePackPixel(pix);

	return rgb565pix;
}

rgb_pixel_t senseGetRGBpixel(unsigned int x, unsigned int y) {
	unsigned int i;
	rgb_pixel_t pix = { .color = {0, 0, 0} };

	if (x < SENSE_LED_WIDTH && y < SENSE_LED_WIDTH) {
		i = (x*24)+y; // offset into array
		pix.color[_R] = LEDStore[i] << 2; // Red
		pix.color[_G] = LEDStore[i+8] << 2; // Green
		pix.color[_B] = LEDStore[i+16] << 2; // Blue
	}

	return pix;
}

rgb565_pixels_t senseGetRGB565pixels() {
// Returns an 8x8 array containing RGB565 pixels
	unsigned int x, y;
	rgb565_pixels_t image;
	
	for(y = 0; y < SENSE_LED_WIDTH; y++)
		for(x = 0; x < SENSE_LED_WIDTH; x++)
			image.array[x][y] = senseGetRGB565pixel(x,y);

	return image;
}

rgb_pixels_t senseGetRGBpixels() {
// Returns an 8x8 array containing [R,G,B] pixels
	unsigned int x, y;
	rgb_pixels_t image;
	
    for(y = 0; y < SENSE_LED_WIDTH; y++)
		for(x = 0; x < SENSE_LED_WIDTH; x++)
			image.array[x][y] = senseGetPixel(x,y);

	return image;
}

rgb_pixels_t _rotate90(rgb_pixels_t pixMat) {
// Internal.
// 90 degrees clockwise rotation of LED matrix
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH/2; i++)
		for (j = i; j < SENSE_LED_WIDTH - i - 1; j++) {
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[SENSE_LED_WIDTH - 1 - j][i];
			pixMat.array[SENSE_LED_WIDTH - 1 - j][i] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = pixMat.array[j][SENSE_LED_WIDTH - 1 - i];
			pixMat.array[j][SENSE_LED_WIDTH - 1 - i] = temp;
			}

	return pixMat;
}

rgb_pixels_t _rotate180(rgb_pixels_t pixMat) {
// Internal.
// 180 degrees rotation of LED matrix
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH/2; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++) {
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = temp;
			}

	return pixMat;
}

rgb_pixels_t _rotate270(rgb_pixels_t pixMat) {
// Internal.
// 270 degrees clockwise (90 anti clockwise) rotation of LED matrix
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH/2; i++)
		for (j = i; j < SENSE_LED_WIDTH - i - 1; j++) {
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[j][SENSE_LED_WIDTH - 1 - i];
			pixMat.array[j][SENSE_LED_WIDTH - 1 - i] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = pixMat.array[SENSE_LED_WIDTH - 1 - j][i];
			pixMat.array[SENSE_LED_WIDTH - 1 - j][i] = temp;
			}

	return pixMat;
}

rgb_pixels_t senseRotation(unsigned int angle) {
// Rotate 90, 180, 270 degrees clockwise
	rgb_pixels_t rotated;
	
	switch(angle) {
		case 90:
			rotated = senseGetPixels();
			rotated = _rotate90(rotated);
			break;
		case 180:
			rotated = senseGetPixels();
			rotated = _rotate180(rotated);
			break;
		case 270:
			rotated = senseGetPixels();
			rotated = _rotate270(rotated);
			break;
		default:
			rotated = senseGetPixels();
			break;
	}

	return rotated;
}

rgb_pixels_t senseFlip_h(bool redraw) {
// Flip LED matrix horizontally
	unsigned int i, start, end;
	rgb_pixels_t flipped;
	rgb_pixel_t temp;

	flipped = senseGetPixels();
	for (i = 0; i < SENSE_LED_WIDTH; i++) {
		start = 0;
		end = SENSE_LED_WIDTH-1;
		while (start < end) {
			// swap 2 pixels
			temp = flipped.array[i][start];
			flipped.array[i][start] = flipped.array[i][end];
			flipped.array[i][end] = temp;
			start++;
			end--;
		}
	}

	if (redraw) {
		senseSetPixels(flipped);
	}

	return flipped;
}

rgb_pixels_t senseFlip_v(bool redraw) {
// Flip LED matrix vertically
	int i, start, end;
	rgb_pixels_t flipped;
	rgb_pixel_t temp;

	flipped = senseGetPixels();
	for (i = 0; i < SENSE_LED_WIDTH; i++) {
		start = 0; 
		end = SENSE_LED_WIDTH-1;
		while (start < end) {
			// swap 2 pixels
			temp = flipped.array[start][i];
			flipped.array[start][i] = flipped.array[end][i];
			flipped.array[end][i] = temp;
			start++;
			end--;
		}
	}

	if (redraw)
		senseSetPixels(flipped);

	return flipped;
}

rgb_pixels_t _fillCharPixels(char sign, rgb_pixel_t fgcolor, rgb_pixels_t signPixels) {
// Internal. Collects PNG bytes and fill pixel array with foreground color.
// Character changes every 5 rows of PNG data.
// Each PNG row has 24 bytes.
	char * sign_p;
	int i, j, pos;
	png_bytep png_row;

	// Look for character in the dictionnary
	sign_p = (char *) memchr(txtDict, sign, txtDictLen);
	if (sign_p != NULL) {
		// Position of character in the dictionnary gives its PNG index 
		pos = sign_p - txtDict;
		for	(i = 0; i < 5; i++) {
			// One PNG row is a column of pixel matrix
			png_row = png_rows[pos * 5 + i];
			for (j = 0; j < SENSE_LED_WIDTH; j++)
				// Every 3 PNG byte, all values above 128 should be printed.
				if (png_row[j*3] > 128)
					signPixels.array[i][j] = fgcolor;
        }
		// Rotate 90 degrees anti clockwise 
		signPixels = _rotate270(signPixels);
	}
	else
		printf("'%c' was not found in the list of printable characters.\n", sign);

	return signPixels;
}


void senseShowRGBColoredLetter(char ltr, rgb_pixel_t fg, rgb_pixel_t bg) {
// Print a character with foreground color on background color
	int i, j;
	rgb_pixels_t ltrPix;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
			ltrPix.array[i][j] = bg;

	ltrPix = _fillCharPixels(ltr, fg, ltrPix);

	senseSetPixels(ltrPix);
}

void senseShowRGB565ColoredLetter(char ltr, rgb565_pixel_t fg, rgb565_pixel_t bg) {
// Print a character with foreground color on background color
	int i, j;
	rgb_pixels_t ltrPix;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
			ltrPix.array[i][j] = senseUnPackPixel(bg);

	ltrPix = _fillCharPixels(ltr, senseUnPackPixel(fg), ltrPix);

	senseSetPixels(ltrPix);
}

void senseShowLetter(char ltr) {
// Print a white character on black background
	const rgb_pixel_t white = { .color = {255, 255, 255} };
	const rgb_pixel_t black = { .color = {0, 0, 0} };

	senseShowColoredLetter(ltr, white, black);
}

void _msecSleep(unsigned int msec) {
// Internal.
// Microseconds to milliseconds conversion.
	usleep(1000 * msec);
}

rgb_pixels_t _shiftLeft(rgb_pixels_t shifted, rgb_pixel_t bg) {
// Internal.
// Shift each column left from one position.
	int i, j;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH - 1; j++)
			shifted.array[i][j] = shifted.array[i][j+1];

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		shifted.array[i][SENSE_LED_WIDTH - 1] = bg;

	return shifted;
}

bool _isBackground(rgb_pixel_t pix, rgb_pixel_t bg) {
// Internal.
// Determines if the current pixel is of background color.
	bool bgPix = false;

	if( pix.color[_R] == bg.color[_R] &&
		pix.color[_G] == bg.color[_G] &&
		pix.color[_B] == bg.color[_B] )
		bgPix = true;

	return bgPix;
}

bool _isSpace(rgb_pixels_t key, rgb_pixel_t bg) {
// Internal.
// Determines if the current character is a space for which all pixels are of background color.
	bool spaceKey = true;
	int i, j;

	i = 0;
	while (i < SENSE_LED_WIDTH && spaceKey) {
		j = 0;
		while (j < SENSE_LED_WIDTH && spaceKey) {
			if (! _isBackground(key.array[i][j], bg))
				spaceKey = false;
			j++;
		}
		i++;
	}

	return spaceKey;
}

void senseShowRGBColoredMessage(char * msg, rgb_pixel_t fg, rgb_pixel_t bg) {
// Scrolls a string of characters from left to right
	const unsigned int speed = 100;
	int i, j, msgPos, msgLen;
	int signWidth, width;
	bool emptyCol;

	rgb_pixels_t scroll, sign;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++) {
			scroll.array[i][j] = bg;
			sign.array[i][j] = bg;
		}

	msgLen = strlen(msg);
	for (msgPos = 0; msgPos < msgLen; msgPos++) {
		// New character sign
		sign = _fillCharPixels(msg[msgPos], fg, sign);

		// Trim empty columns from front
		if (_isSpace(sign, bg)) {
			sign = _shiftLeft(sign, bg);
		}
		else {
			emptyCol = true;
			// Shift left from one position for each empty column
			while (emptyCol) {
				i = 0;
				while (_isBackground(sign.array[i][0], bg) &&
						i < SENSE_LED_WIDTH)
					i++;
		
				if (i == SENSE_LED_WIDTH)
					sign = _shiftLeft(sign, bg);
				else
					emptyCol = false;
			}
			// Compute character width with empty rightmost column 
			signWidth = 0;
			for (i = 0; i < SENSE_LED_WIDTH; i++) {
				width = 0;
				for (j = 0; j < 5; j++)
					if (! _isBackground(sign.array[i][j], bg))
						width = j;
				if (width > signWidth)
					signWidth = width;
			}
			signWidth += 2;
		}
		for (j = 0; j < signWidth; j++) {
			for (i = 0; i < SENSE_LED_WIDTH; i++)
				scroll.array[i][SENSE_LED_WIDTH - 1] = sign.array[i][0];
			senseSetPixels(scroll);
			_msecSleep(speed);
			scroll = _shiftLeft(scroll, bg);
			sign = _shiftLeft(sign, bg);
		}
	}
	// Padding to background color all pixels
	while (! _isSpace(scroll, bg)){
		scroll = _shiftLeft(scroll, bg);
		senseSetPixels(scroll);
		_msecSleep(speed);
	}
}

void senseShowRGB565ColoredMessage(char * msg, rgb565_pixel_t fg, rgb565_pixel_t bg) {

	senseShowRGBColoredMessage(msg, senseUnPackPixel(fg), senseUnPackPixel(bg));
}

void senseShowMessage(char * msg) {
	const rgb_pixel_t white = { .color = {255, 255, 255} };
	const rgb_pixel_t black = { .color = {0, 0, 0} };

	senseShowRGBColoredMessage(msg, white, black);
}

// HTS221 Humidity sensor
bool senseGetTempHumid(double *T_DegC, double *H_rH) {
	char filename[FILENAMELENGTH];
	int humFile;
	bool retOk = true;
	uint8_t status;
	int32_t i2c_status;

	// I2C bus
	snprintf(filename, FILENAMELENGTH-1, "/dev/i2c-%d", I2C_ADAPTER);
	humFile = open(filename, O_RDWR);
	if (humFile < 0) {
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(humFile, I2C_SLAVE, HTS221_ADDRESS) < 0) {
		printf("Unable to open humidity device as slave \n%s\n", strerror(errno));
		close(humFile);
		retOk = false;
	}
	// check we are who we should be
	else if ((i2c_status = i2c_smbus_read_byte_data(humFile, HTS221_WHO_AM_I)) != 0xBC) {
		printf("HTS221 I2C who_am_i error: %" PRId32 "\n", i2c_status);
		close(humFile);
		retOk = false;
	}
	else {
		// Power down the device (clean start)
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x00);

		// Turn on the humidity sensor analog front end in single shot mode
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x84);

		// Run one-shot measurement (temperature and humidity).
		// The set bit will be reset by the sensor itself after execution (self-clearing bit)
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG2, 0x01);

		// Wait until the measurement is completed
		do {
			_msecSleep(25);	// 25 milliseconds
			status = i2c_smbus_read_byte_data(humFile, HTS221_CTRL_REG2);
		} while (status != 0);

		// Read calibration temperature LSB (ADC) data
		// (temperature calibration x-data for two points)
		uint8_t t0_out_l = i2c_smbus_read_byte_data(humFile, HTS221_T0_OUT_L);
		uint8_t t0_out_h = i2c_smbus_read_byte_data(humFile, HTS221_T0_OUT_H);
		uint8_t t1_out_l = i2c_smbus_read_byte_data(humFile, HTS221_T1_OUT_L);
		uint8_t t1_out_h = i2c_smbus_read_byte_data(humFile, HTS221_T1_OUT_H);

		// Read calibration temperature (°C) data
		// (temperature calibration y-data for two points)
		uint8_t t0_degC_x8 = i2c_smbus_read_byte_data(humFile, HTS221_T0_DEGC_X8);
		uint8_t t1_degC_x8 = i2c_smbus_read_byte_data(humFile, HTS221_T1_DEGC_X8);
		uint8_t t1_t0_msb = i2c_smbus_read_byte_data(humFile, HTS221_T1T0_MSB);

		// Read calibration relative humidity LSB (ADC) data
		// (humidity calibration x-data for two points)
		uint8_t h0_out_l = i2c_smbus_read_byte_data(humFile, HTS221_H0_T0_OUT_L);
		uint8_t h0_out_h = i2c_smbus_read_byte_data(humFile, HTS221_H0_T0_OUT_H);
		uint8_t h1_out_l = i2c_smbus_read_byte_data(humFile, HTS221_H1_T0_OUT_L);
		uint8_t h1_out_h = i2c_smbus_read_byte_data(humFile, HTS221_H1_T0_OUT_H);

		// Read relative humidity (% rH) data
		// (humidity calibration y-data for two points)
		uint8_t h0_rh_x2 = i2c_smbus_read_byte_data(humFile, HTS221_H0_RH_X2);
		uint8_t h1_rh_x2 = i2c_smbus_read_byte_data(humFile, HTS221_H1_RH_X2);

		// make 16 bit values (bit shift)
		// (temperature calibration x-values)
		int16_t T0_OUT = t0_out_h << 8 | t0_out_l;
		int16_t T1_OUT = t1_out_h << 8 | t1_out_l;

		// make 16 bit values (bit shift)
		// (humidity calibration x-values)
		int16_t H0_T0_OUT = h0_out_h << 8 | h0_out_l;
		int16_t H1_T0_OUT = h1_out_h << 8 | h1_out_l;

		// make 16 and 10 bit values (bit mask and bit shift)
		uint16_t T0_DegC_x8 = (t1_t0_msb & 3) << 8 | t0_degC_x8;
		uint16_t T1_DegC_x8 = ((t1_t0_msb & 12) >> 2) << 8 | t1_degC_x8;

		// Calculate calibration values
		// (temperature calibration y-values)
		double T0_DegC = T0_DegC_x8 / 8.0;
		double T1_DegC = T1_DegC_x8 / 8.0;

		// Humidity calibration values
		// (humidity calibration y-values)
		double H0_rH = h0_rh_x2 / 2.0;
		double H1_rH = h1_rh_x2 / 2.0;

		// Solve the linear equasions 'y = mx + c' to give the
		// calibration straight line graphs for temperature and humidity
		double t_gradient_m = (T1_DegC - T0_DegC) / (T1_OUT - T0_OUT);
		double t_intercept_c = T1_DegC - (t_gradient_m * T1_OUT);

		double h_gradient_m = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
		double h_intercept_c = H1_rH - (h_gradient_m * H1_T0_OUT);

		// Read the ambient temperature measurement (2 bytes to read)
		uint8_t t_out_l = i2c_smbus_read_byte_data(humFile, HTS221_TEMP_OUT_L);
		uint8_t t_out_h = i2c_smbus_read_byte_data(humFile, HTS221_TEMP_OUT_H);

		// make 16 bit value
		int16_t T_OUT = t_out_h << 8 | t_out_l;

		// Read the ambient humidity measurement (2 bytes to read)
		uint8_t h_t_out_l = i2c_smbus_read_byte_data(humFile, HTS221_HUMIDITY_OUT_L);
		uint8_t h_t_out_h = i2c_smbus_read_byte_data(humFile, HTS221_HUMIDITY_OUT_H);

		// make 16 bit value
		int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;

		// Calculate ambient temperature
		*T_DegC = (t_gradient_m * T_OUT) + t_intercept_c;

		// Calculate ambient humidity
		*H_rH = (h_gradient_m * H_T_OUT) + h_intercept_c;

		// Power down the device
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x00);

		close(humFile);
	}
	return retOk;
}

double senseGetHumidity() {
	double Temp, Humid;

	if (!senseGetTempHumid(&Temp, &Humid))
		Humid = 0.0;

	return Humid;
}

double senseGetTemperatureFromHumidity() {
	double Temp, Humid;

	if (!senseGetTempHumid(&Temp, &Humid))
		Temp = 0.0;

	return Temp;
}

// LPS25H Pressure sensor
bool senseGetTempPressure(double *T_DegC, double *P_hPa) {
	char filename[FILENAMELENGTH];
	int preFile;
	bool retOk = true;
	uint8_t status;
	int32_t i2c_status;

	// I2C bus
	snprintf(filename, FILENAMELENGTH-1, "/dev/i2c-%d", I2C_ADAPTER);
	preFile = open(filename, O_RDWR);
	if (preFile < 0) {
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(preFile, I2C_SLAVE, LPS25H_ADDRESS) < 0) {
		printf("Unable to open pressure device as slave \n%s\n", strerror(errno));
		close(preFile);
		retOk = false;
	}
	// check we are who we should be
	else if((i2c_status = i2c_smbus_read_byte_data(preFile, LPS25H_WHO_AM_I)) != 0xBD) {
		printf("LPS25H I2C who_am_i error: %" PRId32 "\n", i2c_status);
		close(preFile);
		retOk = false;
	}
	else {
		// Power down the device (clean start)
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x00);

		// Turn on the pressure sensor analog front end in single shot mode
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x84);

		// Run one-shot measurement (temperature and pressure).
		// The set bit will be reset by the sensor itself after execution (self-clearing bit)
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG2, 0x01);

		// Wait until the measurement is complete
		do {
			_msecSleep(25); // 25 milliseconds
			status = i2c_smbus_read_byte_data(preFile, LPS25H_CTRL_REG2);
		} while(status != 0);

		// Read the temperature measurement (2 bytes to read)
		uint8_t temp_out_l = i2c_smbus_read_byte_data(preFile, LPS25H_TEMP_OUT_L);
		uint8_t temp_out_h = i2c_smbus_read_byte_data(preFile, LPS25H_TEMP_OUT_H);

		// Read the pressure measurement (3 bytes to read)
		uint8_t press_out_xl = i2c_smbus_read_byte_data(preFile, LPS25H_PRESS_OUT_XL);
		uint8_t press_out_l = i2c_smbus_read_byte_data(preFile, LPS25H_PRESS_OUT_L);
		uint8_t press_out_h = i2c_smbus_read_byte_data(preFile, LPS25H_PRESS_OUT_H);

		// make 16 and 24 bit values (using bit shift)
		int16_t temp_out = temp_out_h << 8 | temp_out_l;
		int32_t press_out = press_out_h << 16 | press_out_l << 8 | press_out_xl;

		// calculate output values
		*T_DegC = 42.5 + (temp_out / 480.0);
		*P_hPa = press_out / 4096.0;

		// Power down the device
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x00);

		close(preFile);
	}
	return retOk;
}

double senseGetPressure() {
	double Temp, Pressure;

	if (!senseGetTempPressure(&Temp, &Pressure))
		Pressure = 0.0;

	return Pressure;
}

double senseGetTemperatureFromPressure() {
	double Temp, Pressure;

	if (!senseGetTempPressure(&Temp, &Pressure))
		Temp = 0.0;

	return Temp;
}

// LSM9DS1 IMU
void senseSetIMUConfig(bool compass_enabled, bool gyro_enabled, bool accel_enabled) {

	imu->setCompassEnable(compass_enabled);
	imu->setGyroEnable(gyro_enabled);
	imu->setAccelEnable(accel_enabled);
}

bool senseGetOrientationRadians(double *p, double *r, double *y) {
	bool retOk = true;

	usleep(imu->IMUGetPollInterval() * 1000);

	if (imu->IMURead()) {
		RTIMU_DATA imuData = imu->getIMUData() ;
		if (imuData.fusionPoseValid) {
			RTVector3 curr_pose = imuData.fusionPose ;
			*p = curr_pose.x();
			*r = curr_pose.y();
			*y = -curr_pose.z();
		}
		else retOk = false;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetOrientationDegrees(double *p, double *r, double *y) {
	bool retOk = true;

	if (senseGetOrientationRadians(p, r, y)) {
		*p *= 180.0 / M_PI;  
		*r *= 180.0 / M_PI;
		*y *= 180.0 / M_PI;
	}
	else
		retOk = false;

	return retOk;
}

double senseGetCompass() {
	double p, r, y;

	_msecSleep(250);
	senseGetOrientationDegrees(&p, &r, &y);

	return y + 180;
}

bool senseGetGyroscopeRadians(double *p, double *r, double *y) {
	bool retOk = true;

	senseSetIMUConfig(false, true, false);

	usleep(imu->IMUGetPollInterval() * 1000);

    if (imu->IMURead()) {
        RTIMU_DATA imuData = imu->getIMUData() ;
        if (imuData.gyroValid) {
            *p = imuData.gyro.x();
            *r = imuData.gyro.y();
            *y = imuData.gyro.z();
        }
        else retOk = false;
    }
    else
        retOk = false;

    return retOk;
}

bool senseGetGyroscopeDegrees(double *p, double *r, double *y) {
	bool retOk = true;

	if (senseGetGyroscopeRadians(p, r, y)) {
		*p *= 180.0 / M_PI;  
		*r *= 180.0 / M_PI;
		*y *= 180.0 / M_PI;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetAccelG(double *x, double *y, double *z) {
	bool retOk = true;

	senseSetIMUConfig(false, false, true);

	usleep(imu->IMUGetPollInterval() * 1000);

    if (imu->IMURead()) {
        RTIMU_DATA imuData = imu->getIMUData() ;
        if (imuData.accelValid) {
            *x = imuData.accel.x();
            *y = imuData.accel.y();
            *z = imuData.accel.z();
        }
        else retOk = false;
    }
    else
        retOk = false;

    return retOk;
}

bool senseGetAccelMPSS(double *x, double *y, double *z) {
	bool retOk = true;

	if (senseGetGyroscopeRadians(x, y, z)) {
		*x *= G_2_MPSS;
		*y *= G_2_MPSS;
		*z *= G_2_MPSS;
	}
	else
		retOk = false;

	return retOk;
}
