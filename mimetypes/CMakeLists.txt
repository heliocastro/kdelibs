add_subdirectory(application)
add_subdirectory(text)
add_subdirectory(audio)
add_subdirectory(video)
add_subdirectory(image)
add_subdirectory(inode)
add_subdirectory(message)

install(FILES
    all.desktop allfiles.desktop
    DESTINATION ${KDE2_MIMEDIR}/all
    )
