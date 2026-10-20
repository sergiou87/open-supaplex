src = $(wildcard ../src/*.c) $(wildcard ../src/null/*.c) $(wildcard ../src/sdl2/*.c) $(wildcard ../src/sdl_common/*.c) ../src/lib/ini/ini.c
exclude := ../src/sdl_common/audio.c ../src/sdl2/video.c
src := $(filter-out $(exclude),$(src))
obj = $(src:.c=.o)

LDFLAGS += -lSDL2_mixer `sdl2-config --libs` -lm

CFLAGS += `sdl2-config --cflags` -O2 -DHAVE_SDL2

all: opensupaplex debug-base-tests

opensupaplex: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

debug-base-tests: debug-base-tests.o $(filter-out ../src/supaplex.o,$(obj))
	$(CC) -o $@ $^ $(LDFLAGS)

debug-base-tests.o: debug-base-tests.c ../src/supaplex.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(obj) debug-base-tests.o
