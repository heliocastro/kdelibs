set(libkjs_SOURCES
    array_object.cpp
    bool_object.cpp
    collector.cpp
    date_object.cpp
    debugger.cpp
    error_object.cpp
    function.cpp
    function_object.cpp
    global_object.cpp
    internal.cpp
    kjs.cpp
    lexer.cpp
    lookup.cpp
    math_object.cpp
    nodes.cpp
    number_object.cpp
    object.cpp
    object_object.cpp
    operations.cpp
    regexp.cpp
    regexp_object.cpp
    string_object.cpp
    testkjs.cpp
    types.cpp
    ustring.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/grammar.cpp
    )

set(libkjs_HEADERS
    array_object.h
    bool_object.h
    collector.h
    date_object.h
    debugger.h
    error_object.h
    function_object.h
    grammar.h
    internal.h
    lexer.h
    lexer.lut.h
    math_object.h
    math_object.lut.h
    nodes.h
    number_object.h
    object_object.h
    regexp.h
    regexp_object.h
    string_object.h
    )

set(libkjs_noinst_HEADERS
    kjs.h
    object.h
    operations.h
    ustring.h
    function.h
    lookup.h
    types.h
    )

kde2_library(libkjs
    SOURCES
        ${libkjs_SOURCES}
    LIBS
        ${PCRE_LIBRARIES}
    )

add_custom_command(TARGET libkjs
    PRE_BUILD
    COMMAND ${BISON_EXECUTABLE}
    ARGS -d -y -p kjsyy ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y -o grammar.cpp
    COMMAND  mv
    ARGS grammar.hpp grammar.h
    DEPENDS grammar.y
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

add_executable(testkjs testkjs.cpp)
target_link_libraries(testkjs kde2::kjs)
