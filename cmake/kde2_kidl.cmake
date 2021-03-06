macro(KDE2_KIDL)
    foreach(Input ${ARGN})
        get_filename_component(FILENAME ${Input} NAME_WE)
        get_filename_component(FULLFILENAME ${Input} ABSOLUTE)

        add_custom_command(OUTPUT ${FILENAME}.kidl
            COMMAND dcopidl ${FULLFILENAME} > ${FILENAME}.kidl
            VERBATIM
            DEPENDS ${FULLFILENAME}
            COMMENT "Generating KIDL for ${Input}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
        add_custom_command(OUTPUT ${FILENAME}_skel.cpp
            COMMAND dcopidl2cpp --c++-suffix cpp --no-stub ${FILENAME}.kidl
            VERBATIM
            DEPENDS ${FILENAME}.kidl
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
        add_custom_command(OUTPUT ${FILENAME}_stub.cpp ${FILENAME}_stub.h
            COMMAND dcopidl2cpp --c++-suffix cpp --no-skel ${FILENAME}.kidl
            VERBATIM
            DEPENDS ${FILENAME}.kidl
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
    endforeach()
endmacro()

