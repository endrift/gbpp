all: vbm-play sdl-console

vbm-play: vbm-play.o common.o
	$(CC) $(LDFLAGS) -o $@ $^

sdl-console: sdl-console.o common.o
	$(CC) $(LDFLAGS) $(shell sdl2-config --libs) -o $@ $^

sdl-console.o: CFLAGS=$(shell sdl2-config --cflags)

clean:
	rm *.o sdl-console vbm-play
