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
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(FILES
    kcookiejar.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/services
    )

install(FILES
    kcookiescfg.upd
    DESTINATION ${CMAKE_INSTALL_DATADIR}
    )

