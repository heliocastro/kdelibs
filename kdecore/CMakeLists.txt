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


qt2_wrap_cpp(TARGET kdecore
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

add_library(kdecore SHARED
    ${kdecore_SRCS}

    )

set_target_properties(kdecore PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
)

target_link_libraries(kdecore
    Qt::Qt2
    libdcop
    ltdl
    dl
    ${X11_LIBRARIES}
    )

target_compile_definitions(kdecore PUBLIC "KDEDIR=\"${KDEDIR}\"")

target_compile_options(kdecore
    PRIVATE
        -fPIC
        -Wno-format-security
    )

target_include_directories(kdecore
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

#install(TARGETS  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
