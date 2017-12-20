cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(kdelibs
    VERSION 2.2.2
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)
include(kde2)
include(kde2_configure)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(ENABLE_XFT "Enable support for Anti-Aliased fonts." ON)
option(ENABLE_SESSION_MANAGEMENT "Support X Session Management, links in -lSM -lICE" ON)
option(ENABLE_THREAD_SUPPORT "Compile with Threading Support" ON)
option(ENABLE_OPENGL "Compile OpenGL module" ON)
option(SYSTEM_ZLIB "Use a zlib from the operating system" ON)
option(SYSTEM_LIBPNG "Use libpng from the operating system." ON)
option(SYSTEM_LIBMNG "Use libmng from the operating system." ON)
option(SYSTEM_JPEG "Use jpeglib from the operating system." ON)
option(BUILD_TUTORIAL "Build tutorials." OFF)
option(BUILD_EXAMPLES "Build examples." OFF)
set(KDEDIR /opt/kde2 CACHE STRING "Default path ofr KDEDIR install")

find_package(PkgConfig REQUIRED)

find_package(Qt2 REQUIRED)

if(ENABLE_XFT)
    pkg_search_module(XFT REQUIRED xft)
    pkg_search_module(FONTCONFIG REQUIRED fontconfig)
    find_package(Freetype REQUIRED)
endif()

if(ENABLE_SESSION_MANAGEMENT)
    pkg_search_module(SM REQUIRED sm)
endif()

if(ENABLE_THREAD_SUPPORT)
    find_package(Threads REQUIRED)
endif()

if(ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
endif()

if(SYSTEM_ZLIB)
    find_package(ZLIB REQUIRED)
endif()

if(SYSTEM_LIBPNG)
    find_package(PNG REQUIRED)
endif()

if(SYSTEM_LIBMNG)
    pkg_search_module(MNG libmng REQUIRED)
endif()

if(SYSTEM_JPEG)
    find_package(JPEG REQUIRED)
    pkg_search_module(XMU xmu REQUIRED)
endif()

find_package(TIFF)
if(TIFF_FOUND)
    set(HAVE_LIBTIFF ON)
endif()

find_package(ALSA)

find_package(X11 REQUIRED)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

pkg_search_module(PCRE libpcre REQUIRED)

find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

find_package(BZip2 REQUIRED)

find_package(Cups)

create_kde2_config_header()

add_subdirectory(dcop)
add_subdirectory(kdecore)
add_subdirectory(kio)
add_subdirectory(kdoctools)
add_subdirectory(kdeui)
add_subdirectory(kdesu)
add_subdirectory(kssl)
add_subdirectory(kjs)
add_subdirectory(kimgio)
add_subdirectory(kfile)
add_subdirectory(kparts)
add_subdirectory(kdeprint)
if(ALSA_FOUND)
    add_subdirectory(libkmid)
endif()
add_subdirectory(kconf_update)
add_subdirectory(kinit)
#add_subdirectory(arts)
add_subdirectory(interfaces)
add_subdirectory(kab)
add_subdirectory(khtml)
add_subdirectory(kspell)
add_subdirectory(kstyles)
add_subdirectory(libkscreensaver)
add_subdirectory(licenses)
add_subdirectory(mimetypes)
add_subdirectory(pics)
add_subdirectory(doc)
