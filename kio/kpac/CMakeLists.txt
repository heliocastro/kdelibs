set(libkpac_SOURCES
    kproxybindings.cpp
    kpac_impl.cpp
    kpac_downloader.cpp
    kpac_discovery.cpp
    )

set(kpac_noint_HEADERS
    kproxybindings.h
    kpac_impl.h
    kpac_downloader.h
    kpac_discovery.h
    dhcp.h
    )

qt2_wrap_cpp(kpac_SOURCES
    ${kpac_noint_HEADERS}
    )

add_library(libkpac MODULE
        ${kpac_SOURCES}
        )
target_link_libraries(libkpac
        kde2::kio
        kde2::kjs
    )
