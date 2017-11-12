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
        kde2::dcop
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

add_executable(kded ${kded_module_SOURCES})
target_link_libraries(kded
    Qt::Qt2
    kde2::dcop
    kde2::ksycoca
    )
