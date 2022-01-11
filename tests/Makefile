src = $(wildcard ../src/*.c) $(wildcard ../src/null/*.c) $(wildcard ../src/sdl2/*.c) $(wildcard ../src/sdl_common/*.c) ../src/lib/ini/ini.c
exclude := ../src/sdl_common/audio.c ../src/sdl2/video.c
src := $(filter-out $(exclude),$(src))
obj = $(src:.c=.o)

LDFLAGS += -lSDL2_mixer `sdl2-config --libs` -lm

CFLAGS += `sdl2-config --cflags` -O2 -DHAVE_SDL2

opensupaplex: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm $(obj)
