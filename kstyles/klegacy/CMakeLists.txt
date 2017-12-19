set(klegacystyle_SRCS
    klegacystyle.cpp plugin.cpp
    )

set(klegacystyle_HEADERS
    klegacystyle.h
    )

qt2_wrap_cpp(klegacystyle_SRCS
    SOURCES
        ${klegacystyle_HEADERS}
    )

kde2_module(klegacystyle
    SOURCES
        ${klegacystyle_SRCS}
    LIBS
    kde2::kdecore
    )

