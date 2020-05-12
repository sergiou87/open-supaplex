TARGET = OpenSupaplex
OBJS =  ../src/supaplex.o \
		../src/audio.o \
		../src/config.o \
		../src/controller.o \
		../src/file.o \
		../src/input.o \
		../src/logging.o \
		../src/menu.o \
		../src/nullVirtualKeyboard.o \
		../src/touchscreen.o \
		../src/utils.o \
		../src/video.o \
		../src/lib/ini/ini.o 

INCDIR = 
CFLAGS = -D__PSP__ -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS = 
LIBS = -lSDL2_mixer -lvorbisfile -lvorbis -logg -lSDL2 -lpspaudiolib -lpspaudio -lGL -lm \
	   -lpspgu -lpspvfpu -lpsprtc -lpspvram -lpsphprm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = OpenSupaplex

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak