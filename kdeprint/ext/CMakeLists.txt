set(LIBKDEPRINT_EXT_SOURCES
    kmextfactory.cpp
    kmextmanager.cpp
    kextprinterimpl.cpp
    kmextuimanager.cpp
    )

set(PRIVATE_HEADERS
    kmextfactory.h
    kmextmanager.h
    kextprinterimpl.h
    kmextuimanager.h
    )

qt2_wrap_cpp(LIBKDEPRINT_EXT_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_ext
    SOURCES
        ${LIBKDEPRINT_EXT_SOURCES}
    LIBS
        kde2::kdeprint
    )

install(FILES ext.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )
