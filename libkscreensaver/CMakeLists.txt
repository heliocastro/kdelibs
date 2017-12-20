set(libkscreensaver_SRCS
    main.cpp kscreensaver.cpp
    )

set(libkscreensaver_HEADERS
    kscreensaver.h kscreensaver_vroot.h
    )

qt2_wrap_cpp(libkscreensaver_SRCS
    SOURCES
        ${libkscreensaver_HEADERS}
    )

install(FILES ${libkscreensaver_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkscreensaver
    SOURCES
        ${libkscreensaver_SRCS}
    LIBS
        kde2::ksycoca
    ALLOW_UNDEFINED
    )

