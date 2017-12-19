add_subdirectory(klauncher)
add_subdirectory(kded)
add_subdirectory(file)
add_subdirectory(http)
add_subdirectory(pics)
add_subdirectory(tests)
#add_subdirectory(ksendbugmail)
add_subdirectory(kpac)
add_subdirectory(gzip)
add_subdirectory(bzip2)
add_subdirectory(kssld)

set(ksycoca_SOURCES
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
    chmodjob.cpp
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

set(ksycoca_HEADERS
    kdirwatch.h
    ktrader.h ktraderparse.h ktraderparsetree.h
    kservicetypefactory.h kservicetype.h
    kmimetype.h kservicegroup.h
    kservice.h kservicefactory.h
    kuserprofile.h kservicegroupfactory.h
    kurifilter.h
    kfilterbase.h kfilterdev.h
    kshellcompletion.h kurlcompletion.h
    kautomount.h krun.h
    kfileitem.h kdirlister.h kimageio.h
    chmodjob.h kscan.h ktar.h previewjob.h
    )

set(kio_HEADERS
    kshred.h
    kprotocolmanager.h
    slave.h slaveinterface.h
    sessiondata.h
    scheduler.h
    connection.h
    global.h
    slaveconfig.h
    netaccess.h paste.h
    kmimemagic.h
    tcpslavebase.h slavebase.h passdlg.h
    progressbase.h defaultprogress.h
    statusbarprogress.h
    kemailsettings.h authinfo.h
    kprotocolinfo.h kprotocolinfofactory.h
    jobclasses.h observer.h
)

set(ksycoca_noinst_HEADERS
    kservicetypefactory.h
    kservicefactory.h
    kmessageboxwrapper.h
    ktraderparse.h
    ktraderparsetree.h
    )

set(kio_noinst_HEADERS
    kimageiofactory.h
    kprotocolinfofactory.h
    )


qt2_wrap_cpp(ksycoca_SOURCES
    SOURCES
        ${ksycoca_HEADERS}
        ${ksycoca_noinst_HEADERS}
    )


qt2_wrap_cpp(kio_SOURCES
    SOURCES
        ${kio_HEADERS}
        ${kio_noinst_HEADERS}
    )

qt2_wrap_cpp(kio_uiserver_SOURCES
    SOURCES
        ${kio_uiserver_HEADERS}
    )

kde2_library(kio
    SOURCES
        ${kio_SOURCES}
    LIBS
        kde2::kdeui
        kde2::kdesu
        kde2::kssl
        ZLIB::ZLIB
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/kded
    )

kde2_library(ksycoca
    SOURCES
        ${ksycoca_SOURCES}
    LIBS
        kde2::kio
        ltdl
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
