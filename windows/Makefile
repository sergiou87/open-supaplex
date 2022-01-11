src = $(wildcard ../src/*.c) $(wildcard ../src/sdl2/*.c) $(wildcard ../src/sdl_common/*.c) ../src/null/virtualKeyboard.c ../src/lib/ini/ini.c
obj = $(src:.c=.o)

LDFLAGS = -lSDL2_mixer `sdl2-config --libs` `pkg-config SDL2_mixer --libs` -lm

CFLAGS = `sdl2-config --cflags` `pkg-config SDL2_mixer --cflags` -DHAVE_SDL2

OpenSupaplex: $(obj) OpenSupaplex.res
	$(CC) -o $@ $^ $(LDFLAGS)

OpenSupaplex.res:
	windres OpenSupaplex.rc -O coff -o OpenSupaplex.res

clean:
	rm $(obj)
	rm OpenSupaplex.res
