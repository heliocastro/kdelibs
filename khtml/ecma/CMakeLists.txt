set(kjs_html_SRCS
    kjs_html.cpp kjs_dom.cpp kjs_window.cpp kjs_navigator.cpp kjs_text.cpp
    kjs_binding.cpp kjs_proxy.cpp kjs_css.cpp kjs_range.cpp kjs_traversal.cpp
    kjs_events.cpp kjs_views.cpp kjs_debugwin.cpp
    )

set(kjs_html_HEADERS
    kjs_html.h kjs_dom.h kjs_window.h kjs_navigator.h kjs_text.h
    kjs_binding.h kjs_proxy.h kjs_css.h kjs_range.h kjs_traversal.h
    kjs_events.h kjs_views.h kjs_debugwin.h
    )

qt2_wrap_cpp(kjs_html_SRCS
    SOURCES
        ${kjs_html_HEADERS}
    )

kde2_module(kjs_html
    SOURCES
        ${kjs_html_SRCS}
LIBS
    kde2::kjs kde2::khtml
    )
