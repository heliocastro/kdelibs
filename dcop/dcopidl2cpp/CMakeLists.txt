add_executable(dcopidl2cpp
    main.cpp
    skel.cpp
    stub.cpp
    stubimpl.cpp
    )

target_link_libraries(dcopidl2cpp
    PRIVATE
        Qt::Qt2
    )
