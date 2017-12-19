# Make the include headers painless
# Again not the desired or optimal solution, but is the fastest

get_target_property(QT2_INCLUDES Qt::Qt2 INCLUDE_DIRECTORIES)
get_target_property(KDECORE_INCLUDES kde2::kdecore INCLUDE_DIRECTORIES)
get_target_property(KIO_INCLUDES kde2::kio INCLUDE_DIRECTORIES)
get_target_property(KDEUI_INCLUDES kde2::kdeui INCLUDE_DIRECTORIES)
get_target_property(DCOP_INCLUDES kde2::DCOP INCLUDE_DIRECTORIES)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    css
    dom
    html
    java
    misc
    xml
    ${QT2_INCLUDES}
    ${KIO_INCLUDES}
    ${KDEUI_INCLUDES}
    ${KDECORE_INCLUDES}
    ${DCOP_INCLUDES}
    )

add_subdirectory(misc)
add_subdirectory(dom)
add_subdirectory(css)
add_subdirectory(xml)
add_subdirectory(html)
add_subdirectory(rendering)
add_subdirectory(pics)
add_subdirectory(java)
add_subdirectory(ecma)

set(libkhtml_SRCS
    khtmlview.cpp khtml_part.cpp khtml_run.cpp khtml_factory.cpp khtml_settings.cc
    khtml_events.cpp khtml_find.cpp khtml_ext.cpp khtml_pagecache.cpp
    )

set(libkhtml_HEADERS
    khtmlview.h khtml_part.h khtml_events.h khtml_settings.h khtmldefaults.h
    khtml_run.h khtml_factory.h
    )

set(libkhtml_NOINST_HEADERS
    design.h testkhtml.h testrender.h khtml_find.h khtml_ext.h khtml_pagecache.h
    khtmlimage.h
    )

qt2_wrap_cpp(libkhtml_SRCS
    SOURCES
        ${libkhtml_HEADERS}
        ${libkhtml_NOINST_HEADERS}
    )

install(FILES ${libkhtml_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkhtml
    SOURCES
        ${libkhtml_SRCS}
        $<TARGET_OBJECTS:libkhtmlxml>
        $<TARGET_OBJECTS:libkhtmlhtml>
        $<TARGET_OBJECTS:libkhtmlrender>
        $<TARGET_OBJECTS:libkhtmlcss>
        $<TARGET_OBJECTS:libkhtmlmisc>
        $<TARGET_OBJECTS:libkhtmldom>
    LIBS
        kde2::kio
        kde2::kparts
        kde2::kfile
        kde2::kssl
        kde2::kdeprint
        kde2::kjava
    )

set(libkhtmlimage_SRCS
    khtmlimage.cpp
    )

set(libkhtmlimage_HEADERS
    khtmlview.h khtml_part.h khtml_events.h khtml_settings.h khtmldefaults.h
    )

qt2_wrap_cpp(libkhtmlimage_SRCS
    SOURCES
        ${libkhtmlimage_HEADERS}
    )

install(FILES ${libkhtmlimage_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkhtmlimage
    SOURCES
        ${libkhtmlimage_SRCS}
LIBS
    kde2::khtml
    )

install(FILES
    khtml.desktop khtmlimage.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )

