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
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>

#include "../include/sensehat.h"

// Led file handle
static int ledFile = -1;
static bool lowLight_switch = false;
static bool lowLight_state = false;
static uint16_t *pixelMap;
static size_t screensize = 0;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const uint8_t gamma8[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
	2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
	5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
	115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
	144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
	177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
	215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

#define I2C_ADDONS_BUS 0
#define I2C_SENSE_HAT_BUS 1
// Sense Hat I2C devices addresses
#define HTS221_ADDRESS 0x5f
#define LPS25H_ADDRESS 0x5c
#define LSM9DS1_ADDRESS_G 0x6a
#define LSM9DS1_ADDRESS_M 0x1c

#define FILENAMELENGTH 50

// Text dictionnary and corresponding pixel maps
#define NBCHARS 92
#define TXT_DICT_FILENAME "/usr/local/lib/sense_hat_text.txt"
#define TXT_PNG_FILENAME "/usr/local/lib/sense_hat_text.png"
static char txtDict[NBCHARS];
static size_t txtDictLen;
// PNG pointers and rows
static png_structp png_ptr;
static png_infop png_info_ptr;
static png_bytepp png_rows;

// IMU setup instantiation
static RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
static RTIMU *imu = RTIMU::createIMU(settings);
static RTPressure *pressure = RTPressure::createPressure(settings);

// Joystick event collection
static int jsFile = -1;
static struct input_event _jsEvent;
static struct timeval _jstv;

// GPIO chip
static struct gpiod_chip *gpio_chip;
#define GPIOLIST 7
const uint8_t gpio_pinlist[GPIOLIST] = {5, 6, 16, 17, 22, 26, 27};
static struct gpiod_line *gpio_line[GPIOLIST];

// TCS34725 color detection
static tcs34725IntegrationTime_t tcs34725IntegrationTime;
static tcs34725Gain_t tcs34725Gain;
static int tcs34725File = -1;

// ----------------------
// Initialization
// ----------------------

// Internal. Parse input devices file and extract event file handler number.
// Returns event file handler number as int.
// Returns -1 if Sense Hat joystick is not found.
int _getJsEvDevNumber()
{
	char line[256] = {0};
	char *ev_pos;
	bool match = false;
	int num = -1;

	FILE *fd = fopen("/proc/bus/input/devices", "r");
	if (!fd)
		printf("Failed to open event devices file.\n%s\n", strerror(errno));
	else
	{
		while (fscanf(fd, "%[^\n] ", line) != EOF && !match)
		{
			if (strstr(line, "rpi-sense-joy") != 0)
				// Sense Hat joystick device name found
				while (fscanf(fd, "%[^\n] ", line) != EOF && !match)
					if (strstr(line, "Handlers") != 0)
					{
						// Handlers list found
						match = true;
						ev_pos = strstr(line, "event");
						sscanf(ev_pos, "%*[^0123456789]%d", &num);
						printf("Joystick points to device event%d\n", num);
					}
		}
		if (!match)
			puts("Failed to find Sense Hat joystick device name");
		fclose(fd);
	}
	return num;
}

// Internal. Parse framebuffer devices file and extract RPi-Sense FB number.
// Returns FB file number as int.
// Returns -1 if RPi-Sense FB is not found.
int _getFBnum()
{
	char line[256] = {0};
	bool match = false;
	int num = -1;

	FILE *fd = fopen("/proc/fb", "r");
	if (!fd)
		printf("Failed to open event devices file.\n%s\n", strerror(errno));
	else
	{
		while (fscanf(fd, "%[^\n] ", line) != EOF && !match)
		{
			if (strstr(line, "RPi-Sense FB") != 0)
			{
				// Sense Hat framebuffer device name found
				match = true;
				sscanf(line, "%d", &num);
				printf("Sense Hat led matrix points to device /dev/fb%d\n", num);
			}
		}
		if (!match)
			puts("Failed to find Sense Hat led matrix device name");
		fclose(fd);
	}
	return num;
}

// Turn off all LEDs
void senseClear()
{
	memset(pixelMap, 0, screensize);
	usleep(1000 * 10); // wait for 10ms
}

// Sense Hat Initialization
// . file handles for led framebuffer and joystick
// . character set
// . IMU
// . GPIO chip
bool senseInit()
{

	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo vinfo;
	// Initialisation boolean set to true by default.
	// Set to false if any initialization step goes wrong.
	bool initOk = true;
	// Dictionnary file
	int txtFile = -1;
	// PNG parameters
	FILE *pngFile;
	png_uint_32 png_width;
	png_uint_32 png_height;
	int png_bit_depth;
	int png_color_type;
	int png_interlace_method;
	int png_compression_method;
	int png_filter_method;
	// Joystick input event filename
	char joystickFilename[20] = "/dev/input/event", js_ev_num_str[4];
	char framebufferFilename[20] = "/dev/fb", fb_num_str[4];
	int js_ev_num;
	int fb_num;

	// LED matrix
	if ((fb_num = _getFBnum()) >= 0)
	{
		// Open LED matrix file descriptor
		sprintf(fb_num_str, "%hd", fb_num);
		strcat(framebufferFilename, fb_num_str);
		ledFile = open(framebufferFilename, O_RDWR);
		if (ledFile < 0)
		{
			printf("Failed to open LED frame buffer file handle.\n%s\n",
				   strerror(errno));
			initOk = false;
		}

		// Get framebuffer device identity
		if (initOk && ioctl(ledFile, FBIOGET_FSCREENINFO, &fix_info) < 0)
		{
			printf("Unable to set LED frame buffer operation.\n%s\n",
				   strerror(errno));
			initOk = false;
		}

		// Check the correct device has been found
		if (initOk && strcmp(fix_info.id, "RPi-Sense FB") != 0)
		{
			puts("RPi-Sense FB not found");
			initOk = false;
		}

		// Get screen ie LED matrix information
		if (initOk && ioctl(ledFile, FBIOGET_VSCREENINFO, &vinfo) == -1)
		{
			printf("Unable to get screen information.\n%s\n",
				   strerror(errno));
			initOk = false;
		}

		// Print LED matrix screen size
		if (initOk)
		{
			printf("%dx%d, %dbpp\n", vinfo.xres_virtual, vinfo.yres_virtual, vinfo.bits_per_pixel);

			// Figure out the size of the screen in bytes
			screensize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;

			// Map the led frame buffer device into memory
			pixelMap = (uint16_t *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, ledFile, (off_t)0);
			if (pixelMap == MAP_FAILED)
			{
				printf("Unable to map the LED matrix into memory.\n%s\n",
					   strerror(errno));
				initOk = false;
			}
		}

		if (!initOk)
			close(ledFile);
	}

	// Build LED matrix character set
	if (initOk)
	{
		// Image text dictionnary
		txtFile = open(TXT_DICT_FILENAME, O_RDONLY);
		if (txtFile < 0)
		{
			printf("Failed to open image text dictionnary.\n%s\n", strerror(errno));
			initOk = false;
		}
		else
		{
			txtDictLen = (size_t)read(txtFile, txtDict, NBCHARS);
			close(txtFile);
		}

		// PNG image file
		pngFile = fopen(TXT_PNG_FILENAME, "rb");
		if (!pngFile)
		{
			printf("Failed to open PNG image file.\n%s\n", strerror(errno));
			initOk = false;
		}
		else if (!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		{
			printf("Cannot create PNG read structure.\n%s\n", strerror(errno));
			initOk = false;
		}
		else if (!(png_info_ptr = png_create_info_struct(png_ptr)))
		{
			printf("Cannot create PNG info structure.\n%s\n", strerror(errno));
			initOk = false;
		}
		else
		{
			png_init_io(png_ptr, pngFile);
			png_read_png(png_ptr, png_info_ptr, 0, 0);
			png_get_IHDR(png_ptr, png_info_ptr, &png_width, &png_height, &png_bit_depth,
						 &png_color_type, &png_interlace_method, &png_compression_method,
						 &png_filter_method);
			png_rows = png_get_rows(png_ptr, png_info_ptr);
			fclose(pngFile);
		}
	}

	// IMU setup
	if (initOk)
	{
		std::cout << "IMU is opening" << std::endl;

		if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL))
		{
			std::cout << "Error, couldn't open IMU" << std::endl;
			initOk = false;
		}

		// Initialise the imu object
		if (initOk)
		{
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
		}
	}

	// Joystick file handler
	if (initOk && (js_ev_num = _getJsEvDevNumber()) >= 0)
	{
		sprintf(js_ev_num_str, "%d", js_ev_num);
		strcat(joystickFilename, js_ev_num_str);
		jsFile = open(joystickFilename, O_RDONLY);
		if (jsFile < 0)
		{
			printf("Failed to open joystick file handle.\n%s\n", strerror(errno));
			initOk = false;
		}
	}
	else
		initOk = false;

	// GPIO chip selection
	if (initOk)
	{
		gpio_chip = gpiod_chip_open_by_name("gpiochip0");
		if (!gpio_chip)
		{
			printf("GPIO chip opening failure.\n%s\n", strerror(errno));
			initOk = false;
		}
	}

	if (initOk)
		senseClear();

	return initOk;
}

// Free Sense Hat file handles
void senseShutdown()
{

	senseClear();
	munmap(pixelMap, screensize);
	// Close led I2C file handle
	if (ledFile != -1)
	{
		close(ledFile);
		ledFile = -1;
	}
	// Close joystick file handle
	if (jsFile != -1)
	{
		close(jsFile);
		jsFile = -1;
	}
	// Close GPIO chip communication
	gpiod_chip_close(gpio_chip);

	// Close TCS34725 file handle
	colorDetectShutdown();
}

// ----------------------
// LEDs
// ----------------------

// Internal. Reduce light intensity if lowLight_switch is true.
rgb_pixel_t _lowLightDimmer(rgb_pixel_t px)
{
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

// Lower led light intensity
void senseSetLowLight(bool low)
{
	if (low)
		lowLight_switch = true;
	else
		lowLight_switch = false;
	lowLight_state = false;
}

// Internal. Encodes [R,G,B] array into 16 bit RGB565
uint16_t sensePackPixel(rgb_pixel_t rgb)
{
	uint16_t r, g, b;

	r = (uint16_t)(rgb.color[_R] >> 3) & 0x1f;
	g = (uint16_t)(rgb.color[_G] >> 2) & 0x3f;
	b = (uint16_t)(rgb.color[_B] >> 3) & 0x1f;
	return r << 11 | g << 5 | b;
}

// Internal. Decodes 16 bit RGB565 into [R,G,B] array
rgb_pixel_t senseUnPackPixel(uint16_t rgb565)
{
	rgb_pixel_t pix;

	pix.color[_R] = (uint8_t)((rgb565 & 0xf800) >> 11) << 3; // Red
	pix.color[_G] = (uint8_t)((rgb565 & 0x7e0) >> 5) << 2;	 // Green
	pix.color[_B] = (uint8_t)((rgb565 & 0x1f)) << 3;		 // Blue
	return pix;
}

// Turn on all pixels with the same RGB color
void senseRGBClear(uint8_t r, uint8_t g, uint8_t b)
{
	int x, y, i;
	rgb_pixel_t rgb = {.color = {r, g, b}};
	rgb565_pixel_t rgb565 = sensePackPixel(rgb);

	for (x = 0; x < SENSE_LED_WIDTH; x++)
		for (y = 0; y < SENSE_LED_WIDTH; y++)
		{
			i = (x * 8) + y; // offset into array
			*(pixelMap + i) = rgb565;
		}
}

// Turn on a single pixel with RGB565 color format
bool senseSetRGB565pixel(int x, int y, rgb565_pixel_t rgb565)
{
	int i;
	bool retOk = false;

	if (x >= 0 && x < SENSE_LED_WIDTH && y >= 0 && y < SENSE_LED_WIDTH)
	{
		i = (x * 8) + y; // offset into array
		*(pixelMap + i) = rgb565;
		retOk = true;
	}
	return retOk;
}

// Turn on a single pixel with R, G, and B individual values
bool senseSetRGBpixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
	rgb565_pixel_t rgb565;
	rgb_pixel_t pix = {.color = {red, green, blue}};
	bool retOk = false;

	if (x >= 0 && x < SENSE_LED_WIDTH && y >= 0 && y < SENSE_LED_WIDTH)
	{
		rgb565 = sensePackPixel(pix);
		retOk = senseSetRGB565pixel(x, y, rgb565);
	}
	return retOk;
}

// Turn on all pixels from a RGB565 predefined map
void senseSetRGB565pixels(rgb565_pixels_t pixelArray)
{
	int x, y, i;
	rgb565_pixel_t rgb565;
	rgb_pixel_t temp;

	for (x = 0; x < SENSE_LED_WIDTH; x++)
		for (y = 0; y < SENSE_LED_WIDTH; y++)
		{
			if (lowLight_switch && !lowLight_state)
			{
				temp = senseUnPackPixel(pixelArray.array[x][y]);
				temp = _lowLightDimmer(temp);
				pixelArray.array[x][y] = sensePackPixel(temp);
				if (x == SENSE_LED_WIDTH)
					lowLight_state = true; // the brightness of all LEDs is reduced
			}
			rgb565 = pixelArray.array[x][y];
			i = (x * 8) + y; // offset into array
			*(pixelMap + i) = rgb565;
		}
}

// Turn on all pixels from a predefined map of rgb_pixel_t color array
void senseSetRGBpixels(rgb_pixels_t pixelArray)
{
	int x, y, i;
	uint16_t rgb565;

	for (x = 0; x < SENSE_LED_WIDTH; x++)
		for (y = 0; y < SENSE_LED_WIDTH; y++)
		{
			if (lowLight_switch && !lowLight_state)
			{
				pixelArray.array[x][y] = _lowLightDimmer(pixelArray.array[x][y]);
				if (x == SENSE_LED_WIDTH)
					lowLight_state = true; // the brightness of all LEDs is reduced
			}
			rgb565 = sensePackPixel(pixelArray.array[x][y]);
			i = (x * 8) + y; // offset into array
			*(pixelMap + i) = rgb565;
		}
}

// Read a single pixel color in RGB565 format
rgb565_pixel_t senseGetRGB565pixel(int x, int y)
{
	int i;
	rgb565_pixel_t rgb565pix;

	if (x >= 0 && x < SENSE_LED_WIDTH && y >= 0 && y < SENSE_LED_WIDTH)
	{
		i = (x * 8) + y; // offset into array
		rgb565pix = *(pixelMap + i);
	}

	return rgb565pix;
}

// Read a single pixel color in a R, G, B array
rgb_pixel_t senseGetRGBpixel(int x, int y)
{
	int i;
	rgb_pixel_t pix = {.color = {0, 0, 0}};

	if (x >= 0 && x < SENSE_LED_WIDTH && y >= 0 && y < SENSE_LED_WIDTH)
	{
		i = (x * 8) + y; // offset into array
		pix = senseUnPackPixel(*(pixelMap + i));
	}

	return pix;
}

// Returns an 8x8 array containing RGB565 pixels
rgb565_pixels_t senseGetRGB565pixels()
{
	int x, y;
	rgb565_pixels_t image;

	for (y = 0; y < SENSE_LED_WIDTH; y++)
		for (x = 0; x < SENSE_LED_WIDTH; x++)
			image.array[x][y] = senseGetRGB565pixel(x, y);

	return image;
}

// Returns an 8x8 array containing [R,G,B] pixels
rgb_pixels_t senseGetRGBpixels()
{
	int x, y;
	rgb_pixels_t image;

	for (y = 0; y < SENSE_LED_WIDTH; y++)
		for (x = 0; x < SENSE_LED_WIDTH; x++)
			image.array[x][y] = senseGetPixel(x, y);

	return image;
}

// Internal. 90 degrees clockwise rotation of LED matrix
rgb_pixels_t _rotate90(rgb_pixels_t pixMat)
{
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH / 2; i++)
		for (j = i; j < SENSE_LED_WIDTH - i - 1; j++)
		{
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[SENSE_LED_WIDTH - 1 - j][i];
			pixMat.array[SENSE_LED_WIDTH - 1 - j][i] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = pixMat.array[j][SENSE_LED_WIDTH - 1 - i];
			pixMat.array[j][SENSE_LED_WIDTH - 1 - i] = temp;
		}

	return pixMat;
}

// Internal. 180 degrees rotation of LED matrix
rgb_pixels_t _rotate180(rgb_pixels_t pixMat)
{
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH / 2; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
		{
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = temp;
		}

	return pixMat;
}

// Internal. 270 degrees clockwise (90 anti clockwise) rotation of LED matrix
rgb_pixels_t _rotate270(rgb_pixels_t pixMat)
{
	int i, j;
	rgb_pixel_t temp;

	for (i = 0; i < SENSE_LED_WIDTH / 2; i++)
		for (j = i; j < SENSE_LED_WIDTH - i - 1; j++)
		{
			temp = pixMat.array[i][j];
			pixMat.array[i][j] = pixMat.array[j][SENSE_LED_WIDTH - 1 - i];
			pixMat.array[j][SENSE_LED_WIDTH - 1 - i] = pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j];
			pixMat.array[SENSE_LED_WIDTH - 1 - i][SENSE_LED_WIDTH - 1 - j] = pixMat.array[SENSE_LED_WIDTH - 1 - j][i];
			pixMat.array[SENSE_LED_WIDTH - 1 - j][i] = temp;
		}

	return pixMat;
}

// Rotate 90, 180, 270 degrees clockwise
rgb_pixels_t senseRotation(unsigned int angle)
{
	rgb_pixels_t rotated;

	switch (angle)
	{
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

// Flip LED matrix horizontally
rgb_pixels_t senseFlip_h(bool redraw)
{
	unsigned int i, start, end;
	rgb_pixels_t flipped;
	rgb_pixel_t temp;

	flipped = senseGetPixels();
	for (i = 0; i < SENSE_LED_WIDTH; i++)
	{
		start = 0;
		end = SENSE_LED_WIDTH - 1;
		while (start < end)
		{
			// swap 2 pixels
			temp = flipped.array[i][start];
			flipped.array[i][start] = flipped.array[i][end];
			flipped.array[i][end] = temp;
			start++;
			end--;
		}
	}

	if (redraw)
	{
		senseSetPixels(flipped);
	}

	return flipped;
}

// Flip LED matrix vertically
rgb_pixels_t senseFlip_v(bool redraw)
{
	int i, start, end;
	rgb_pixels_t flipped;
	rgb_pixel_t temp;

	flipped = senseGetPixels();
	for (i = 0; i < SENSE_LED_WIDTH; i++)
	{
		start = 0;
		end = SENSE_LED_WIDTH - 1;
		while (start < end)
		{
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

// Internal. Collects PNG bytes and fill pixel array with foreground color.
// Character changes every 5 rows of PNG data.
// Each PNG row has 24 bytes.
rgb_pixels_t _fillCharPixels(char sign, rgb_pixel_t fgcolor, rgb_pixels_t signPixels)
{
	char *sign_p;
	int i, j, pos;
	png_bytep png_row;

	// Look for character in the dictionnary
	sign_p = (char *)memchr(txtDict, sign, txtDictLen);
	if (sign_p != NULL)
	{
		// Position of character in the dictionnary gives its PNG index
		pos = sign_p - txtDict;
		for (i = 0; i < 5; i++)
		{
			// One PNG row is a column of pixel matrix
			png_row = png_rows[pos * 5 + i];
			for (j = 0; j < SENSE_LED_WIDTH; j++)
				// Every 3 PNG byte, all values above 128 should be printed.
				if (png_row[j * 3] > 128)
					signPixels.array[i][j] = fgcolor;
		}
		// Rotate 90 degrees anti clockwise
		signPixels = _rotate270(signPixels);
	}
	else
		printf("'%c' was not found in the list of printable characters.\n", sign);

	return signPixels;
}

// Print a character with foreground color on background color
// Colors are defined in rgb_pixel_t arrays
void senseShowRGBColoredLetter(char ltr, rgb_pixel_t fg, rgb_pixel_t bg)
{
	int i, j;
	rgb_pixels_t ltrPix;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
			ltrPix.array[i][j] = bg;

	ltrPix = _fillCharPixels(ltr, fg, ltrPix);

	senseSetPixels(ltrPix);
}

// Print a character with foreground color on background color
// Colors are encoded ine RGB565 format
void senseShowRGB565ColoredLetter(char ltr, rgb565_pixel_t fg, rgb565_pixel_t bg)
{
	int i, j;
	rgb_pixels_t ltrPix;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
			ltrPix.array[i][j] = senseUnPackPixel(bg);

	ltrPix = _fillCharPixels(ltr, senseUnPackPixel(fg), ltrPix);

	senseSetPixels(ltrPix);
}

// Print a character, white foreground on black background
void senseShowLetter(char ltr)
{
	const rgb_pixel_t white = {.color = {255, 255, 255}};
	const rgb_pixel_t black = {.color = {0, 0, 0}};

	senseShowColoredLetter(ltr, white, black);
}

// Internal. Microseconds to milliseconds conversion.
void _msecSleep(unsigned int msec)
{
	usleep(1000 * msec);
}

// Internal. Shift each column left from one position.
rgb_pixels_t _shiftLeft(rgb_pixels_t shifted, rgb_pixel_t bg)
{
	int i, j;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH - 1; j++)
			shifted.array[i][j] = shifted.array[i][j + 1];

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		shifted.array[i][SENSE_LED_WIDTH - 1] = bg;

	return shifted;
}

// Internal. Determines if the current pixel is of background color.
bool _isBackground(rgb_pixel_t pix, rgb_pixel_t bg)
{
	bool bgPix = false;

	if (pix.color[_R] == bg.color[_R] &&
		pix.color[_G] == bg.color[_G] &&
		pix.color[_B] == bg.color[_B])
		bgPix = true;

	return bgPix;
}

// Internal. Determines if the current character is a space for which all
// pixels are of background color.
bool _isSpace(rgb_pixels_t key, rgb_pixel_t bg)
{
	bool spaceKey = true;
	int i, j;

	i = 0;
	while (i < SENSE_LED_WIDTH && spaceKey)
	{
		j = 0;
		while (j < SENSE_LED_WIDTH && spaceKey)
		{
			if (!_isBackground(key.array[i][j], bg))
				spaceKey = false;
			j++;
		}
		i++;
	}

	return spaceKey;
}

// Scrolls a string of characters from left to right
// Foreground and background colors are defined in two rgb_pixel_t arrays
void senseShowRGBColoredMessage(std::string msg, rgb_pixel_t fg, rgb_pixel_t bg)
{
	const unsigned int speed = 100;
	unsigned int i, j, msgPos, signWidth, width;
	bool emptyCol;

	rgb_pixels_t scroll, sign;

	for (i = 0; i < SENSE_LED_WIDTH; i++)
		for (j = 0; j < SENSE_LED_WIDTH; j++)
		{
			scroll.array[i][j] = bg;
			sign.array[i][j] = bg;
		}

	for (msgPos = 0; msgPos < msg.length(); msgPos++)
	{
		// New character sign
		sign = _fillCharPixels(msg[msgPos], fg, sign);

		// Trim empty columns from front
		if (_isSpace(sign, bg))
		{
			sign = _shiftLeft(sign, bg);
		}
		else
		{
			emptyCol = true;
			// Shift left from one position for each empty column
			while (emptyCol)
			{
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
			for (i = 0; i < SENSE_LED_WIDTH; i++)
			{
				width = 0;
				for (j = 0; j < 5; j++)
					if (!_isBackground(sign.array[i][j], bg))
						width = j;
				if (width > signWidth)
					signWidth = width;
			}
			signWidth += 2;
		}
		for (j = 0; j < signWidth; j++)
		{
			for (i = 0; i < SENSE_LED_WIDTH; i++)
				scroll.array[i][SENSE_LED_WIDTH - 1] = sign.array[i][0];
			senseSetPixels(scroll);
			_msecSleep(speed);
			scroll = _shiftLeft(scroll, bg);
			sign = _shiftLeft(sign, bg);
		}
	}
	// Padding to background color all pixels
	while (!_isSpace(scroll, bg))
	{
		scroll = _shiftLeft(scroll, bg);
		senseSetPixels(scroll);
		_msecSleep(speed);
	}
}

// Scrolls a string of characters from left to right
// Foreground and background colors are encoded in RGB565 format
void senseShowRGB565ColoredMessage(std::string msg, rgb565_pixel_t fg, rgb565_pixel_t bg)
{

	senseShowRGBColoredMessage(msg, senseUnPackPixel(fg), senseUnPackPixel(bg));
}

// Scrolls a string of characters from left to right
// White foreground on black background
void senseShowMessage(std::string msg)
{
	const rgb_pixel_t white = {.color = {255, 255, 255}};
	const rgb_pixel_t black = {.color = {0, 0, 0}};

	senseShowRGBColoredMessage(msg, white, black);
}

// -----------------------------
// HTS221 Humidity sensor
// -----------------------------

// Read both temperature and relative humidity
bool senseGetTempHumid(double &t_C, double &h_R)
{
	char filename[FILENAMELENGTH];
	int humFile;
	bool retOk = true;
	uint8_t status;
	int32_t i2c_status;

	// I2C bus
	snprintf(filename, FILENAMELENGTH - 1, "/dev/i2c-%d", I2C_SENSE_HAT_BUS);
	humFile = open(filename, O_RDWR);
	if (humFile < 0)
	{
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(humFile, I2C_SLAVE, HTS221_ADDRESS) < 0)
	{
		printf("Unable to open humidity device as slave \n%s\n", strerror(errno));
		close(humFile);
		retOk = false;
	}
	// check we are who we should be
	else if ((i2c_status = i2c_smbus_read_byte_data(humFile, HTS221_WHO_AM_I)) != 0xBC)
	{
		printf("HTS221 I2C who_am_i error: %" PRId32 "\n", i2c_status);
		close(humFile);
		retOk = false;
	}
	else
	{
		// Power down the device (clean start)
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x00);

		// Turn on the humidity sensor analog front end in single shot mode
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x84);

		// Run one-shot measurement (temperature and humidity).
		// The set bit will be reset by the sensor itself after execution (self-clearing bit)
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG2, 0x01);

		// Wait until the measurement is completed
		do
		{
			_msecSleep(25); // 25 milliseconds
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
		t_C = (t_gradient_m * T_OUT) + t_intercept_c;

		// Calculate ambient humidity
		h_R = (h_gradient_m * H_T_OUT) + h_intercept_c;

		// Power down the device
		i2c_smbus_write_byte_data(humFile, HTS221_CTRL_REG1, 0x00);

		close(humFile);
	}
	return retOk;
}

// Return relative humidity only
double senseGetHumidity()
{
	double Temp, Humid;

	if (!senseGetTempHumid(Temp, Humid))
		Humid = 0.0;

	return Humid;
}

// Return temperature only
double senseGetTemperatureFromHumidity()
{
	double Temp, Humid;

	if (!senseGetTempHumid(Temp, Humid))
		Temp = 0.0;

	return Temp;
}

// -----------------------------
// LPS25H Pressure sensor
// -----------------------------

// Read both temperature and pressure
bool senseGetTempPressure(double &t_C, double &p_hPa)
{
	char filename[FILENAMELENGTH];
	int preFile;
	bool retOk = true;
	uint8_t status;
	int32_t i2c_status;

	// I2C bus
	snprintf(filename, FILENAMELENGTH - 1, "/dev/i2c-%d", I2C_SENSE_HAT_BUS);
	preFile = open(filename, O_RDWR);
	if (preFile < 0)
	{
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(preFile, I2C_SLAVE, LPS25H_ADDRESS) < 0)
	{
		printf("Unable to open pressure device as slave \n%s\n", strerror(errno));
		close(preFile);
		retOk = false;
	}
	// check we are who we should be
	else if ((i2c_status = i2c_smbus_read_byte_data(preFile, LPS25H_WHO_AM_I)) != 0xBD)
	{
		printf("LPS25H I2C who_am_i error: %" PRId32 "\n", i2c_status);
		close(preFile);
		retOk = false;
	}
	else
	{
		// Power down the device (clean start)
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x00);

		// Turn on the pressure sensor analog front end in single shot mode
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x84);

		// Run one-shot measurement (temperature and pressure).
		// The set bit will be reset by the sensor itself after execution (self-clearing bit)
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG2, 0x01);

		// Wait until the measurement is complete
		do
		{
			_msecSleep(25); // 25 milliseconds
			status = i2c_smbus_read_byte_data(preFile, LPS25H_CTRL_REG2);
		} while (status != 0);

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
		t_C = 42.5 + (temp_out / 480.0);
		p_hPa = press_out / 4096.0;

		// Power down the device
		i2c_smbus_write_byte_data(preFile, LPS25H_CTRL_REG1, 0x00);

		close(preFile);
	}
	return retOk;
}

// Return pressure only
double senseGetPressure()
{
	double Temp, Pressure;

	if (!senseGetTempPressure(Temp, Pressure))
		Pressure = 0.0;

	return Pressure;
}

// Return temperature only
double senseGetTemperatureFromPressure()
{
	double Temp, Pressure;

	if (!senseGetTempPressure(Temp, Pressure))
		Temp = 0.0;

	return Temp;
}

// -----------------------------
// LSM9DS1 IMU
// -----------------------------

// Select function(s) to enable
void senseSetIMUConfig(bool compass_enabled, bool gyro_enabled, bool accel_enabled)
{

	imu->setCompassEnable(compass_enabled);
	imu->setGyroEnable(gyro_enabled);
	imu->setAccelEnable(accel_enabled);
}

bool senseGetOrientationRadians(double &p, double &r, double &y)
{
	bool retOk = true;

	usleep((__useconds_t)(imu->IMUGetPollInterval() * 1000));

	if (imu->IMURead())
	{
		RTIMU_DATA imuData = imu->getIMUData();
		if (imuData.fusionPoseValid)
		{
			RTVector3 curr_pose = imuData.fusionPose;
			p = curr_pose.x();
			r = curr_pose.y();
			y = -curr_pose.z();
		}
		else
			retOk = false;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetOrientationDegrees(double &p, double &r, double &y)
{
	bool retOk = true;

	if (senseGetOrientationRadians(p, r, y))
	{
		p *= 180.0 / M_PI;
		r *= 180.0 / M_PI;
		y *= 180.0 / M_PI;
	}
	else
		retOk = false;

	return retOk;
}

double senseGetCompass()
{
	double p, r, y;

	_msecSleep(20);
	senseGetOrientationDegrees(p, r, y);

	return y + 180;
}

bool senseGetGyroRadians(double &p, double &r, double &y)
{
	bool retOk = true;

	senseSetIMUConfig(false, true, false);

	usleep((__useconds_t)(imu->IMUGetPollInterval() * 1000));

	if (imu->IMURead())
	{
		RTIMU_DATA imuData = imu->getIMUData();
		if (imuData.gyroValid)
		{
			p = imuData.gyro.x();
			r = imuData.gyro.y();
			y = imuData.gyro.z();
		}
		else
			retOk = false;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetGyroDegrees(double &p, double &r, double &y)
{
	bool retOk = true;

	if (senseGetGyroRadians(p, r, y))
	{
		p *= 180.0 / M_PI;
		r *= 180.0 / M_PI;
		y *= 180.0 / M_PI;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetAccelG(double &x, double &y, double &z)
{
	bool retOk = true;

	senseSetIMUConfig(false, false, true);

	usleep((__useconds_t)(imu->IMUGetPollInterval() * 1000));

	if (imu->IMURead())
	{
		RTIMU_DATA imuData = imu->getIMUData();
		if (imuData.accelValid)
		{
			x = imuData.accel.x();
			y = imuData.accel.y();
			z = imuData.accel.z();
		}
		else
			retOk = false;
	}
	else
		retOk = false;

	return retOk;
}

bool senseGetAccelMPSS(double &x, double &y, double &z)
{
	bool retOk = true;

	if (senseGetAccelG(x, y, z))
	{
		x *= G_2_MPSS;
		y *= G_2_MPSS;
		z *= G_2_MPSS;
	}
	else
		retOk = false;

	return retOk;
}

// ----------------------
// Joystick
// ----------------------

// Wait for any joystick event
stick_t senseWaitForJoystick()
{
	stick_t ev;

	if (read(jsFile, &_jsEvent, sizeof(_jsEvent)) == sizeof(_jsEvent))
	{
		// EV_SYN is the event separator mark, not used
		if (_jsEvent.type != EV_SYN)
		{
			ev.action = _jsEvent.code;
			ev.state = _jsEvent.value;
			ev.timestamp = _jsEvent.time.tv_sec + _jsEvent.time.tv_usec / 1000000.0;
		}
	}

	return ev;
}

// Define the duration for monotoring joystick events
void senseSetJoystickWaitTime(long int sec, long int msec)
{

	_jstv.tv_sec = sec;
	_jstv.tv_usec = msec * 1000;
}

// Get joystick event if any
bool senseGetJoystickEvent(stick_t &ev)
{
	bool jsAction = false;
	int clicked;
	struct timeval timeout = _jstv;
	int _jsfd = jsFile;
	fd_set _jsRead;

	// Initialize read file descriptor
	FD_ZERO(&_jsRead);

	// Add driver file descriptor
	FD_SET(_jsfd, &_jsRead);

	clicked = select(_jsfd + 1, &_jsRead, NULL, NULL, &timeout);

	if (clicked == -1)
	{
		printf("Unable to access to joystick.\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	else if (clicked == 0)
	{
		jsAction = false;
	}
	else if (FD_ISSET(_jsfd, &_jsRead))
	{
		read(jsFile, &_jsEvent, sizeof(_jsEvent));
		// EV_SYN is the event separator mark, not used
		if (_jsEvent.type != EV_SYN)
		{
			ev.action = _jsEvent.code;
			ev.state = _jsEvent.value;
			ev.timestamp = _jsEvent.time.tv_sec + _jsEvent.time.tv_usec / 1000000.0;
			jsAction = true;
		}
	}

	return jsAction;
}

// Wait for joystick KEY_ENTER event
bool senseWaitForJoystickEnter()
{
	bool enter = false;
	stick_t joystick;

	do
	{
		joystick = senseWaitForJoystick();

		if (joystick.action == KEY_ENTER && joystick.state == KEY_PRESSED)
			enter = true;
	} while (!enter);

	return enter;
}

// ----------------------
// GPIO pins
// ----------------------

// Check that the GPIO pin number belongs to the list defined in gpio_pinlist
// array
int _gpioCheckPin(uint8_t pin)
{
	int i, pos = -1;

	for (i = 0; i < GPIOLIST; i++)
		if (pin == gpio_pinlist[i])
			pos = i;

	return pos;
}

// Set GPIO pin configuration:
// . Pin number must belong to gpio_pinlist
// . Pin line must be free
// . Resource mist be available
bool gpioSetConfig(unsigned int pin, gpio_dir_t direction)
{
	bool retOk = true;
	int pos;

	if ((pos = _gpioCheckPin(pin)) < 0)
	{
		printf("Wrong GPIO pin number: %u.\n", pin);
		retOk = false;
	}
	else
	{
		gpio_line[pos] = gpiod_chip_get_line(gpio_chip, pin);
		if (!gpio_line[pos])
		{
			printf("GPIO get line failed for pin number: %u.\n", pin);
			retOk = false;
		}
		else if ((direction == out) &&
				 (gpiod_line_request_output(gpio_line[pos], GPIO_CONSUMER, 0) < 0))
		{
			printf("Request line as output failed for pin number: %u.\n", pin);
			gpiod_line_release(gpio_line[pos]);
			retOk = false;
		}
		else if ((direction == in) &&
				 (gpiod_line_request_input(gpio_line[pos], GPIO_CONSUMER) < 0))
		{
			printf("Request line as output failed for pin number: %u.\n", pin);
			gpiod_line_release(gpio_line[pos]);
			retOk = false;
		}
	}

	return retOk;
}

// Set GPIO output pin on or off
bool gpioSetOutput(unsigned int pin, gpio_state_t val)
{
	bool retOk = true;
	int pos;

	if ((pos = _gpioCheckPin(pin)) < 0)
	{
		printf("Wrong GPIO pin number: %u.\n", pin);
		retOk = false;
	}
	else if (gpiod_line_set_value(gpio_line[pos], val) < 0)
	{
		puts("Set line output failed.");
		gpiod_line_release(gpio_line[pos]);
		retOk = false;
	}

	return retOk;
}

// Get GPIO input from pin
int gpioGetInput(unsigned int pin)
{
	int pos;
	int val;

	if ((pos = _gpioCheckPin(pin)) < 0)
	{
		printf("Wrong GPIO pin number: %u.\n", pin);
		val = -1;
	}
	else if ((val = (gpio_state_t)gpiod_line_get_value(gpio_line[pos])) < 0)
	{
		puts("Get line input failed.");
		gpiod_line_release(gpio_line[pos]);
	}

	return val;
}

// -------------------------------------------------------------
// PWM
// The 2 default PWM channels are available if the line below is present in the
// /boot/config.txt file:
// dtoverlay=pwm-2chan,pin2=13,func2=4
// PWM0 on pin # BCM18
// PWM1 on pin # BCM13
// ---------------------------

bool _chanOk(unsigned int line)
{
	bool retOk = true;

	if (line != 0 && line != 1)
	{
		puts("Allowed PWM channels are 0 or 1");
		retOk = false;
	}
	return retOk;
}

bool pwmInit(unsigned int chan)
{
	FILE *fd;
	bool retOk = true;

	if (_chanOk(chan))
	{
		fd = fopen("/sys/class/pwm/pwmchip0/export", "w");
		if (!fd)
		{
			printf("Failed to open export file.\n%s\n", strerror(errno));
			retOk = false;
		}
		else
		{
			fprintf(fd, "%u", chan);
			fclose(fd);
		}
	}
	return retOk;
}

bool pwmPeriod(unsigned int chan, unsigned int period)
{
	FILE *fd;
	bool retOk = true;
	char buf[(FILENAMELENGTH - 1)];

	if (_chanOk(chan))
	{
		sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%u/period", chan);
		fd = fopen(buf, "w");
		if (!fd)
		{
			printf("Failed to open channel %u period file.\n%s\n", chan,
				   strerror(errno));
			retOk = false;
		}
		else
		{
			// usec to nanosec
			period *= 1000;
			fprintf(fd, "%u", period);
			fclose(fd);
		}
	}
	return retOk;
}

bool pwmDutyCycle(unsigned int chan, unsigned int percent)
{
	FILE *fd;
	bool retOk = true;
	char buf[(FILENAMELENGTH - 1)];

	if (_chanOk(chan))
	{
		sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%u/duty_cycle", chan);
		fd = fopen(buf, "w");
		if (!fd)
		{
			printf("Failed to open channel %u duty_cycle file.\n%s\n", chan,
				   strerror(errno));
			retOk = false;
		}
		else
		{
			// percent to nanosec period
			percent *= 100000;
			fprintf(fd, "%u", percent);
			fclose(fd);
		}
	}
	return retOk;
}

bool pwmChangeState(unsigned int chan, std::string state)
{
	FILE *fd;
	bool retOk = true;
	char buf[(FILENAMELENGTH - 1)];

	if (_chanOk(chan))
	{
		sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%u/enable", chan);
		fd = fopen(buf, "w");
		if (!fd)
		{
			printf("Failed to open channel %u enable file.\n%s\n", chan,
				   strerror(errno));
			retOk = false;
		}
		else
		{
			fprintf(fd, "%s", state.c_str());
			fclose(fd);
		}
	}
	return retOk;
}

bool pwmEnable(unsigned int chan)
{
	char status[2] = "1";

	return pwmChangeState(chan, status);
}

bool pwmDisable(unsigned int chan)
{
	char status[2] = "0";

	return pwmChangeState(chan, status);
}

// -------------------------------------------------------------
// TCS34725 color detection
// This sensor is plugged on i2c-0 bus which is actived in /boot/config.txt
// file:
// dtoverlay=pwm-2chan
// ---------------------------

bool colorDetectInit(tcs34725IntegrationTime_t it, tcs34725Gain_t gain)
{
	char filename[FILENAMELENGTH];
	bool retOk = true;

	// set internal parameters
	tcs34725IntegrationTime = it;
	tcs34725Gain = gain;

	// I2C bus
	snprintf(filename, FILENAMELENGTH - 1, "/dev/i2c-%d", I2C_ADDONS_BUS);
	tcs34725File = open(filename, O_RDWR);
	if (tcs34725File < 0)
	{
		printf("Failed to open I2C bus.\n%s\n", strerror(errno));
		retOk = false;
	}
	else if (ioctl(tcs34725File, I2C_SLAVE, TCS34725_ADDRESS) < 0)
	{
		printf("Unable to open TCS34725 color detection device as slave\n%s\n", strerror(errno));
		close(tcs34725File);
		retOk = false;
	}
	else
	{
		uint8_t id = i2c_smbus_read_byte_data(tcs34725File, TCS34725_ID);
		if ((id != 0x4d) && (id != 0x44) && (id != 0x10) && (id != 0x12))
		{
			printf("TCS34725 module identification failed: %hhx\n", id);
			retOk = false;
		}
		else
		{
			i2c_smbus_write_byte_data(tcs34725File, TCS34725_ENABLE, TCS34725_ENABLE_PON);
			_msecSleep(3);
			i2c_smbus_write_byte_data(tcs34725File, TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
			/*
			 * Set a delay for the integration time.
			 * This is only necessary in the case where enabling and then
			 * immediately trying to read values back. This is because setting
			 * AEN triggers an automatic integration, so if a read RGBC is
			 * performed too quickly, the data is not yet valid and all 0's are
			 * returned
			 */
			switch (tcs34725IntegrationTime)
			{
			case TCS34725_INTEGRATIONTIME_2_4MS:
				_msecSleep(3);
				break;
			case TCS34725_INTEGRATIONTIME_24MS:
				_msecSleep(24);
				break;
			case TCS34725_INTEGRATIONTIME_50MS:
				_msecSleep(50);
				break;
			case TCS34725_INTEGRATIONTIME_101MS:
				_msecSleep(101);
				break;
			case TCS34725_INTEGRATIONTIME_154MS:
				_msecSleep(154);
				break;
			case TCS34725_INTEGRATIONTIME_700MS:
				_msecSleep(700);
				break;
			}
		}
	}
	return retOk;
}

void colorDetectShutdown()
{
	/* Turn the device off to save power */
	if (tcs34725File != -1)
	{
		uint8_t reg = i2c_smbus_read_byte_data(tcs34725File, TCS34725_ENABLE);
		i2c_smbus_write_byte_data(tcs34725File, TCS34725_ENABLE, (__u8)(reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN)));
		close(tcs34725File);
		tcs34725File = -1;
	}
}
