set(hcstyle_SRCS
    hcstyle.cpp plugin.cpp
    )

set(hcstyle_NOINST_HEADERS
    hcstyle.h
    )

qt2_wrap_cpp(hcstyle_SRCS
    SOURCES
        ${hcstyle_NOINST_HEADERS}
    )

install(FILES ${hcstyle_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(hcstyle
    SOURCES
        ${hcstyle_SRCS}
LIBS
    kde2::kdeui
    )

