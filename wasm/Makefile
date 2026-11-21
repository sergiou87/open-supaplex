CC=emcc

src = $(wildcard ../src/*.c) $(wildcard ../src/sdl2/*.c) $(wildcard ../src/sdl_common/*.c) ../src/null/virtualKeyboard.c ../src/lib/ini/ini.c
obj = $(src:.c=.o)

# -sJSPI=1 is required: without it SDL_Delay busy-waits on the JS main thread
# and the never-returning main() loop freezes the page (JSPI makes
# emscripten_sleep suspend the wasm stack natively; ASYNCIFY would also work)
# -lidbfs.js backs /persistent (writable files: savegames, configs, hall of
# fame) with IndexedDB so they survive reloads; see src/file.c
# -sSDL2_MIXER_FORMATS=mod compiles libmodplug into the SDL2_mixer port so
# the .xm music files decode; it replaces the default "ogg" (unused, no .ogg
# assets), keeping WAV (always built in) for sound effects
# -sDEFAULT_TO_CXX links the C++ runtime: libmodplug is C++ and all game
# objects are C, so emcc would otherwise leave operator new/delete undefined
LDFLAGS = -sJSPI=1 -lSDL2_mixer -lSDL2 -lidbfs.js -sSDL2_MIXER_FORMATS=mod -sDEFAULT_TO_CXX -sEXPORTED_RUNTIME_METHODS=FS

CFLAGS = -s USE_SDL=2 -s USE_SDL_MIXER=2 -DHAVE_SDL2

opensupaplex.html: $(obj)
	$(CC) -s ALLOW_MEMORY_GROWTH=1 -o $@ $^ $(LDFLAGS) --preload-file ../resources@/

# refresh wasm/deploy/ (self-contained browser deployment; index.html and
# README.md are tracked, the copied artifacts are gitignored)
deploy: opensupaplex.html
	cp opensupaplex.js opensupaplex.wasm opensupaplex.data deploy/

clean:
	rm $(obj)
