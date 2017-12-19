set(libkspell_SRCS
    ksconfig.cpp kspelldlg.cpp kspell.cpp
    )

set(libkspell_HEADERS
    kspell.h ksconfig.h kspelldlg.h
    )

set(libkspell_NOINST_HEADERS
    version.h
    )

qt2_wrap_cpp(libkspell_SRCS
    SOURCES
        ${libkspell_HEADERS}
        ${libkspell_NOINST_HEADERS}
    )

install(FILES ${libkspell_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkspell
    SOURCES
        ${libkspell_SRCS}
    COMPILE_DEFINITIONS
        -DQT_NO_COMPAT
LIBS
    kde2::kdeui
    )

