set(libmcop_SRCS
    common.cpp buffer.cc connection.cc core.cc dispatcher.cc
    iomanager.cc object.cc socketconnection.cc tcpconnection.cc
    unixconnection.cc tcpserver.cc unixserver.cc objectmanager.cc
    factory.cc idlfilereg.cc ifacerepo_impl.cc mcoputils.cc
    startupmanager.cc md5.c md5auth.cc referenceclean.cc
    datapacket.cc asyncstream.cc notification.cc flowsystem.cc
    extensionloader.cc tmpglobalcomm.cc mcopconfig.cc connect.cc
    reference.cc type.cc trader_impl.cc dynamicrequest.cc
    anyref.cc loopback.cc debug.cc delayedreturn.cc thread.cc
    dynamicskeleton.cc
    )

kde2_library(libmcop
    SOURCES
        ${libmcop_SRCS}
    LIBS
        ltdl
        dl
    COMPILE_DEFINITIONS
        -DEXTENSION_DIR="${CMAKE_INSTALL_FULL_LIBDIR}"
        -DTRADER_DIR="${CMAKE_INSTALL_FULL_LIBDIR}/mcop"
    )
