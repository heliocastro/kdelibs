macro(KDE2_ICON app)
    foreach(real_icon ${ARGN})
        STRING(REPLACE "hi16" "icons/hicolor/16x16" icon ${real_icon})
        STRING(REPLACE "hi24" "icons/hicolor/24x24" icon ${icon})
        STRING(REPLACE "hi32" "icons/hicolor/32x32" icon ${icon})
        STRING(REPLACE "hi48" "icons/hicolor/48x48" icon ${icon})
        STRING(REPLACE "hi64" "icons/hicolor/64x64" icon ${icon})
        STRING(REPLACE "app" "apps" icon ${icon})
        STRING(REPLACE "-" "/" icon ${icon})
        get_filename_component(icon_path ${icon} DIRECTORY)
        get_filename_component(icon_name ${icon} NAME)
        if(${app} EQUAL "icondir")
            install(FILES ${real_icon}
                DESTINATION ${CMAKE_INSTALL_DATADIR}/${icon_path}
                RENAME ${icon_name}
                )
        else()
            install(FILES ${real_icon}
                DESTINATION ${CMAKE_INSTALL_DATADIR}/apps/${app}/${icon_path}
                RENAME ${icon_name}
                )
        endif()
    endforeach()
endmacro()



