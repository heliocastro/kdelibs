set(libkpac_SOURCES
    kproxybindings.cpp
    kpac_impl.cpp
    kpac_downloader.cpp
    kpac_discovery.cpp
    )

set(kpac_noinst_HEADERS
    kproxybindings.h
    kpac_impl.h
    kpac_downloader.h
    kpac_discovery.h
    dhcp.h
    )

qt2_wrap_cpp(libkpac_SOURCES
    SOURCES
        ${kpac_noinst_HEADERS}
    )

kde2_module(libkpac
    SOURCES
        ${libkpac_SOURCES}
    LIBS
        kde2::kio
        kde2::kjs
    )

add_executable(kpac_dhcp_helper kpac_dhcp_helper.c)
install(TARGETS kpac_dhcp_helper RUNTIME DESTINATION ${KDE2_BINDIR})
