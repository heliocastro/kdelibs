set(libkdeprint_cups_SOURCES
    cupsinfos.cpp
    ipprequest.cpp
    kmcupsfactory.cpp
    kmcupsjobmanager.cpp
    kmcupsmanager.cpp
    kmcupsuimanager.cpp
    kmpropbanners.cpp
    kmwbanners.cpp
    kmwipp.cpp
    kmwippselect.cpp
    kcupsprinterimpl.cpp
    kmcupsconfig.cpp
    kmcupsconfigwidget.cpp
    kmconfigcups.cpp
    kmconfigcupsdir.cpp
    kmwippprinter.cpp
    kmwfax.cpp
    )

set(PRIVATE_HEADERS
    cupsinfos.h
    ipprequest.h
    kmcupsfactory.h
    kmcupsjobmanager.h
    kmcupsmanager.h
    kmcupsuimanager.h
    kmpropbanners.h
    kmwbanners.h
    kmwipp.h
    kmwippselect.h
    kcupsprinterimpl.h
    kmcupsconfig.h
    kmcupsconfigwidget.h
    kmconfigcups.h
    kmconfigcupsdir.h
    kmwippprinter.h
    kmwfax.h
    )

qt2_wrap_cpp(libkdeprint_cups_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_cups
    SOURCES
        ${libkdeprint_cups_SOURCES}
    INCLUDE_DIRECTORIES
        ${CUPS_INCLUDE_DIR}
    LIBS
        kde2::kdeprint_management
        kde2::kdeui
        ${CUPS_LIBRARIES}
    )

install(FILES lpd.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )

add_executable(make_driver_db_cups make_driver_db_cups.c)
target_link_libraries(make_driver_db_cups ZLIB::ZLIB)
add_executable(cupsdoprint cupsdoprint.c)
target_link_libraries(cupsdoprint ${CUPS_LIBRARIES})

install(TARGETS make_driver_db_cups cupsdoprint RUNTIME DESTINATION ${KDE2_BINDIR})
