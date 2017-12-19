macro(KDE2_ICON app)
    foreach(real_icon ${ARGN})
        string(REPLACE "hi16" "icons/hicolor/16x16" icon ${real_icon})
        string(REPLACE "hi24" "icons/hicolor/24x24" icon ${icon})
        string(REPLACE "hi32" "icons/hicolor/32x32" icon ${icon})
        string(REPLACE "hi48" "icons/hicolor/48x48" icon ${icon})
        string(REPLACE "hi64" "icons/hicolor/64x64" icon ${icon})
        string(REPLACE "app" "apps" icon ${icon})
        string(REPLACE "-" "/" icon ${icon})
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

macro(KDE2_STDICON)
    set(oneValueArgs OUTPUT_DIR)
    cmake_parse_arguments(_ico "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    file(GLOB _ico_FILES LIST_DIRECTORIES false hi*.png)
    foreach(icon_file ${_ico_FILES})
        get_filename_component(real_icon ${icon_file} NAME)
        string(REGEX REPLACE ".*([0-9][0-9]).*" "\\1" size ${real_icon})
        string(REGEX REPLACE "^.*-([A-Za-z_]+).*" "\\1" name ${real_icon})
        string(REGEX REPLACE "hi[0-9][0-9]-([A-Za-z_]+).*" "\\1" type ${real_icon})
        set(outdir "${KDE2_DATADIR}/apps/${_ico_OUTPUT_DIR}")
            install(
                FILES ${real_icon}
                DESTINATION "${outdir}/hicolor/${size}x${size}/${type}s"
                RENAME ${name}.png
                )
    endforeach()
endmacro()
