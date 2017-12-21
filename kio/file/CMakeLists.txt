qt2_wrap_cpp(MOC_SRCS
    SOURCES
        file.h
    )

kde2_module(kio_file
    SOURCES
        file.cc
        ${MOC_SRCS}
    LIBS
        kde2::kio
    COMPILE_DEFINITIONS
        _LARGEFILE64_SOURCE
    )

install(FILES
    file.protocol
    DESTINATION ${KDE2_SERVICESDIR}
    )

install(FILES file.h DESTINATION ${KDE2_INCLUDEDIR}/kio)
