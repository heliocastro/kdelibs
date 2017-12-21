bison_target(MyParser
    grammar.y
    ${CMAKE_CURRENT_BINARY_DIR}/grammar.cpp
    COMPILE_FLAGS "-y -d -p kjsyy"
    )

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
    ${BISON_MyParser_OUTPUTS}
    )

set(libkjs_noinst_HEADERS
    nodes.h
    lexer.h
    regexp.h
    internal.h
    collector.h
    object_object.h
    function_object.h
    bool_object.h
    math_object.lut.h
    array_object.h
    string_object.h
    number_object.h
    date_object.h
    regexp_object.h
    error_object.h
    debugger.h
    )

set(libkjs_HEADERS
    kjs.h
    object.h
    operations.h
    ustring.h
    function.h
    lookup.h
    types.h
    )

install(FILES ${libkjs_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kjs)

kde2_library(libkjs
    SOURCES
        ${libkjs_SOURCES}
    LIBS
        ${PCRE_LIBRARIES}
    VERSION
        1.0.0
    )

add_executable(testkjs testkjs.cpp)
target_link_libraries(testkjs kde2::kjs)
