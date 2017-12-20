add_subdirectory(kde)

kde2_stdicon("")

install(FILES
    index.desktop
    DESTINATION ${KDE2_ICONDIR}/hicolor
    )

