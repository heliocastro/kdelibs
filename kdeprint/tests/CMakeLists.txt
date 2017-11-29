qt2_wrap_cpp(HELPVIEWER_MOC SOURCES helpwindow.h)
add_executable(helpviewer helpwindow.cpp main.cpp richpage.cpp ${HELPVIEWER_MOC})
target_link_libraries(helpviewer kde2::kdeprint)

qt2_wrap_cpp(DRAWDEMO_MOC SOURCES drawdemo.h)
add_executable(drawdemo drawdemo.cpp  ${DRAWDEMO_MOC})
target_link_libraries(drawdemo kde2::kdeprint)
