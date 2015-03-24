#include "common.h"

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

int openTty(const char* input) {
	int f = open(input, O_RDWR | O_NONBLOCK);
	if (f < 0) {
		return f;
	}

	struct termios tio;
	cfmakeraw(&tio);
	cfsetispeed(&tio, B9600);
	cfsetospeed(&tio, B9600);
	tio.c_cflag &= ~PARENB;
	tio.c_cflag &= ~CSTOPB;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CS8;
	tio.c_cflag &= ~CRTSCTS;
	tio.c_cflag |= CREAD | CLOCAL;
	tio.c_iflag &= ~(IXON | IXOFF | IXANY);
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tio.c_oflag &= ~OPOST;
	tio.c_cc[VMIN]  = 0;
	tio.c_cc[VTIME] = 0;
	tcsetattr(f, TCSANOW, &tio);
	return f;
}
