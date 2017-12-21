set(kio_ftp_SRCS
    ftp.cc
    )

kde2_module(kio_ftp
    SOURCES
        ${moc_SRCS}
        ${kio_ftp_SRCS}
    LIBS
        kde2::ksycoca
    )

set(global_NOINST_HEADERS
    ftp.h
    )

qt2_wrap_cpp(moc_SRCS
    SOURCES
        ${global_NOINST_HEADERS}
    )

install(FILES
    ftp.protocol
    DESTINATION ${KDE2_SERVICESDIR}
    )

