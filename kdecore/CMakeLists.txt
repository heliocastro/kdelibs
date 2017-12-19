set(kdecore_SRCS
    libintl.cpp kdebug.cpp netwm.cpp kapp.cpp
    kconfigbase.cpp kstyle.cpp
    kconfig.cpp  ksimpleconfig.cpp kdesktopfile.cpp
    kconfigbackend.cpp kglobalaccel.cpp
    kstddirs.cpp kuniqueapp.cpp ksock.cpp
    kprocess.cpp kprocctrl.cpp klocale.cpp krfcdate.cpp
    kiconeffect.cpp kicontheme.cpp kiconloader.cpp
    kstdaccel.cpp kwin.cpp kwinmodule.cpp krootprop.cpp kcharsets.cpp
    kpixmap.cpp kckey.cpp kaccel.cpp kcrash.cpp
    kurl.cpp kregexp.cpp
    kglobal.cpp kglobalsettings.cpp dmalloc.cpp
    kallocator.cpp kmimesourcefactory.cpp
    kinstance.cpp kpalette.cpp kipc.cpp klibloader.cpp ktempfile.cpp
    ksavefile.cpp krandomsequence.cpp kstringhandler.cpp kcompletion.cpp
    kcmdlineargs.cpp kaboutdata.cpp kcompletionbase.cpp knotifyclient.cpp
    kaudioplayer.cpp kdestyle.cpp kdcoppropertyproxy.cpp
    kextsock.cpp ksockaddr.cpp netsupp.cpp kbufferedio.cpp
    kpixmapprovider.cpp kurldrag.cpp kpixmapsplitter.cpp
    kmdcodec.cpp ksocks.cpp vsnprintf.c
    ksycoca.cpp ksycocadict.cpp ksycocafactory.cpp
    kxmessages.cpp kstartupinfo.cpp kdesktopwidget.cpp
    )


qt2_wrap_cpp(kdecore_SRCS
    SOURCES
        kaccel.h
        kapp.h
        kaudioplayer.h
        kbufferedio.h
        kcompletion.h
        kconfigbase.h
        kconfig.h
        kdesktopfile.h
        kdestyle.h
        kextsock.h
        kextsocklookup.h
        kglobalaccel.h
        klibloader.h
        kprocctrl.h
        kprocess.h
        ksimpleconfig.h
        ksockaddr.h
        ksock.h
        kstartupinfo.h
        kstyle.h
        ksycoca.h
        kuniqueapp.h
        kwinmodule.h
        kxmessages.h
    )

qt2_wrap_moc(kdecore_SRCS
    SOURCES
        kasyncio.h
        kdesktopwidget.h
        )

kde2_library(kdecore
    SOURCES
        ${kdecore_SRCS}
    PRIVATE_LIBS
        ltdl
        dl
    COMPILE_OPTIONS
        -Wno-format-security
    LIBS
        Qt::Qt2
        kde2::DCOP
        ${X11_LIBRARIES}
    )

configure_file(kde-config.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/kde-config.cpp @ONLY)

add_executable(kde-config ${CMAKE_CURRENT_BINARY_DIR}/kde-config.cpp)
target_link_libraries(kde-config kde2::kdecore)
install(TARGETS kde-config  RUNTIME DESTINATION ${KDE2_BINDIR})
