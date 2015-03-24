#include <SDL.h>
#include <pthread.h>

#include "common.h"

volatile short keys;
volatile int running;

void* fdThread(void* input) {
	printf("Opening %s\n", input);
	int f = openTty(input);
	printf("Opened!\n");

	while (running) {
		unsigned short status;
		status = keys;
		printf("Writing %03X\n", status);
		write(f, &status, 2);
		while (read(f, &status, 2) <= 0) {
			usleep(25);
			continue;
		}
		printf("Read %03X\n", status);
	}

	close(f);

	return input;
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		printf("Could not initialize video: %s\n", SDL_GetError());
		return 0;
	}
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);

	SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 64, 0);
	running = 1;
	keys = 0x7FF;


	pthread_t thread;
	pthread_create(&thread, 0, fdThread, argv[1]);

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			int keychange = -1;
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_X:
					keychange = 0;
					break;
				case SDL_SCANCODE_Z:
					keychange = 1;
					break;
				case SDL_SCANCODE_BACKSPACE:
					keychange = 2;
					break;
				case SDL_SCANCODE_RETURN:
					keychange = 3;
					break;
				case SDL_SCANCODE_RIGHT:
					keychange = 4;
					break;
				case SDL_SCANCODE_LEFT:
					keychange = 5;
					break;
				case SDL_SCANCODE_UP:
					keychange = 6;
					break;
				case SDL_SCANCODE_DOWN:
					keychange = 7;
					break;
				case SDL_SCANCODE_S:
					keychange = 8;
					break;
				case SDL_SCANCODE_A:
					keychange = 9;
					break;
				case SDL_SCANCODE_ESCAPE:
					keychange = 10;
				default:
					break;
				}
				break;
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				switch (event.jbutton.button) {
				case 15:
					keychange = 0;
					break;
				case 16:
					keychange = 1;
					break;
				case 7:
					keychange = 2;
					break;
				case 6:
					keychange = 3;
					break;
				case 14:
					keychange = 4;
					break;
				case 13:
					keychange = 5;
					break;
				case 11:
					keychange = 6;
					break;
				case 12:
					keychange = 7;
					break;
				case 19:
					keychange = 8;
					break;
				case 20:
					keychange = 9;
					break;
				case 9:
					keychange = 10;
				}
				break;
			case SDL_QUIT:
				running = 0;
				break;
			}

			if (keychange < 0) {
				continue;
			}

			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_JOYBUTTONDOWN:
				keys &= ~(1 << keychange);
				break;
			case SDL_KEYUP:
			case SDL_JOYBUTTONUP:
				keys |= 1 << keychange;
				break;
			}
		}
		SDL_Delay(5);
	}

	return 0;
}
