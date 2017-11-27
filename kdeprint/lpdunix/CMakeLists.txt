set(LIBKDEPRINT_LPDUNIX_SOURCES
    kmlpdunixfactory.cpp
    kmlpdunixmanager.cpp
    klpdunixprinterimpl.cpp
    )

set(PRIVATE_HEADERS
    kmlpdunixfactory.h
    kmlpdunixmanager.h
    klpdunixprinterimpl.h
    )

qt2_wrap_cpp(LIBKDEPRINT_LPDUNIX_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_lpdunix
    SOURCES
        ${LIBKDEPRINT_LPDUNIX_SOURCES}
    LIBS
        kde2::kdeprint
    )

install(FILES lpdunix.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )
