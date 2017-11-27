set(LIBKDEPRINT_LPD_SOURCES
    kmlpdfactory.cpp
    kmlpdmanager.cpp
    klpdprinterimpl.cpp
    kmlpduimanager.cpp
    lpdtools.cpp
    gschecker.cpp
    )

set(PRIVATE_HEADERS
    kmlpdfactory.h
    kmlpdmanager.h
    klpdprinterimpl.h
    kmlpduimanager.h
    lpdtools.h
    gschecker.h
    )

qt2_wrap_cpp(LIBKDEPRINT_LPDUNIX_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_lpd
    SOURCES
        ${LIBKDEPRINT_LPD_SOURCES}
    LIBS
        kde2::kdeprint_management
        kde2::kdeui
    )

install(FILES lpd.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )

add_executable(make_driver_db_lpd make_driver_db_lpd.c)
