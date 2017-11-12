add_subdirectory(kcookiejar)

qt2_wrap_cpp(MOC_SRCS
    SOURCES
        http.h
    )

kde2_module(kio_http
    SOURCES
        http.cc
        ${MOC_SRCS}
    LIBS
        ZLIB::ZLIB
        kde2::kio
    )


add_executable(kio_http_cache_cleaner
    http_cache_cleaner.cpp
    )

target_link_libraries(kio_http_cache_cleaner
    kde2::kio
    )

install(TARGETS kio_http_cache_cleaner
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(FILES
    http_cache_cleaner.desktop http.protocol https.protocol
    DESTINATION ${CMAKE_INSTALL_DATADIR}/services
    )

