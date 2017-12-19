set(libkhtmlcss_SRCS
    css_stylesheetimpl.cpp css_ruleimpl.cpp css_valueimpl.cpp cssparser.cpp cssstyleselector.cpp
    csshelper.cpp
    )

set(libkhtmlcss_HEADERS
    css_extensionsimpl.h css_stylesheetimpl.h cssparser.h css_ruleimpl.h css_valueimpl.h
    cssstyleselector.h csshelper.h
    )

qt2_wrap_cpp(libkhtmlcss_SRCS
    SOURCES
        ${libkhtmlcss_HEADERS}
    )

add_library(libkhtmlcss OBJECT ${libkhtmlcss_SRCS})

install(FILES html4.css DESTINATION ${KDE2_DATADIR}/khtml/css)

set_target_properties(libkhtmlcss PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
