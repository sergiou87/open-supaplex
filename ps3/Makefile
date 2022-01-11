#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

include $(PSL1GHT)/ppu_rules

TITLE			:=	OpenSupaplex
APPID			:=	OSPX00001
SFOXML			:=	../assets/sfo.xml
ICON0			:=	../assets/ICON0.PNG
SND0			:=	../assets/SND0.AT3
PIC1			:=	../assets/PIC1.PNG
CONTENTID		:=	UP0001-$(APPID)_00-0000000000000000

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET			:=	OpenSupaplex
BUILD			:=	build
SOURCES			:=	../src ../src/sdl_common ../src/sdl2 ../src/ps3 ../src/lib/ini
OTHER_SOURCES	:=	
DATA			:=	data
INCLUDES		:=	include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		=	-O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE) $(LIBPSL1GHT_INC) -I$(PORTLIBS)/include -DHAVE_SDL2 -D__PSL1GHT__ -DPS3APPID=\"$(APPID)\"

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lSDL2_mixer -lmikmod -lSDL2 -lrsx -lgcm_sys -lio -laudio -lsysutil -lrt -llv2 -lm

ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG=1 -DDEBUGNET
	LIBS += -ldebugnet
endif

ifneq ($(strip $(DEBUGNETIP)),)
	CFLAGS += -DDEBUGNETIP=\"$(DEBUGNETIP)\"
endif

CXXFLAGS	=	$(CFLAGS)

LDFLAGS		=	$(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

CFILES      := $(CFILES) $(OTHER_SOURCES)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean bin sendpkg

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

# Usage: PS3IP="192.168.0.6" make sendpkg
# You need to build the pkg first
sendpkg:
	curl -T $(OUTPUT).pkg ftp://$(PS3IP)/dev_hdd0/packages/$(TARGET).pkg

# Usage: PS3IP="192.168.0.6" make send
# You need to build the pkg first to get the EBOOT.BIN
send:
	curl -T $(BUILDDIR)/pkg/USRDIR/EBOOT.BIN ftp://$(PS3IP)/dev_hdd0/game/$(APPID)/USRDIR/EBOOT.BIN

#---------------------------------------------------------------------------------
else

DEPENDS	:= $(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).pkg: preparepkg $(OUTPUT).self
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

preparepkg:
ifeq ("$(wildcard ./.pkg_prepared)", "")
	@echo "Preparing pkg resources"
	@mkdir -p pkg/USRDIR
	@cp -R ../../resources/* pkg/USRDIR/
	@cp $(SND0) pkg/SND0.AT3
	@cp $(PIC1) pkg/PIC1.PNG
	@touch .pkg_prepared
	@echo "pkg resources prepared!"
else
	@echo "pkg resources already prepared"
endif



#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
