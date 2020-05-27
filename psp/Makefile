TARGET = OpenSupaplex
SRC = $(wildcard ../src/*.c) $(wildcard ../src/sdl1/*.c) $(wildcard ../src/sdl_common/*.c) ../src/lib/ini/ini.c
OBJS = $(SRC:.c=.o)

INCDIR = 
CFLAGS = -D__PSP__ -DSDL_MAIN_AVAILABLE -O2 -G0 -g -Wall -DHAVE_SDL
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS = 
LIBS = -lSDL_mixer -lvorbisfile -lvorbis -logg -lSDL -lSDLmain -lGL -lGLU -lmikmod -lpspaudiolib -lpspaudio -lm \
	   -lpspgu -lpspvfpu -lpsprtc -lpspvram -lpsphprm -lpspirkeyb -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = OpenSupaplex

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak