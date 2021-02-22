src = $(wildcard ../src/*.c) $(wildcard ../src/sdl1/*.c) $(wildcard ../src/sdl_common/*.c) ../src/null/virtualKeyboard.c ../src/lib/ini/ini.c
obj = $(src:.c=.o)

CC=arm-unknown-riscos-gcc
CFLAGS = -O3 -std=c99 -I$(GCCSDK_INSTALL_ENV)/include -DHAVE_SDL
LDFLAGS = -static -L$(GCCSDK_INSTALL_ENV)/lib -lSDL_mixer -lSDL -lvorbisidec -logg -lmikmod
ZIP = $(GCCSDK_INSTALL_ENV)/bin/zip

all: !OSupaplex/!RunImage,ff8 !OSupaplex/data !OSupaplex/audio

!OSupaplex/!RunImage,ff8: opensupaplex,e1f
	elf2aif $< $@

opensupaplex,e1f: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

!OSupaplex/data: ../resources/*.*
	mkdir -p $@
	cp $^ $@

!OSupaplex/audio: ../resources/audio/*.*
	mkdir -p $@
	cp $^ $@

open-supaplex-riscos.zip: all
	$(RM) $@
	$(ZIP) -,9r $@ !OSupaplex

clean:
	$(RM) $(obj)
	$(RM) opensupaplex,e1f
	$(RM) !OSupaplex/!RunImage,ff8
	rm -rf !OSupaplex/data
	rm -rf !OSupaplex/audio
