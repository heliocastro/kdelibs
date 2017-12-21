install(FILES kdeprint_collate.png kdeprint_collate_reverse.png kdeprint_color.png kdeprint_duplex_none.png
    kdeprint_duplex_long.png kdeprint_duplex_short.png kdeprint_grayscale.png kdeprint_landscape.png
    kdeprint_nup1.png kdeprint_nup2.png kdeprint_nup4.png kdeprint_portrait.png kdeprint_revlandscape.png
    kdeprint_revportrait.png kdeprint_uncollate.png kdeprint_uncollate_reverse.png
    DESTINATION ${KDE2_DATADIR}/kdeprint/pics
    )

kde2_stdicon(OUTPUT_DIR "${KDE2_DATADIR}/kdeprint/icons")
