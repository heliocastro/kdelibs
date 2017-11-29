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
    set(multiValueArgs FILES)
    cmake_parse_arguments(_ico "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(real_icon ${_ico_FILES})
        foreach(size 16 22 32 48 64)
            foreach(type app action)
                if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/hi${size}-${type}-${real_icon}.png")
                    set(outdir "${KDE2_DATADIR}/${type}s/${_ico_OUTPUT_DIR}")
                    string(REPLACE "hi${size}-${type}-" "" output_filename "hi${size}-${type}-${real_icon}.png")
                    install(
                        FILES "hi${size}-${type}-${real_icon}.png"
                        DESTINATION "${outdir}/hicolor/${size}x${size}/${type}s"
                        RENAME ${output_filename}
                        )
                endif()
            endforeach()
        endforeach()
    endforeach()
endmacro()
