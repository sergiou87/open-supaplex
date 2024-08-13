src = $(wildcard ../src/*.c) $(wildcard ../src/sdl2/*.c) $(wildcard ../src/sdl_common/*.c) ../src/lib/ini/ini.c ../src/null/virtualKeyboard.c
obj = $(src:.c=.o)

LDFLAGS += -lSDL2_mixer `sdl2-config --libs` -lm

CFLAGS += `sdl2-config --cflags` -DHAVE_SDL2

opensupaplex: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj)
