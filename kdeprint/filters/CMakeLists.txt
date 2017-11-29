install(FILES
    enscript.desktop enscript.xml
    psnup.desktop psnup.xml
    imagetops.desktop imagetops.xml
    pdfwrite.desktop pdfwrite.xml
    psbook1.desktop psbook1.xml
    psbook2.desktop psbook2.xml
    psbook.desktop psbook.xml
    DESTINATION ${KDE2_DATADIR}/kdeprint/filters
    )

install(FILES imagetops
    PERMISSIONS WORLD_EXECUTE
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
