set(libkhtmldom_SRCS
    dom_misc.cpp html_block.cpp html_inline.cpp css_rule.cpp dom_node.cpp
    html_document.cpp html_list.cpp css_stylesheet.cpp dom_string.cpp html_element.cpp
    html_misc.cpp css_value.cpp dom_text.cpp html_object.cpp dom_doc.cpp
    dom_xml.cpp html_head.cpp dom_element.cpp html_base.cpp html_image.cpp
    html_form.cpp dom2_range.cpp html_table.cpp dom2_traversal.cpp dom2_events.cpp
    dom2_views.cpp
    )

set(libkhtmldom_HEADERS
    dom_exception.h html_base.h html_inline.h
    css_rule.h        dom_html.h        html_block.h      html_list.h
    css_stylesheet.h  dom_misc.h        html_document.h   html_misc.h
    css_value.h       dom_node.h        html_element.h    html_object.h
    dom_core.h        dom_string.h      html_form.h       html_table.h
    dom_doc.h         dom_text.h        html_head.h
    dom_element.h     dom_xml.h         html_image.h
    dom2_range.h      dom2_traversal.h  dom2_events.h
    dom2_views.h
    )

qt2_wrap_cpp(libkhtmldom_SRCS
    SOURCES
        ${libkhtmldom_HEADERS}
    )

install(FILES ${libkhtmldom_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/dom)

add_library(libkhtmldom OBJECT ${libkhtmldom_SRCS})

set_target_properties(libkhtmldom PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
