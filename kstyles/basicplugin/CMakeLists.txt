set(basicstyle_SRCS
    plugin.cpp
    )

kde2_module(basicstyle
    SOURCES
        ${basicstyle_SRCS}
    LIBS
        kde2::kdeui
    )

