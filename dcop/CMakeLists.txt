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

set(libdcop_HEADERS
    dcopclient.h dcopobject.h dcopref.h dcopstub.h
    )

qt2_wrap_cpp(libdcop_SRCS
    SOURCES
        dcopclient.h
    )

kde2_library(libDCOP
    SOURCES
        ${libdcop_SRCS}
        $<TARGET_OBJECTS:kICE>
    LIBS
        Qt::Qt2
    VERSION
        1.0.0
    )

install(FILES ${libdcop_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

set(dcopserver_SRCS
    dcopserver.cpp
    dcopsignals.cpp
    )

qt2_wrap_cpp(dcopserver_SRCS
    SOURCES
        dcopserver.h
    )
add_executable(dcopserver ${dcopserver_SRCS})
target_link_libraries(dcopserver kde2::DCOP)
install(TARGETS dcopserver RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(testdcop_SRCS testdcop.cpp)
qt2_wrap_cpp(testdcop_SRCS
    SOURCES
        testdcop.h
    )
add_executable(testdcop
    ${testdcop_SRCS}
    )
target_link_libraries(testdcop kde2::kdecore)

#add_executable(testdcopc
#    testdcopc.c
#    dcopc.c
#    $<TARGET_OBJECTS:kICE>
#    )
