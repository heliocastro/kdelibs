#=============================================================================
# Copyright 2017       Helio Chissini de Castro <helio@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(kdelibs
    VERSION 2.2.2
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

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
if(NOT TARGET BZip2::BZip2)
	add_library(BZip2::BZip2 UNKNOWN IMPORTED)
	set_target_properties(BZip2::BZip2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${BZIP2_INCLUDE_DIRS}")
        set_property(TARGET BZip2::BZip2 APPEND PROPERTY IMPORTED_LOCATION "${BZIP2_LIBRARY}")
endif()

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
