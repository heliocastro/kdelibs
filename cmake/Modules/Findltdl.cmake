# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying

#.rst:
# Findltdl
# --------
#
# Find ltdl
#
# Find the native ltdl includes and library This module defines
#
# ::
#
#   LTDL_FOUND, If false, do not try to use ltdl.
#   LTDL_INCLUDE_DIR, where to find ltdl.h, etc.
#   LTDL_LIBRARIES, the libraries needed to use LTDL.
#

find_path(LTDL_INCLUDE_DIR
    NAMES
        ltdl.h
    PATHS_SUFFIXES
        include
	)

find_library(LTDL_LIBRARIES
    NAMES
        ltdl
    PATH_SUFFIXES
        lib
        lib64
	)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(ltdl
    REQUIRED_VARS
        LTDL_LIBRARIES
        LTDL_INCLUDE_DIR
    )

if(NOT TARGET ltdl::ltdl)
    add_library(ltdl::ltdl UNKNOWN IMPORTED)
    set_target_properties(ltdl::ltdl PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LTDL_INCLUDE_DIR}")
    set_property(TARGET ltdl::ltdl APPEND PROPERTY
        IMPORTED_LOCATION "${LTDL_LIBRARIES}")
endif()
