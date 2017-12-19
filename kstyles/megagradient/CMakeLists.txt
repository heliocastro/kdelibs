set(mega_SRCS
    mega.cpp plugin.cpp
    )

set(mega_HEADERS
    mega.h
    )

qt2_wrap_cpp(mega_SRCS
    SOURCES
        ${mega_HEADERS}
    )

kde2_module(mega
    SOURCES
        ${mega_SRCS}
    LIBS
        kde2::kdeui
    )

