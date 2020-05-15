TARGET = OpenSupaplex
OBJS =  ../src/supaplex.o \
		../src/audio.o \
		../src/conditionals.o \
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
CFLAGS = -D__PSP__ -DSDL_MAIN_AVAILABLE -O2 -G0 -g -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS = 
LIBS = -lSDL2_mixer -lvorbisfile -lvorbis -logg -lSDL2 -lSDL2main -lGL -lGLU -lpspaudiolib -lpspaudio -lm \
	   -lpspgu -lpspvfpu -lpsprtc -lpspvram -lpsphprm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = OpenSupaplex

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak