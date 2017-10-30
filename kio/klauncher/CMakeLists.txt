qt2_wrap_cpp(MOC_SOURCES
    SOURCES
        klauncher.h)

add_executable(klauncher
    klauncher.cpp
    main.cpp
    autostart.cpp
    ${MOC_SOURCES}
    )

target_link_libraries(klauncher kde2::ksycoca)
