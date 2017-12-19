set(settheme_SRCS
    main.cpp
    )

add_executable(settheme ${settheme_SRCS})

target_link_libraries(settheme
    kde2::kdeui
    )

install(TARGETS settheme RUNTIME DESTINATION ${KDE2_BINDIR})
