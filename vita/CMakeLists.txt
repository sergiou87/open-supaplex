cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME OpenSupaplex)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "OpenSupaplex")
set(VITA_TITLEID  "OSPX00001")
set(VITA_VERSION  "07.11")

option(DEBUGNET "Enable debugnet for logging" ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -DHAVE_SDL2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DHAVE_SDL2")

include_directories(
  ../common
)

set(VITA_ELF_NAME ${SHORT_NAME}.elf)
add_executable(${SHORT_NAME}
  ../src/supaplex.c
  ../src/animations.c
  ../src/buttonBorders.c
  ../src/commandLineParser.c
  ../src/conditionals.c
  ../src/config.c
  ../src/demo.c
  ../src/file.c
  ../src/globals.c
  ../src/graphics.c
  ../src/logging.c
  ../src/input.c
  ../src/menu.c
  ../src/utils.c
  ../src/savegame.c
  ../src/sdl_common/audio.c
  ../src/sdl_common/system.c
  ../src/sdl2/controller.c
  ../src/sdl2/keyboard.c
  ../src/sdl2/video.c
  ../src/sdl2/touchscreen.c
  ../src/psp2/psp2VirtualKeyboard.c
  ../src/lib/ini/ini.c
)

target_compile_definitions(${SHORT_NAME}
        PRIVATE
        VITA_VERSION="${VITA_VERSION}"
        VITA_TITLEID="${VITA_TITLEID}"
        )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug mode")
    add_definitions(-DDEBUG)
    if (DEBUGNET)
        message("Debugnet turned on")
        add_definitions(-DDEBUGNET)
        target_link_libraries(${SHORT_NAME} debugnet sceNetCtl_stub)

        get_filename_component(debugnetipFilePath debugnetip.txt REALPATH)
        if (NOT EXISTS ${debugnetipFilePath})
            message(FATAL_ERROR "You need to put an IP in debugnetip.txt to enable the DEBUGNET option.")
        endif ()

        file(STRINGS ${debugnetipFilePath} DEBUGNETIP)
        add_definitions(-DDEBUGNETIP="${DEBUGNETIP}")

        message("The app will send logs to ${DEBUGNETIP}")
    endif ()
endif (CMAKE_BUILD_TYPE MATCHES Debug)

target_link_libraries(${PROJECT_NAME}
  SDL2_mixer
  mpg123
  vorbisfile
  vorbis
  ogg
  FLAC
  mikmod
  SDL2
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceAudio_stub
  SceAppMgr_stub
  SceAppUtil_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
  SceTouch_stub
  SceHid_stub
  m
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml

  FILE ../resources/audio-hq/base-adlib.ogg audio-hq/base-adlib.ogg
  FILE ../resources/audio-hq/base-beep.ogg audio-hq/base-beep.ogg
  FILE ../resources/audio-hq/base-blaster.ogg audio-hq/base-blaster.ogg
  FILE ../resources/audio-hq/base-sample.ogg audio-hq/base-sample.ogg
  FILE ../resources/audio-hq/bug-adlib.ogg audio-hq/bug-adlib.ogg
  FILE ../resources/audio-hq/bug-beep.ogg audio-hq/bug-beep.ogg
  FILE ../resources/audio-hq/bug-blaster.ogg audio-hq/bug-blaster.ogg
  FILE ../resources/audio-hq/bug-sample.ogg audio-hq/bug-sample.ogg
  FILE ../resources/audio-hq/exit-adlib.ogg audio-hq/exit-adlib.ogg
  FILE ../resources/audio-hq/exit-beep.ogg audio-hq/exit-beep.ogg
  FILE ../resources/audio-hq/exit-blaster.ogg audio-hq/exit-blaster.ogg
  FILE ../resources/audio-hq/exit-sample.ogg audio-hq/exit-sample.ogg
  FILE ../resources/audio-hq/explosion-adlib.ogg audio-hq/explosion-adlib.ogg
  FILE ../resources/audio-hq/explosion-beep.ogg audio-hq/explosion-beep.ogg
  FILE ../resources/audio-hq/explosion-blaster.ogg audio-hq/explosion-blaster.ogg
  FILE ../resources/audio-hq/explosion-sample.ogg audio-hq/explosion-sample.ogg
  FILE ../resources/audio-hq/fall-adlib.ogg audio-hq/fall-adlib.ogg
  FILE ../resources/audio-hq/fall-beep.ogg audio-hq/fall-beep.ogg
  FILE ../resources/audio-hq/fall-blaster.ogg audio-hq/fall-blaster.ogg
  FILE ../resources/audio-hq/fall-sample.ogg audio-hq/fall-sample.ogg
  FILE ../resources/audio-hq/infotron-adlib.ogg audio-hq/infotron-adlib.ogg
  FILE ../resources/audio-hq/infotron-beep.ogg audio-hq/infotron-beep.ogg
  FILE ../resources/audio-hq/infotron-blaster.ogg audio-hq/infotron-blaster.ogg
  FILE ../resources/audio-hq/infotron-sample.ogg audio-hq/infotron-sample.ogg
  FILE ../resources/audio-hq/music-beep.xm audio-hq/music-beep.xm
  FILE ../resources/audio-hq/music-blaster.xm audio-hq/music-blaster.xm
  FILE ../resources/audio-hq/music-roland.xm audio-hq/music-roland.xm
  FILE ../resources/audio-hq/push-adlib.ogg audio-hq/push-adlib.ogg
  FILE ../resources/audio-hq/push-beep.ogg audio-hq/push-beep.ogg
  FILE ../resources/audio-hq/push-blaster.ogg audio-hq/push-blaster.ogg
  FILE ../resources/audio-hq/push-sample.ogg audio-hq/push-sample.ogg
  FILE ../resources/BACK.DAT BACK.DAT
  FILE ../resources/CHARS6.DAT CHARS6.DAT
  FILE ../resources/CHARS8.DAT CHARS8.DAT
  FILE ../resources/CONTROLS.DAT CONTROLS.DAT
  FILE ../resources/DEMO0.BIN DEMO0.BIN
  FILE ../resources/DEMO1.BIN DEMO1.BIN
  FILE ../resources/DEMO2.BIN DEMO2.BIN
  FILE ../resources/DEMO3.BIN DEMO3.BIN
  FILE ../resources/DEMO4.BIN DEMO4.BIN
  FILE ../resources/DEMO5.BIN DEMO5.BIN
  FILE ../resources/DEMO6.BIN DEMO6.BIN
  FILE ../resources/DEMO7.BIN DEMO7.BIN
  FILE ../resources/DEMO8.BIN DEMO8.BIN
  FILE ../resources/DEMO9.BIN DEMO9.BIN
  FILE ../resources/FIXED.DAT FIXED.DAT
  FILE ../resources/GFX.DAT GFX.DAT
  FILE ../resources/LEVEL.L01 LEVEL.L01
  FILE ../resources/LEVEL.L02 LEVEL.L02
  FILE ../resources/LEVEL.L03 LEVEL.L03
  FILE ../resources/LEVEL.L04 LEVEL.L04
  FILE ../resources/LEVEL.L05 LEVEL.L05
  FILE ../resources/LEVEL.L06 LEVEL.L06
  FILE ../resources/LEVEL.L07 LEVEL.L07
  FILE ../resources/LEVEL.L08 LEVEL.L08
  FILE ../resources/LEVEL.L09 LEVEL.L09
  FILE ../resources/LEVEL.L10 LEVEL.L10
  FILE ../resources/LEVEL.L11 LEVEL.L11
  FILE ../resources/LEVEL.L12 LEVEL.L12
  FILE ../resources/LEVEL.L27 LEVEL.L27
  FILE ../resources/LEVEL.L92 LEVEL.L92
  FILE ../resources/LEVEL.L93 LEVEL.L93
  FILE ../resources/LEVEL.L94 LEVEL.L94
  FILE ../resources/LEVEL.L95 LEVEL.L95
  FILE ../resources/LEVEL.L96 LEVEL.L96
  FILE ../resources/LEVEL.L97 LEVEL.L97
  FILE ../resources/LEVEL.L98 LEVEL.L98
  FILE ../resources/LEVEL.LST LEVEL.LST
  FILE ../resources/LEVELS.D01 LEVELS.D01
  FILE ../resources/LEVELS.D02 LEVELS.D02
  FILE ../resources/LEVELS.D03 LEVELS.D03
  FILE ../resources/LEVELS.D04 LEVELS.D04
  FILE ../resources/LEVELS.D05 LEVELS.D05
  FILE ../resources/LEVELS.D06 LEVELS.D06
  FILE ../resources/LEVELS.D07 LEVELS.D07
  FILE ../resources/LEVELS.D08 LEVELS.D08
  FILE ../resources/LEVELS.D09 LEVELS.D09
  FILE ../resources/LEVELS.D10 LEVELS.D10
  FILE ../resources/LEVELS.D11 LEVELS.D11
  FILE ../resources/LEVELS.D12 LEVELS.D12
  FILE ../resources/LEVELS.D27 LEVELS.D27
  FILE ../resources/LEVELS.D92 LEVELS.D92
  FILE ../resources/LEVELS.D93 LEVELS.D93
  FILE ../resources/LEVELS.D94 LEVELS.D94
  FILE ../resources/LEVELS.D95 LEVELS.D95
  FILE ../resources/LEVELS.D96 LEVELS.D96
  FILE ../resources/LEVELS.D97 LEVELS.D97
  FILE ../resources/LEVELS.D98 LEVELS.D98
  FILE ../resources/LEVELS.DAT LEVELS.DAT
  FILE ../resources/MENU.DAT MENU.DAT
  FILE ../resources/MOVING.DAT MOVING.DAT
  FILE ../resources/myspsig.txt myspsig.txt
  FILE ../resources/PALETTES.DAT PALETTES.DAT
  FILE ../resources/PANEL.DAT PANEL.DAT
  FILE ../resources/README.TXT README.TXT
  FILE ../resources/SPFIX63.DOC SPFIX63.DOC
  FILE ../resources/SPLVLS01.TXT SPLVLS01.TXT
  FILE ../resources/SPLVLS02.TXT SPLVLS02.TXT
  FILE ../resources/SPLVLS03.TXT SPLVLS03.TXT
  FILE ../resources/SPLVLS04.TXT SPLVLS04.TXT
  FILE ../resources/SPLVLS05.TXT SPLVLS05.TXT
  FILE ../resources/SPLVLS06.TXT SPLVLS06.TXT
  FILE ../resources/SPLVLS07.TXT SPLVLS07.TXT
  FILE ../resources/SPLVLS08.TXT SPLVLS08.TXT
  FILE ../resources/SPLVLS09.TXT SPLVLS09.TXT
  FILE ../resources/SPLVLS10.TXT SPLVLS10.TXT
  FILE ../resources/SPLVLS11.TXT SPLVLS11.TXT
  FILE ../resources/SPLVLS12.TXT SPLVLS12.TXT
  FILE ../resources/SPLVLS27.TXT SPLVLS27.TXT
  FILE ../resources/SPLVLS92.TXT SPLVLS92.TXT
  FILE ../resources/SPLVLS93.TXT SPLVLS93.TXT
  FILE ../resources/SPLVLS94.TXT SPLVLS94.TXT
  FILE ../resources/SPLVLS95.TXT SPLVLS95.TXT
  FILE ../resources/SPLVLS96.TXT SPLVLS96.TXT
  FILE ../resources/SPLVLS97.TXT SPLVLS97.TXT
  FILE ../resources/SPLVLS98.TXT SPLVLS98.TXT
  FILE ../resources/TITLE.DAT TITLE.DAT
  FILE ../resources/TITLE1.DAT TITLE1.DAT
  FILE ../resources/TITLE2.DAT TITLE2.DAT
)

# Usage: PSVITAIP="192.168.0.224" make launch
add_custom_target(launch
  COMMAND curl -T ${PROJECT_NAME}.self ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/eboot.bin
  COMMAND echo "launch ${VITA_TITLEID}" | nc $(PSVITAIP) 1338
  DEPENDS ${PROJECT_NAME}.self
)

# Usage: PSVITAIP="192.168.0.224" make send
add_custom_target(send
  COMMAND curl -T ${PROJECT_NAME}.self ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/eboot.bin
  DEPENDS ${PROJECT_NAME}.self
)

# Usage: PSVITAIP="192.168.0.224" make vpksend
add_custom_target(vpksend
  COMMAND curl -T ${PROJECT_NAME}.vpk ftp://$(PSVITAIP):1337/ux0:/data/
  DEPENDS ${PROJECT_NAME}.vpk
)
