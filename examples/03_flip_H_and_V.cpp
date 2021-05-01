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

	const rgb_pixel_t R = { .color = {255, 0, 0} }; // Red
	const rgb_pixel_t W = { .color = {255, 255, 255} }; // White

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

	if(senseInit()) {
		cout << "-------------------------------" << endl
			 << "Sense Hat initialization Ok." << endl;
		senseClear();
		senseSetPixels(question_mark);
		cout << endl << "Waiting for keypress to flip horizontally." << endl;
		getch();
		senseFlip_h(true);
		cout << endl << "Waiting for keypress to flip vertically." << endl;
		getch();
		senseFlip_v(true);
		cout << endl << "Waiting for keypress." << endl;
		getch();
		senseShutdown();
		cout << "-------------------------------" << endl
			 << "Sense Hat shut down." << endl;
	}

	return EXIT_SUCCESS;
}
