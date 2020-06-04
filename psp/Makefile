TARGET = OpenSupaplex
SRC = $(wildcard ../src/*.c) $(wildcard ../src/sdl1/*.c) $(wildcard ../src/sdl_common/*.c) ../src/null/virtualKeyboard.c ../src/lib/ini/ini.c
EXCLUDE = 
SRC := $(filter-out $(EXCLUDE),$(SRC))
OBJS = $(SRC:.c=.o)

INCDIR = 
CFLAGS = -D__PSP__ -Dmain=SDL_main -O2 -G0 -g -Wall -DHAVE_SDL
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS = 
LIBS = -lSDL_mixer -lvorbisfile -lvorbis -logg -lSDL -lSDLmain -lGL -lGLU -lmikmod -lpspaudiolib -lpspaudio -lm \
	   -lpspgu -lpspvfpu -lpsprtc -lpspvram -lpsphprm -lpspirkeyb -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = OpenSupaplex
PSP_EBOOT_ICON = assets/ICON0.PNG
PSP_EBOOT_PIC1 = assets/PIC1.PNG
PSP_EBOOT_SND0 = assets/SND0.AT3

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak