flex_target(MyScanner
    scanner.ll
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
    COMPILE_FLAGS "-B -8"
    )

bison_target(MyParser
    yacc.yy
    ${CMAKE_CURRENT_BINARY_DIR}/yacc.cpp
    COMPILE_FLAGS "-t -d -v")

add_flex_bison_dependency(MyScanner MyParser)

add_executable(dcopidl
    main.cpp
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    )

target_include_directories(dcopidl
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_libraries(dcopidl
    PRIVATE
        Qt::Qt2
        kde2::dcop
    )
