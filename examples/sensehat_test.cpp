#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sensehat.h>

#define MAXCHAR 32

int getch() {
	int c=0;

	struct termios org_opts, new_opts;
	int res=0;

	//----- store current settings -------------
	res=tcgetattr(STDIN_FILENO, &org_opts);
	assert(res==0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c=getchar();
	//------ restore current settings- ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);

	return c;
}

void ledTests() {

	unsigned int i;
	unsigned int row, col, angle;
	bool lwLght = false;
	char choice, letter;
	const char * led_options[8] = {
		"[1]\tToggle low light switch\n",
		"[2]\tSet/Get a single pixel\n",
		"[3]\tSet/Get all pixels\n",
		"[4]\tFlip horizontally/vertically all pixels\n",
		"[5]\tRotate led matrix with a given angle\n",
		"[6]\tShow a single letter with/without colors\n",
		"[7]\tShow a scrolling message with/without colors\n",
		"[q]\tBack to main menu\n"
	};
	
	const char * sayHello = "Hello, World !";
	char msg[MAXCHAR];

	const rgb_pixel_t B = { .color = {0, 0, 255} }; // blue
	const rgb_pixel_t R = { .color = {255, 0, 0} }; // Red
	const rgb_pixel_t Y = { .color = {255, 255, 0} }; // Yellow
	const rgb_pixel_t W = { .color = {255, 255, 255} }; // White

	rgb_pixel_t rgb_test_pixel;
	rgb_pixels_t rgb_test_pixels;
	rgb_pixels_t question_mark = { .array = {
		{ W, W, W, R, R, W, W, W },
		{ W, W, R, W, W, R, W, W },
		{ W, W, W, W, W, R, W, W },
		{ W, W, W, W, R, W, W, W },
		{ W, W, W, R, W, W, W, W },
		{ W, W, W, R, W, W, W, W },
		{ W, W, W, W, W, W, W, W },
		{ W, W, W, R, W, W, W, W } }
	};

	do {
		senseClear();
		puts(".-------------------------------------------.");
		puts("|             L e d   t e s t s             |");
		puts("'-------------------------------------------'");
		for (i = 0; i < sizeof led_options / sizeof led_options[0]; i++)
			printf("%s", led_options[i]);
		printf("Enter your choice: ");
		choice = (char) getchar();
		while ((getchar()) != '\n');
		putchar('\n');

		switch(choice){
			case '1': // Toggle low light switch
				lwLght = ! lwLght;
				senseSetLowLight(lwLght);
				if (lwLght)
					puts("Low light switch is set to ON.");
				else
					puts("Low light switch is set to OFF.");
				break;
			case '2': // Set/Get a single pixel
				puts("Print a question mark with red foreground and white background.");
				senseSetPixels(question_mark);
				do {
					printf("Enter the row and column coordinates of the pixel to read: ");
				} while (scanf("%u %u", &row, &col) != 2);
				while ((getchar()) != '\n');
				rgb_test_pixel = senseGetPixel(row,col);

				printf("[ R, G, B] values of this pixel : [ %3u, %3u, %3u]\n",
						(unsigned int) rgb_test_pixel.color[_R],
						(unsigned int) rgb_test_pixel.color[_G],
						(unsigned int) rgb_test_pixel.color[_B]);

				puts("Clear all pixels and print only the pixel read.");
				senseClear();
				senseSetPixel(row, col,
						rgb_test_pixel.color[_R],
						rgb_test_pixel.color[_G],
						rgb_test_pixel.color[_B]);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case '3': // Set/Get all pixels
				puts("Print a question mark with red foreground and white background.");
				senseSetPixels(question_mark);
				puts("Waiting for keypress to read all pixels.");
				getch();
				rgb_test_pixels = senseGetPixels();
				for (row = 0; row < SENSE_LED_WIDTH; row++) {
					for (col = 0; col < SENSE_LED_WIDTH; col++)
						printf("[ %3u, %3u, %3u] ",
								(unsigned int) rgb_test_pixels.array[row][col].color[_R],
								(unsigned int) rgb_test_pixels.array[row][col].color[_G],
								(unsigned int) rgb_test_pixels.array[row][col].color[_B]);
					putchar('\n');
				}
				senseClear();
				puts("Waiting for keypress to print all the pixels read.");
				getch();
				senseSetPixels(rgb_test_pixels);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case '4': // Flip horizontally/vertically all pixels
				puts("Print a question mark with red foreground and white background.");
				senseSetPixels(question_mark);
				puts("Waiting for keypress to flip horizontally all pixels and redraw.");
				getch();
				senseFlip_h(true);
				puts("\tWaiting for keypress.");
				getch();
				puts("Print a question mark with red foreground and white background.");
				senseSetPixels(question_mark);
				puts("Waiting for keypress to flip vertically all pixels and redraw.");
				getch();
				senseFlip_v(true);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case '5': // Led matrix rotation
				puts("Print a question mark with red foreground and white background.");
				senseSetPixels(question_mark);
				do {
					printf("Enter the rotation angle [0, 90, 180, 270]: ");
				} while (scanf("%u", &angle) != 1);
				while ((getchar()) != '\n');
				rgb_test_pixels = senseRotation(angle);
				senseSetPixels(rgb_test_pixels);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case '6': // Show letter
				do {
					printf("Enter the character to print with white foreground and black background: ");
				} while (scanf("%c", &letter) != 1);
				while ((getchar()) != '\n');
				printf("The chosen character is: %c\n", letter);
				senseShowLetter(letter);
				do {
					printf("Enter the character to print with yellow foreground and blue background: ");
				} while (scanf("%c", &letter) != 1);
				while ((getchar()) != '\n');
				printf("The chosen character is: %c\n", letter);
				senseShowRGBColoredLetter(letter, Y, B);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case '7': // Show scrolling message
				strncpy(msg, sayHello, MAXCHAR-1);
				senseShowMessage(msg);
				puts("\tWaiting for keypress.");
				getch();
				senseShowRGBColoredMessage(msg, Y, B);
				puts("\t---- End of test ----");
				puts("\tWaiting for keypress.");
				getch();
				break;
			case 'q':
			case 'Q':
				puts("\tGoing back to main menu.");
				break;
			default:
			printf("\tWrong choice.\n");
		}
	} while (toupper(choice) != 'Q');
}

void sensorsTests() {

	unsigned int i;
	double Temp, Humid, Pressure;
	char choice;
	const char * sensors_options[3] = {
		"[1]\tGet temperature and humidity from humidity sensor (HTS221).\n",
		"[2]\tGet temperature and pressure from pressure sensor (LPS25H).\n",
		"[q]\tBack to main menu\n"
	};
	
	do {
		puts(".-------------------------------------------.");
		puts("|         S e n s o r s   t e s t s         |");
		puts("'-------------------------------------------'");
		for (i = 0; i < sizeof sensors_options / sizeof sensors_options[0]; i++)
			printf("%s", sensors_options[i]);
		printf("Enter your choice: ");
		choice = (char) getchar();
		while ((getchar()) != '\n');
		putchar('\n');

		switch(choice){
			case '1': // Humidity
				if (senseGetTempHumid(&Temp, &Humid)) {
					printf("Temp (from humid) = %.2f°C\n", Temp);
					printf("Humidity = %.0f%% rH\n", Humid);
				}
				else
					puts("Error. No measures.");
				break;
			case '2': // Pressure
				if (senseGetTempPressure(&Temp, &Pressure)) {
					printf("Temp (from press) = %.2f°C\n", Temp);
					printf("Pressure = %.0f hPa\n", Pressure);
				}
				else
					puts("Error. No measures.");
				break;
			case 'q':
			case 'Q':
				puts("\tGoing back to main menu.");
				break;
			default:
			printf("\tWrong choice.\n");
		}
	} while (toupper(choice) != 'Q');
}

double _degrees(double radians) {
	return radians * 180.0 / M_PI;
}

void imuTests() {

	unsigned int m;
	double x, y, z;
	char choice;
	const char * imu_options[4] = {
		"[1]\tGet orientation in radians.\n",
		"[2]\tGet orientation in degrees.\n",
		"[3]\tGet temperature and pressure from pressure sensor (LPS25H).\n",
		"[q]\tBack to main menu\n"
	};

	do {
		puts(".-------------------------------------------.");
		puts("|   Inertial Measurement Unit  t e s t s    |");
		puts("'-------------------------------------------'");
		for (m = 0; m < sizeof imu_options / sizeof imu_options[0]; m++)
			printf("%s", imu_options[m]);
		printf("Enter your choice: ");
		choice = (char) getchar();
		while ((getchar()) != '\n');
		putchar('\n');

		senseSetIMUConfig(true, true, true);
		switch(choice){
			case '1': // Orientation in radians
				if (senseGetOrientationRadians(&x, &y, &z)) {
					printf("Roll = %g Pitch = %g Yaw = %g \n", x, y, z);
				}
				else
					puts("Error. No measures.");
				break;
			case '2': // Orientation in degrees
				if (senseGetOrientationDegrees(&x, &y, &z)) {
					printf("Roll = %g Pitch = %g Yaw = %g \n", x, y, z);
				}
				else
					puts("Error. No measures.");
				break;
			case '3': // Pressure
				getch();
				break;
			case 'q':
			case 'Q':
				puts("\tGoing back to main menu.");
				break;
			default:
			printf("\tWrong choice.\n");
		}
	} while (toupper(choice) != 'Q');
}

int main() {

	unsigned int i;
	char choice;

	const char * menu_options[5] = {
		"[1]\tLED matrix\n",
		"[2]\tSensors\n",
		"[3]\tMagnetometer/Accelerometer/Gyroscope\n",
		"[4]\tJoystick\n",
		"[q]\tQuit\n"
	};

	if(senseInit()) {
		printf("sense Hat initialization is ok.\n");

		do {
			puts(".===========================================.");
			puts("|        S e n s e H a t   t e s t s        |");
			puts("'==========================================='");
			for (i = 0; i < sizeof menu_options / sizeof menu_options[0]; i++)
				printf("%s", menu_options[i]);
			printf("Enter your choice: ");
			choice = (char) getchar();
			while ((getchar()) != '\n');
			putchar('\n');
		
			switch(choice){
				case '1':
					ledTests();
					break;
				case '2':
					sensorsTests();
					break;
				case '3':
					imuTests();
					break;
				case 'q':
				case 'Q':
					printf("\tSee you next time.\n");
					break;
				default:
					printf("\tWrong choice.\n");
			}
		} while (toupper(choice) != 'Q');

		senseShutdown();
		printf("Sense Hat shut down.\n");
	}

	return EXIT_SUCCESS;
}
