cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(OpenSupaplex)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "OpenSupaplex")
set(VITA_TITLEID  "OSPX00001")
set(VITA_VERSION  "07.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
  ../common
)

add_executable(${PROJECT_NAME}
  ../src/supaplex.c
  ../src/controller.c
)

target_link_libraries(${PROJECT_NAME}
  SDL2
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceAudio_stub
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

  FILE resources/ADLIB.SND ADLIB.SND
  FILE resources/BACK.DAT BACK.DAT
  FILE resources/BEEP.SND BEEP.SND
  FILE resources/BLASTER.SND BLASTER.SND
  FILE resources/CHARS6.DAT CHARS6.DAT
  FILE resources/CHARS8.DAT CHARS8.DAT
  FILE resources/CONTROLS.DAT CONTROLS.DAT
  FILE resources/DEMO0.BIN DEMO0.BIN
  FILE resources/DEMO1.BIN DEMO1.BIN
  FILE resources/DEMO2.BIN DEMO2.BIN
  FILE resources/DEMO3.BIN DEMO3.BIN
  FILE resources/DEMO4.BIN DEMO4.BIN
  FILE resources/DEMO5.BIN DEMO5.BIN
  FILE resources/DEMO6.BIN DEMO6.BIN
  FILE resources/DEMO7.BIN DEMO7.BIN
  FILE resources/DEMO8.BIN DEMO8.BIN
  FILE resources/DEMO9.BIN DEMO9.BIN
  FILE resources/FIXED.DAT FIXED.DAT
  FILE resources/GFX.DAT GFX.DAT
  FILE resources/LEVEL.LST LEVEL.LST
  FILE resources/LEVELS.DAT LEVELS.DAT
  FILE resources/MENU.DAT MENU.DAT
  FILE resources/MOVING.DAT MOVING.DAT
  FILE resources/PALETTES.DAT PALETTES.DAT
  FILE resources/PANEL.DAT PANEL.DAT
  FILE resources/README.TXT README.TXT
  FILE resources/ROLAND.SND ROLAND.SND
  FILE resources/SAMPLE.SND SAMPLE.SND
  FILE resources/SPFIX63.DOC SPFIX63.DOC
  FILE resources/SUPAPLEX.CFG SUPAPLEX.CFG
  FILE resources/TITLE.DAT TITLE.DAT
  FILE resources/TITLE1.DAT TITLE1.DAT
  FILE resources/TITLE2.DAT TITLE2.DAT
  FILE resources/myspsig.txt myspsig.txt
)
