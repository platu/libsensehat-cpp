#include <iostream>

#include <termios.h>
#include <assert.h>

#include <sensehat.h>

#define MAXCHAR 32

using namespace std;

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

int main() {


	rgb565_pixels_t question_mark = { .array= {
		{0,  1,  2,  3,  4,  5,  6,  7},
		{8,  9, 10, 11, 12, 13, 14, 15},
		{16, 17, 18, 19, 20, 21, 22, 23},
		{24, 25, 26, 27, 28, 29, 30, 31},
		{32, 33, 34, 35, 36, 37, 38, 39},
		{40, 41, 42, 43, 44, 45, 46, 47},
		{48, 49, 50, 51, 52, 53, 54, 55},
		{56, 57, 58, 59, 60, 61, 62, 63}
		}
	};

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
		senseSetRGB565pixels(question_mark);
		cout << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
