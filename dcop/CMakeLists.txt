add_subdirectory(KDE-ICE)

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

qt2_wrap_cpp(TARGET libdcop
    SOURCES
        dcopclient.h
    )

add_library(libdcop SHARED
    ${libdcop_SRCS}
    $<TARGET_OBJECTS:kICE>
    )
add_dependencies(libdcop moc_libdcop)
set_target_properties(libdcop PROPERTIES
    OUTPUT_NAME DCOP
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
)
target_link_libraries(libdcop
    Qt::Qt2
    )
target_compile_options(libdcop PRIVATE -fPIC)
target_include_directories(libdcop
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
#install(TARGETS  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#--------------------------------------------------------------

set(dcopserver_SRCS
    dcopserver.cpp
    dcopsignals.cpp
    )

qt2_wrap_cpp(TARGET dcopserver
    SOURCES
        dcopserver.h
    )
add_library(dcopserver MODULE ${dcopserver_SRCS})
target_link_libraries(dcopserver PRIVATE libdcop)
set_target_properties(dcopserver PROPERTIES
    LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
)
