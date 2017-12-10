set(kssld_SOURCES
    kssld.cpp
    )

qt2_wrap_cpp(kssld_SOURCES
    SOURCES
        kssld.h
    )

kde2_module(libkded_kssld
    SOURCES
        ${kssld_SOURCES}
    LIBS
        kde2::ksycoca
    ALLOW_UNDEFINED
    )

kde2_kidl(kssld.h)

install(
    FILES kssld.desktop
    DESTINATION ${KDE2_SERVICESDIR}/kded
    )
