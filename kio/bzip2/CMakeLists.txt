set(kbzip2filter_SOURCES kbzip2filter.cpp)

kde2_module(kbzip2filter
    SOURCES
        ${kbzip2filter_SOURCES}
    LIBS
        kde2::ksycoca
        BZip2::BZip2
    )

install(
    FILES kbzip2filter.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )
