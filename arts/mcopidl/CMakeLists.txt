flex_target(MyScanner scanner.ll
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
    COMPILE_FLAGS "-B -B"
    )

bison_target(MyParser
    yacc.yy
    ${CMAKE_CURRENT_BINARY_DIR}/yacc.cpp
    COMPILE_FLAGS "-d -t"
    )

add_flex_bison_dependency(MyScanner MyParser)

add_executable(mcopidl
    mcopidl.cc
    namespace.cc
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/yacc.cpp
    )

target_include_directories(mcopidl
    PUBLIC
        ${PROJECT_SOURCE_DIR}/arts/mcop
    )

install(TARGETS mcopidl RUNTIME DESTINATION ${KDE2_BINDIR})
