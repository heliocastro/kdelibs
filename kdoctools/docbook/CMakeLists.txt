dirlist(dirs)
foreach(dir ${dirs})
    install(DIRECTORY ${dir} DESTINATION ${KDE2_DATADIR}/ksgmltools2/docbook)
endforeach()
