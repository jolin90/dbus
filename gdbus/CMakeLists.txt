#CFLAGS :=  `pkg-config gio-2.0 --cflags` `pkg-config glib-2.0 --cflags` `pkg-config gobject-2.0 --cflags` `pkg-config gio-unix-2.0 --cflags` 
#LDD    :=  `pkg-config gio-2.0 --libs`   `pkg-config glib-2.0 --libs`   `pkg-config gobject-2.0 --libs` `pkg-config gio-unix-2.0 --cflags` 

PROJECT (MAIN)
SET(OUT_BIN jolin-server)

set(CMAKE_VERBOSE_MAKEFILE true)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src SRC_DIR)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/gen GEN_DIR)

SET(SRC_LIST  ${SRC_DIR} ${GEN_DIR})

ADD_DEFINITIONS(-O2 -Wall)

include_directories(/usr/include/glib-2.0)
include_directories(/usr/include/gio-unix-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(gen)

LINK_DIRECTORIES(/usr/lib)
ADD_EXECUTABLE(${OUT_BIN} ${SRC_LIST})

TARGET_LINK_LIBRARIES(${OUT_BIN} glib-2.0)
TARGET_LINK_LIBRARIES(${OUT_BIN} gio-2.0)
TARGET_LINK_LIBRARIES(${OUT_BIN} gobject-2.0)
TARGET_LINK_LIBRARIES(${OUT_BIN} pthread)
