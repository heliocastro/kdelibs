add_subdirectory(KDE-ICE)
add_subdirectory(dcopidl)
add_subdirectory(dcopidl2cpp)
add_subdirectory(client)

set(testdcop_SRCS
    testdcop.cpp
    )

set(testdcopic_SRCS
    testdcopc.c
    dcopc.c
    )

#--------------------------------------------------------------
# libdcop Library

set(libdcop_SRCS
    dcopstub.cpp
    dcopclient.cpp
    dcopobject.cpp
    dcopref.cpp
    )

qt2_wrap_cpp(libdcop_SRCS
    SOURCES
        dcopclient.h
    )

kde2_library(libdcop
    SOURCES
        ${libdcop_SRCS}
        $<TARGET_OBJECTS:kICE>
    LIBS
        Qt::Qt2
    )

#install(TARGETS  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#--------------------------------------------------------------

set(dcopserver_SRCS
    dcopserver.cpp
    dcopsignals.cpp
    )

qt2_wrap_cpp(dcopserver_SRCS
    SOURCES
        dcopserver.h
    )
add_library(dcopserver MODULE ${dcopserver_SRCS})
target_link_libraries(dcopserver PRIVATE kde2::dcop)
set_target_properties(dcopserver PROPERTIES
    LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
)
