set(libkhtmlrender_SRCS
    bidi.cpp break_lines.cpp render_style.cpp render_object.cpp render_container.cpp
    render_box.cpp render_flow.cpp render_text.cpp render_image.cpp render_table.cpp
    render_hr.cpp render_replaced.cpp render_form.cpp render_list.cpp render_root.cpp
    render_frames.cpp render_br.cpp render_applet.cpp render_html.cpp render_body.cpp
    )

set(libkhtmlrender_HEADERS
    bidi.h break_lines.h render_hr.h render_style.h render_object.h
    render_container.h render_box.h render_flow.h render_text.h render_box.h
    render_replaced.h render_form.h render_list.h render_root.h render_frames.h
    render_br.h render_applet.h
    )

qt2_wrap_cpp(libkhtmlrender_SRCS
    SOURCES
        ${libkhtmlrender_HEADERS}
    )

get_target_property(KFILE_INCLUDES kde2::kfile INCLUDE_DIRECTORIES)


add_library(libkhtmlrender OBJECT ${libkhtmlrender_SRCS})
target_include_directories(libkhtmlrender PUBLIC ${KFILE_INCLUDES})
set_target_properties(libkhtmlrender PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )

