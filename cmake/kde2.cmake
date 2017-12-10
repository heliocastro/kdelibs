# Standard KDE2 directories
set(KDE2_SERVICETYPESDIR ${CMAKE_INSTALL_DATADIR}/servicetypes CACHE STRING "KDE2 ServiceTypes dir")
set(KDE2_SERVICESDIR ${CMAKE_INSTALL_DATADIR}/services CACHE STRING "KDE2 Services dir")
set(KDE2_DATADIR ${CMAKE_INSTALL_DATADIR} CACHE STRING "KDE2 Data dir")
set(KDE2_HTMLDIR ${CMAKE_INSTALL_DOCDIR}/HTML CACHE STRING "KDE2 Doc dir")

include("${CMAKE_CURRENT_LIST_DIR}/kde2_library.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_kidl.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_icon.cmake")

# The unfortunate global definitions
add_definitions(-DQT_NO_TRANSLATION -DQT_CLEAN_NAMESPACE -DQT_NO_COMPAT -DQT_NO_ASCII_CAST)


