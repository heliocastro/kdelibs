add_subdirectory(tests)

set(libkjava_SRCS
    kjavaapplet.cpp kjavaappletcontext.cpp kjavaappletserver.cpp kjavaappletwidget.cpp kjavaprocess.cpp
    kjavadownloader.cpp javaembed.cpp
    )

set(libkjava_HEADERS
    kjavaapplet.h kjavaappletcontext.h kjavaappletserver.h kjavaappletwidget.h kjavaprocess.h
    kjavadownloader.h
    )

set(libkjava_NOINST_HEADERS javaembed.h)

qt2_wrap_cpp(libkjava_SRCS
    SOURCES
        ${libkjava_HEADERS}
        ${libkjava_NOINST_HEADERS}
    )

install(FILES ${libkjava_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkjava
    SOURCES
        ${libkjava_SRCS}
LIBS
    kde2::kdecore kde2::kio
    )

install(FILES kjava.jar kjava.policy DESTINATION ${KDE2_DATADIR}/kjava)
