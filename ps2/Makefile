DEBUG              = 0

TARGET = OpenSupaplex-debug.elf
TARGET_RELEASE = OpenSupaplex.elf

ifeq ($(DEBUG), 1)
   OPTIMIZE_LV	:= -O0 -g
	RARCH_DEFINES += -DDEBUG
else
   OPTIMIZE_LV	:= -O3
	LDFLAGS :=  -s
endif

INCDIR = -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include
CFLAGS = $(OPTIMIZE_LV) -ffast-math -fsingle-precision-constant
ASFLAGS = $(CFLAGS)

CFLAGS += -D__PS2__ -Dmain=SDL_main -DHAVE_SDL2

LDFLAGS += -L$(PS2DEV)/gsKit/lib -L$(PS2SDK)/ports/lib -L.
LIBS += -lSDL2main -lSDL2 -lSDL2_mixer -lpatches -lgskit -ldmakit -lps2_drivers -lxmp -lmodplug

SRC = $(wildcard ../src/*.c) $(wildcard ../src/sdl_common/*.c) $(wildcard ../src/sdl2/*.c) ../src/null/virtualKeyboard.c ../src/lib/ini/ini.c
EXCLUDE = ../src/sdl1/video.c
SRC := $(filter-out $(EXCLUDE),$(SRC))
OBJS = $(SRC:.c=.o)

EE_OBJS += $(OBJS)
EE_CFLAGS = $(CFLAGS)
EE_CXXFLAGS = $(CFLAGS)
EE_LDFLAGS = $(LDFLAGS)
EE_LIBS = $(LIBS)
EE_ASFLAGS = $(ASFLAGS)
EE_INCS = $(INCDIR)
EE_BIN = $(TARGET)
EE_GPVAL = $(GPVAL)

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

release: all
	ps2-packer $(EE_BIN) $(TARGET_RELEASE)

#Include preferences
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp