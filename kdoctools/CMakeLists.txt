add_subdirectory(customization)
add_subdirectory(docbook)

set(FILTER_SOURCES
    ${PROJECT_SOURCE_DIR}/kio/gzip/kgzipfilter.cpp
    ${PROJECT_SOURCE_DIR}/kio/bzip2/kbzip2filter.cpp
    )

set(kio_help_SOURCES kio_help.cpp main.cpp xslt.cpp)

kde2_module(kio_help
    SOURCES
        ${FILTER_SOURCES}
        ${kio_help_SOURCES}
    LIBS
        kde2::ksycoca
        BZip2::BZip2
        ${LIBXML2_LIBRARY}
        ${LIBXSLT_LIBRARIES}
    COMPILE_DEFINITIONS
        -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    INCLUDE_DIRECTORIES
        ${LIBXML2_INCLUDE_DIR}
    )

#install(FILES help.protocol DESTINATION ${KDE2_SERVICESDIR})

add_executable(meinproc xslt.cpp meinproc.cpp ${FILTER_SOURCES})
target_compile_definitions(meinproc
    PRIVATE -DSIMPLE_XSLT -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_include_directories(meinproc PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(meinproc
    kde2::ksycoca
    BZip2::BZip2
    ${LIBXML2_LIBRARY}
    ${LIBXSLT_LIBRARIES}
    )

add_executable(xml2man xml2man.cpp xslt.cpp ${FILTER_SOURCES})
target_compile_definitions(xml2man
    PRIVATE -DSIMPLE_XSLT -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_include_directories(xml2man PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(xml2man
    kde2::ksycoca
    BZip2::BZip2
    ${LIBXML2_LIBRARY}
    ${LIBXSLT_LIBRARIES}
    )

configure_file(checkXML.in ${CMAKE_CURRENT_BINARY_DIR}/checkXML @ONLY)

install(TARGETS meinproc xml2man RUNTIME DESTINATION ${KDE2_BINDIR})

install(FILES kdb2html ${CMAKE_CURRENT_BINARY_DIR}/checkXML
    DESTINATION ${CMAKE_INSTALL_BINDIR})
