
cmake_minimum_required(VERSION 3.10)
project(wsmbusmeters)

include(GNUInstallDirs)
enable_language(CXX)

option(MAN "Build and install manual" OFF)

# default to release build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# add flags for debug
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG_FLAGS "-O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(DEBUG_LIBS "-lasan -lgcov")
endif()

# always use PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# init flags for release build
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
# add common flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0 -std=c++11 -Wall -Wno-unused-function")

add_subdirectory(src)

set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
configure_file(wmbusmeters.pc.in wmbusmeters.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/wmbusmeters.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

if (MAN)
    add_custom_command(OUTPUT wmbusmeters.1.gz
                    MAIN_DEPENDENCY wmbusmeters.1
                    COMMAND gzip -c ${CMAKE_SOURCE_DIR}/wmbusmeters.1 > ${CMAKE_BINARY_DIR}/wmbusmeters.1.gz)
    add_custom_target(man ALL DEPENDS wmbusmeters.1.gz)
    install(FILES ${CMAKE_BINARY_DIR}/wmbusmeters.1.gz
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()

install(FILES wmbusmeters.conf
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR})
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/wmbusmeters.d)
