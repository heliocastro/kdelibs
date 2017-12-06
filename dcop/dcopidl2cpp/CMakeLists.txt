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

install(TARGETS dcopidl2cpp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
