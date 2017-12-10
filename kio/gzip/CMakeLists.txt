set(kgzipfilter_SOURCES kgzipfilter.cpp)

kde2_module(kgzipfilter
    SOURCES
        ${kgzipfilter_SOURCES}
    LIBS
        kde2::ksycoca
        ZLIB::ZLIB
    )

install(
    FILES kgzipfilter.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )
