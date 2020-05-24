src = $(wildcard ../src/*.c) ../src/lib/ini/ini.c
obj = $(src:.c=.o)

LDFLAGS = -lSDL2_mixer -lvorbis -lFLAC -logg `sdl2-config --libs` `pkg-config SDL2_mixer --libs` -lm

CFLAGS = `sdl2-config --cflags` `pkg-config SDL2_mixer --cflags`

OpenSupaplex: $(obj) OpenSupaplex.res
	$(CC) -o $@ $^ $(LDFLAGS)

OpenSupaplex.res:
	windres OpenSupaplex.rc -O coff -o OpenSupaplex.res

clean:
	rm $(obj)
	rm OpenSupaplex.res
