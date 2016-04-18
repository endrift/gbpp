#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"

int readVBM(int fd, unsigned short* keys) {
	if (read(fd, keys, 2) < 2) {
		return 0;
	}
	return 1;
}

int readBK2(int fd, unsigned short* keys) {
	char line[16] = "";
	if (read(fd, line, 15) < 15) {
		return 0;
	}
	if (line[0] != '|') {
		return 0;
	}
	if (line[12] != '|') {
		return 0;
	}
	if (line[13] != '\r') {
		return 0;
	}
	if (line[14] != '\n') {
		return 0;
	}

	short k = 0;
	if (line[1] != '.') {
		// Up
		k ^= 0x040;
	}
	if (line[2] != '.') {
		// Down
		k ^= 0x080;
	}
	if (line[3] != '.') {
		// Left
		k ^= 0x020;
	}
	if (line[4] != '.') {
		// Right
		k ^= 0x010;
	}
	if (line[5] != '.') {
		// Start
		k ^= 0x008;
	}
	if (line[6] != '.') {
		// Select
		k ^= 0x004;
	}
	if (line[7] != '.') {
		// B
		k ^= 0x002;
	}
	if (line[8] != '.') {
		// A
		k ^= 0x001;
	}
	if (line[9] != '.') {
		// L
		k ^= 0x200;
	}
	if (line[10] != '.') {
		// R
		k ^= 0x100;
	}
	// Ignore 11
	*keys = k;
	return 1;
}

int readLSMV(int fd, unsigned short* keys) {
	char line[12] = "";
	if (read(fd, line, 12) < 12) {
		return 0;
	}
	if (line[0] != 'F') {
		return 0;
	}
	if (line[2] != '|') {
		return 0;
	}
	if (line[11] != '\n') {
		return 0;
	}

	short k = 0;
	if (line[3] != '.') {
		// A
		k ^= 0x001;
	}
	if (line[4] != '.') {
		// B
		k ^= 0x002;
	}
	if (line[5] != '.') {
		// Select
		k ^= 0x004;
	}
	if (line[6] != '.') {
		// Start
		k ^= 0x008;
	}
	if (line[7] != '.') {
		// Right
		k ^= 0x010;
	}
	if (line[8] != '.') {
		// Left
		k ^= 0x020;
	}
	if (line[9] != '.') {
		// Up
		k ^= 0x040;
	}
	if (line[10] != '.') {
		// Down
		k ^= 0x080;
	}

	*keys = k;
	return 1;
}

int main(int argc, char** argv) {
	printf("Opening tty %s\n", argv[1]);
	int f = openTty(argv[1]);

	printf("Opening movie %s\n", argv[2]);
	int m = open(argv[2], O_RDONLY);

	int (*readFrame)(int, unsigned short*) = 0;
	char header[4];
	read(m, &header, 4);
	if (memcmp(header, "VBM\x1A", 4) == 0) {
		lseek(m, 0x3C, SEEK_SET);
		int offset;
		read(m, &offset, 4);
		printf("Detected VBM, seeking to %08X\n", offset);
		lseek(m, offset, SEEK_SET);
		readFrame = readVBM;
	} else if (memcmp(header, "LogK", 4) == 0) {
		printf("Probably BK2\n");
		char crlf = '\0';
		while (crlf != '\r') {
			read(m, &crlf, 1);
		}
		while (crlf != '\n') {
			read(m, &crlf, 1);
		}
		readFrame = readBK2;
	} else {
		printf("Probably LSMV\n");
		lseek(m, 0, SEEK_SET);
		readFrame = readLSMV;
	}

	int running = 1;
	unsigned long long frames = 0;
	struct timeval t, t2;
	gettimeofday(&t, 0);
	unsigned short lastKey = 0;
	while (running) {
		if (!(frames % 60)) {
			printf("%02llu:%02llu:%02llu\n",
				frames / (60 * 60 * 60),
				(frames / (60 * 60)) % 60,
				(frames / 60) % 60);
		}
		unsigned short status;
		if (!readFrame(m, &status)) {
			printf("Movie ended.\n");
			running = 0;
			break;
		}
		if (status != lastKey) {
			printf("%llu - %03X -> %03X \n", frames, lastKey, status);
			lastKey = status;
		}
		status ^= 0x3FF;
		write(f, &status, 2);
		usleep(100);
		while (read(f, &status, 2) < 2) {
			usleep(1);
		}
		++frames;
	}

	close(f);
	close(m);

	return 0;
}
