set(webstyle_SRCS
    webstyle.cpp plugin.cpp
    )

set(webstyle_HEADERS
    webstyle.h
    )

qt2_wrap_cpp(webstyle_SRCS
    SOURCES
        ${webstyle_HEADERS}
    )

kde2_module(webstyle
    SOURCES
        ${webstyle_SRCS}
    LIBS
        kde2::kdeui
    )

install(FILES
    web.themerc
    DESTINATION ${KDE2_DATADIR}/kstyle/themes
    )

