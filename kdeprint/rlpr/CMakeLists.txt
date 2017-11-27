set(LIBKDEPRINT_RLPR_SOURCES
    kmrlprfactory.cpp
    kmrlprmanager.cpp
    krlprprinterimpl.cpp
    kmrlpruimanager.cpp
    kmwrlpr.cpp
    kmproprlpr.cpp
    kmproxywidget.cpp
    kmconfigproxy.cpp
    kmproxydialog.cpp
    )

set(PRIVATE_HEADERS
    kmrlprfactory.h
    kmrlprmanager.h
    krlprprinterimpl.h
    kmrlpruimanager.h
    kmwrlpr.h
    kmproprlpr.h
    kmproxywidget.h
    kmconfigproxy.h
    kmproxydialog.h
    )

qt2_wrap_cpp(LIBKDEPRINT_RLPR_SOURCES
    SOURCES
        ${PRIVATE_HEADERS}
    )

kde2_module(libkdeprint_rlpr
    SOURCES
        ${LIBKDEPRINT_RLPR_SOURCES}
    LIBS
        kde2::kdeprint_management
    )

install(FILES rlpr.print
    DESTINATION ${KDE2_DATADIR}/kdeprint/plugins
    )
