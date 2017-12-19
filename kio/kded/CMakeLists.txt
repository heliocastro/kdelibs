set(kded_module_SOURCES
    kded.cpp
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kdedmodule.cpp
    )

set(kbuildsycoca_module_SOURCES
    kbuildsycoca.cpp
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kbuildimageiofactory.cpp
    kbuildprotocolinfofactory.cpp
    kctimefactory.cpp
    )

qt2_wrap_cpp(kded_module_SOURCES
    SOURCES
        kded.h
        kdedmodule.h
    )

qt2_wrap_cpp(kbuildsycoca_module_SOURCES
    SOURCES
        kbuildsycoca.h
    )

kde2_module(kded_module
    SOURCES
        ${kded_module_SOURCES}
    PRIVATE_LIBS
        Qt::Qt2
        kde2::DCOP
        kde2::ksycoca
    OUTPUT_NAME
        kded
    )

kde2_module(kbuildsycoca_module
    SOURCES
        ${kbuildsycoca_module_SOURCES}
    PRIVATE_LIBS
        Qt::Qt2
        kde2::ksycoca
    OUTPUT_NAME
        kbuildsycoca
    )

add_executable(kbuildsycoca ${kbuildsycoca_module_SOURCES})
target_link_libraries(kbuildsycoca
    Qt::Qt2
    kde2::ksycoca
    )

add_executable(kded ${kded_module_SOURCES})
target_link_libraries(kded
    Qt::Qt2
    kde2::DCOP
    kde2::ksycoca
    )

add_executable(kdontchangethehostname khostname.cpp)
target_link_libraries(kdontchangethehostname
    kde2::kdecore
    )

install(TARGETS kbuildsycoca kded kdontchangethehostname
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
