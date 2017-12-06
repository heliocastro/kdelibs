add_executable(dcop dcop.cpp)
target_link_libraries(dcop
    PRIVATE
        Qt::Qt2
        kde2::DCOP
    )
