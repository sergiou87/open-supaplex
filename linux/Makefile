src = $(wildcard ../src/*.c) ../src/lib/ini/ini.c
obj = $(src:.c=.o)

LDFLAGS = -lSDL2_mixer -lvorbis -logg `sdl2-config --libs` -lm

CFLAGS = `sdl2-config --cflags`

opensupaplex: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm $(obj)