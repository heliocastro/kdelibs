set(libkab_SRCS
    kabapi.cc addressbook.cc qconfigDB.cc
    )

set(libkab_HEADERS
    kabapi.h addressbook.h qconfigDB.h
    )

qt2_wrap_cpp(libkab_SRCS
    SOURCES
        ${libkab_HEADERS}
    )

install(FILES ${libkab_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkab
    SOURCES
        ${libkab_SRCS}
    LIBS
        kde2::kdeui
    VERSION
        3.0.0
    )

install(FILES template.kab template.config DESTINATION ${KDE2_DATADIR}/kab)
