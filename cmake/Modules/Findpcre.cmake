# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying

#.rst:
# Findpcre
# --------
#
# Find pcre
#
# Find the native pcre includes and library This module defines
#
# ::
#
#   PCRE_FOUND, If false, do not try to use pcre.
#   PCRE_INCLUDE_DIR, where to find pcre.h, etc.
#   PCRE_LIBRARIES, the libraries needed to use PCRE.
#

find_path(PCRE_INCLUDE_DIR
    NAMES
        pcre.h
    PATHS_SUFFIXES
        include
	)

find_library(PCRE_LIBRARIES
    NAMES
        pcre
    PATH_SUFFIXES
        lib
        lib64
	)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(pcre
    REQUIRED_VARS
        PCRE_LIBRARIES
        PCRE_INCLUDE_DIR
    )

if(NOT TARGET pcre::pcre)
    add_library(pcre::pcre UNKNOWN IMPORTED)
    set_target_properties(pcre::pcre PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PCRE_INCLUDE_DIR}")
    set_property(TARGET pcre::pcre APPEND PROPERTY
        IMPORTED_LOCATION "${PCRE_LIBRARIES}")
endif()
