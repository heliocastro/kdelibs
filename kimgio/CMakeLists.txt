
if(TIFF_FOUND)
    list(APPEND TIFF_KIMGIO tiff.kimgio g3.kimgio)
    kde2_module(kimg_tiff
        SOURCES
            tiffr.cpp
        LIBS
            TIFF::TIFF
        )

    kde2_module(kimg_g3
        SOURCES
            g3r.cpp
        LIBS
            TIFF::TIFF
        )
endif()

kde2_module(kimg_xview
    SOURCES
        xview.cpp
    )

kde2_module(kimg_krl
    SOURCES
        krl.cpp
    )

kde2_module(kimg_eps
    SOURCES
        eps.cpp
    PRIVATE_LIBS
        kde2::kdecore
    )

kde2_module(kimg_ico
    SOURCES
        ico.cpp
    )

install(FILES
        png.kimgio xpm.kimgio bmp.kimgio gif.kimgio pnm.kimgio xbm.kimgio
        jpeg.kimgio xv.kimgio krl.kimgio eps.kimgio ${TIFF_KIMGIO} ico.kimgio
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/services/
    )
