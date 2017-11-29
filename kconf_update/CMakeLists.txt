set(KCONF_UPDATE_SOURCES kconf_update.cpp)

kde2_module(kconf_update
    SOURCES
        ${KCONF_UPDATE_SOURCES}
    LIBS
        kde2::kdecore
    )

add_executable(kconf_update ${KCONF_UPDATE_SOURCES})
target_link_libraries(kconf_update kde2::kdecore)
install(TARGETS kconf_update RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

