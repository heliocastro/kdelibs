set(libkhtmlxml_SRCS
    dom_docimpl.cpp dom_nodeimpl.cpp dom_textimpl.cpp dom_elementimpl.cpp dom_stringimpl.cpp
    dom2_rangeimpl.cpp dom2_traversalimpl.cpp xml_tokenizer.cpp dom_xmlimpl.cpp dom2_eventsimpl.cpp
    dom2_viewsimpl.cpp
    )

set(libkhtmlxml_HEADERS
    dom_docimpl.h dom_nodeimpl.h dom_textimpl.h dom_elementimpl.h dom_stringimpl.h
    dom2_rangeimpl.h dom2_traversalimpl.h xml_tokenizer.h dom_xmlimpl.h dom2_eventsimpl.h
    dom2_viewsimpl.h
    )

qt2_wrap_cpp(libkhtmlxml_SRCS
    SOURCES
        ${libkhtmlxml_HEADERS}
    )

add_library(libkhtmlxml OBJECT ${libkhtmlxml_SRCS})

set_target_properties(libkhtmlxml PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
