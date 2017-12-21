add_subdirectory(klauncher)
add_subdirectory(kded)
add_subdirectory(file)
add_subdirectory(ftp)
add_subdirectory(http)
add_subdirectory(pics)
add_subdirectory(tests)
#add_subdirectory(ksendbugmail)
add_subdirectory(kpac)
add_subdirectory(gzip)
add_subdirectory(bzip2)
add_subdirectory(kssld)

set(ksycoca_SOURCES
    chmodjob.cpp
    kdirwatch.cpp
    ktrader.cpp
    ktraderparse.cpp
    ktraderparsetree.cpp
    kservicefactory.cpp
    kservicetype.cpp
    kmimetype.cpp
    kservicegroup.cpp
    kuserprofile.cpp
    kservicegroupfactory.cpp
    kservice.cpp
    kservicetypefactory.cpp
    kurifilter.cpp
    kfilterbase.cpp
    kfilterdev.cpp
    kshellcompletion.cpp
    kurlcompletion.cpp
    kautomount.cpp
    krun.cpp
    kfileitem.cpp
    kdirlister.cpp
    kimageio.cpp
    lex.c
    yacc.c
    kscan.cpp
    ktar.cpp
    previewjob.cpp
    )

set(kio_SOURCES
    kshred.cpp
    kprotocolmanager.cpp
    slave.cpp
    slaveinterface.cpp
    sessiondata.cpp
    scheduler.cpp
    connection.cpp
    job.cpp
    global.cpp
    slaveconfig.cpp
    netaccess.cpp
    paste.cpp
    kmimemagic.cpp
    tcpslavebase.cpp
    slavebase.cpp
    passdlg.cpp
    progressbase.cpp
    defaultprogress.cpp
    statusbarprogress.cpp
    kdirnotify.cpp
    observer.cpp
    kemailsettings.cpp
    authinfo.cpp
    kprotocolinfo.cpp
    kprotocolinfofactory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_stub.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_skel.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_stub.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/observer_skel.cpp
    )

kde2_kidl(kdirnotify.h uiserver.h observer.h)

set(kio_uiserver_SOURCES
    uiserver.cpp
    renamedlg.cpp
    skipdlg.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_skel.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/observer_stub.cpp
    )

set(kio_uiserver_HEADERS
    uiserver.h
    renamedlg.h
    skipdlg.h
    )

set(global_HEADERS
    kservicetype.h kmimetype.h kmimemagic.h kservice.h
    krun.h kdirwatch.h kautomount.h kuserprofile.h
    kshred.h ktar.h ktrader.h kurifilter.h kurlcompletion.h
    kshellcompletion.h kfileitem.h kdirlister.h kservicegroup.h
    progressbase.h defaultprogress.h statusbarprogress.h
    kimageio.h kdirnotify.h ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_stub.h
    kurlpixmapprovider.h kprotocolinfo.h kprotocolmanager.h
    kfilterbase.h kfilterdev.h kemailsettings.h kscan.h
    )

set(kio_HEADERS
    connection.h
    slaveinterface.h slave.h slaveconfig.h
    sessiondata.h global.h passdlg.h
    netaccess.h scheduler.h
    jobclasses.h paste.h slavebase.h
    progressbase.h defaultprogress.h
    statusbarprogress.h uiserver.h 
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_stub.h tcpslavebase.h
    renamedlg.h skipdlg.h
    observer.h chmodjob.h
    kpac.h kmdbase.h authinfo.h
    ioslave_defaults.h http_slave_defaults.h previewjob.h
)

install(FILES ${kio_HEADERS} job.h DESTINATION ${KDE2_INCLUDEDIR}/kio)
install(FILES ${global_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

set(global_NOINST_HEADERS
    kservicetypefactory.h
    kservicefactory.h
    kmessageboxwrapper.h
    ktraderparse.h
    ktraderparsetree.h
    kimageiofactory.h
    kprotocolinfofactory.h
    )

qt2_wrap_cpp(moc_SOURCES
    SOURCES
        ${global_HEADERS}
        ${global_NOINST_HEADERS}
    )


qt2_wrap_cpp(kio_SOURCES
    SOURCES
        ${kio_HEADERS}
    )

qt2_wrap_cpp(kio_uiserver_SOURCES
    SOURCES
        ${kio_uiserver_HEADERS}
    )

kde2_library(kio
    SOURCES
        ${moc_SOURCES}
        ${kio_SOURCES}
    LIBS
        kde2::kdeui
        kde2::kdesu
        kde2::kssl
        ZLIB::ZLIB
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/kded
    VERSION
        3.0.0
    )

kde2_library(ksycoca
    SOURCES
        ${moc_SOURCES}
        ${ksycoca_SOURCES}
    LIBS
        kde2::kio
        ltdl
    VERSION
        3.0.0
    )

kde2_module(kio_uiserver_module
    SOURCES
        ${kio_uiserver_SOURCES}
    LIBS
        kde2::ksycoca
        kde2::kssl
        kde2::kdesu
    OUTPUT_NAME
        kio_uiserver
    )

add_executable(kio_uiserver ${kio_uiserver_SOURCES})
target_link_libraries(kio_uiserver
        kde2::ksycoca
        kde2::kssl
        kde2::kdesu
    )
install(TARGETS kio_uiserver RUNTIME DESTINATION ${KDE2_BINDIR})

install(FILES magic DESTINATION ${KDE2_MIMEDIR})

install(FILES
    application.desktop kurifilterplugin.desktop kcomprfilter.desktop kscan.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

install(FILES
    kio_uiserver.desktop kmailservice.protocol telnet.protocol rlogin.protocol
    DESTINATION ${KDE2_SERVICESDIR}
    )

install(FILES
    kioslave.upd useragent.pl proxytype.pl
    DESTINATION ${KDE2_DATADIR}/kconf_update
    )
