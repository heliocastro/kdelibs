function(kde2_module library_name)
    include(GNUInstallDirs)
    include(CMakeParseArguments)
    include(GenerateExportHeader)
    include(CMakePackageConfigHelpers)

    set(oneValueArgs OUTPUT_NAME)
    set(multiValueArgs
        LIBS
        PRIVATE_LIBS
        COMPILE_OPTIONS
        COMPILE_DEFINITIONS
        SOURCES
        INCLUDE_DIRECTORIES
        )
    cmake_parse_arguments(_lib "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _lib_SOURCES)
        message(FATAL_ERROR "Error adding library ${library_name}: No SOURCES specified.")
    endif()

    set(output_name ${library_name})
    if(_lib_OUTPUT_NAME)
        set(output_name ${_lib_OUTPUT_NAME})
    endif()

    add_library(${library_name} MODULE ${_lib_SOURCES})
    target_link_libraries(${library_name}
        PUBLIC
            ${_lib_LIBS}
        PRIVATE
            ${_lib_PRIVATE_LIBS}
            Qt::Qt2
        )

    # We need iteract over the multiple directory list
    foreach(include ${_lib_INCLUDE_DIRECTORIES})
        list(APPEND build_interface_includes "$<BUILD_INTERFACE:${include}>")
    endforeach()

    target_include_directories(${library_name}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            ${build_interface_includes}
        )
    target_compile_options(${library_name} PRIVATE
        ${_lib_COMPILE_OPTIONS}
        ${default_options}
        )
    if(_lib_COMPILE_DEFINITIONS)
        target_compile_definitions(${library_name} PRIVATE
            ${_lib_COMPILE_DEFINITIONS}
            )
    endif()
    set_target_properties(${library_name} PROPERTIES
        LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
        OUTPUT_NAME ${output_name}
        PREFIX ""
        )

    install(TARGETS ${library_name}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        )

    get_filename_component(LAPROP_SONAME ${output_name} NAME)
    get_target_property(LAPROP_DEPENDENCY_LIBS ${library_name} INTERFACE_LINK_LIBRARIES)

    configure_file(${PROJECT_SOURCE_DIR}/cmake/kde2_libtool_template.la.in
        ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.la
        @ONLY
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.la
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        )
endfunction()
