set(libktexteditor_SRCS
    ktexteditor.cpp
    )

set(libktexteditor_HEADERS
    ktexteditor.h
    )

install(FILES ${libktexteditor_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

qt2_wrap_cpp(libktexteditor_SRCS
    SOURCES
        ${libktexteditor_HEADERS}
    )

kde2_library(libktexteditor
    SOURCES
        ${libktexteditor_SRCS}
    LIBS
        kde2::kparts
    VERSION
        0.0.0
    )

install(FILES
    ktexteditor.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

