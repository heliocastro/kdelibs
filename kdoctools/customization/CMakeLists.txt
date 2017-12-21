dirlist(dirs)
foreach(dir ${dirs})
    install(DIRECTORY ${dir} DESTINATION ${KDE2_DATADIR}/ksgmltools2/customization/)
endforeach()

install(FILES
    htdig_index.xsl kde-chunk.xsl kde-navig.xsl kde-style.xsl
    kde-ttlpg.xsl catalog
    DESTINATION ${KDE2_DATADIR}/ksgmltools2/customization
    )
