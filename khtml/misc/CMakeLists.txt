set(libkhtmlmisc_SRCS
    decoder.cpp loader.cpp loader_jpeg.cpp htmlhashes.cpp helper.cpp
    )

set(libkhtmlmisc_HEADERS
    decoder.h khtmllayout.h loader_jpeg.h loader.h stringit.h
    htmlhashes.h helper.h font.h
    )

qt2_wrap_cpp(libkhtmlmisc_SRCS
    SOURCES
        ${libkhtmlmisc_HEADERS}
    )

add_library(libkhtmlmisc OBJECT ${libkhtmlmisc_SRCS})

set_target_properties(libkhtmlmisc PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
