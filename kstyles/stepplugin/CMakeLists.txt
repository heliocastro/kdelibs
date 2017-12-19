set(kstepstyle_SRCS
    kstep.cpp plugin.cpp
    )

set(kstepstyle_HEADERS
    kstep.h
    )

qt2_wrap_cpp(kstepstyle_SRCS
    SOURCES
        ${kstepstyle_HEADERS}
    )

kde2_module(kstepstyle
    SOURCES
        ${kstepstyle_SRCS}
    LIBS
    kde2::kdeui
    )

