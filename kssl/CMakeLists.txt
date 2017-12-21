add_subdirectory(kssl)

set(kssl_SOURCES
    ksslcertchain.cc
    ksslcertificatefactory.cc
    ksslsettings.cc
    ksslcertdlg.cc
    ksslinfodlg.cc
    kssl.cc
    ksslutils.cc
    ksslconnectioninfo.cc
    ksslpeerinfo.cc
    ksslx509map.cc
    ksslcertificatehome.cc
    ksslcertificate.cc
    ksslpkcs12.cc
    kopenssl.cc
    ksslcertificatecache.cc
    )

set(kssl_HEADERS
    kssl.h
    kssldefs.h
    ksslall.h
    ksslcertificate.h
    ksslcertificatehome.h
    ksslx509map.h
    kopenssl.h
    ksslinfodlg.h
    ksslpeerinfo.h
    ksslcertificatecache.h
    ksslcertificatefactory.h
    ksslpkcs12.h
    ksslcertdlg.h
    ksslcertchain.h
    ksslconnectioninfo.h
    ksslutils.h
    ksslsettings.h
    )

qt2_wrap_cpp(kssl_SOURCES
    SOURCES
        ${kssl_HEADERS}
    )

kde2_library(kssl
    SOURCES
        ${kssl_SOURCES}
    LIBS
        kde2::kdeui
    )

install(FILES ${kssl_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})
