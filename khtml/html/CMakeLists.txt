set(libkhtmlhtml_SRCS
    htmlparser.cpp htmltokenizer.cpp dtd.cpp html_headimpl.cpp html_blockimpl.cpp
    html_elementimpl.cpp html_inlineimpl.cpp html_documentimpl.cpp html_baseimpl.cpp html_imageimpl.cpp
    html_listimpl.cpp html_miscimpl.cpp html_formimpl.cpp html_objectimpl.cpp html_tableimpl.cpp
    )

set(libkhtmlhtml_HEADERS
    dtd.h html_headimpl.h html_tableimpl.h html_baseimpl.h html_imageimpl.h
    htmlparser.h html_blockimpl.h html_inlineimpl.h html_documentimpl.h html_listimpl.h
    htmltokenizer.h html_elementimpl.h html_miscimpl.h html_formimpl.h html_objectimpl.h
    )

qt2_wrap_cpp(libkhtmlhtml_SRCS
    SOURCES
        ${libkhtmlhtml_HEADERS}
    )

get_target_property(KJS_INCLUDES kde2::kjs INCLUDE_DIRECTORIES)

add_library(libkhtmlhtml OBJECT ${libkhtmlhtml_SRCS})
target_include_directories(libkhtmlhtml PUBLIC ${KJS_INCLUDES})
set_target_properties(libkhtmlhtml PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
