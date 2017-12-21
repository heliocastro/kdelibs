qt2_wrap_cpp(MOC_SRCS
    SOURCES
        kcookiejar.h
        kcookiewin.h
        kcookieserver.h
    )

add_executable(kcookiejar
    kcookiejar.cpp
    kcookieserver.cpp
    kcookiewin.cpp
    main.cpp
    ${MOC_SRCS}
)

target_link_libraries(kcookiejar
    kde2::kio
    )

install(TARGETS kcookiejar
    RUNTIME DESTINATION ${KDE2_BINDIR}
    )

install(FILES
    kcookiejar.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )

install(FILES
    kcookiescfg.upd
    DESTINATION ${KDE2_DATADIR}/kconf_update
    )

