add_subdirectory(acl_prop_page)

set(libkfile_SRCS
    kfilebookmark.cpp kfiledialogconf.cpp kfilereader.cpp kfileviewitem.cpp booktoken.cpp
    kfileview.cpp kfileiconview.cpp kfilefilter.cpp krecentdocument.cpp kfiledialog.cpp
    kdiroperator.cpp kfiledetailview.cpp kcombiview.cpp kurlrequester.cpp kfilepreview.cpp
    kurlcombobox.cpp kurlrequesterdlg.cpp kopenwith.cpp kpropsdlg.cpp kicondialog.cpp
    kdirsize.cpp krecentdirs.cpp kdiskfreesp.cpp kimagefilepreview.cpp
    )

set(libkfile_HEADERS
    kfiledialog.h kfileviewitem.h kfilebookmark.h kfilereader.h kdiroperator.h
    kfileview.h booktoken.h kfiledetailview.h kcombiview.h kfileiconview.h
    kfilefilter.h krecentdocument.h kfileview.h kfiledialog.h kurlrequester.h
    kfilepreview.h kfile.h kurlcombobox.h kurlrequesterdlg.h kopenwith.h
    kpropsdlg.h kicondialog.h kdirsize.h kpreviewwidgetbase.h kimagefilepreview.h
    )

set(libkfile_NOINST_HEADERS
    kfiledialogconf.h kfilefilter.h config-kfile.h krecentdirs.h
    )

qt2_wrap_cpp(libkfile_SRCS
    SOURCES
        ${libkfile_HEADERS}
        ${libkfile_NOINST_HEADERS}
    )

install(FILES ${libkfile_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkfile
    SOURCES
        ${libkfile_SRCS}
    LIBS
        kde2::ksycoca
    VERSION
        3.0.0
    )

set(kfstest_SRCS
    kfstest.cpp
    )

add_executable(kfstest ${kfstest_SRCS})

target_link_libraries(kfstest
    kde2::kfile
    )

set(kurlrequestertest_SRCS
    kurlrequestertest.cpp
    )

add_executable(kurlrequestertest ${kurlrequestertest_SRCS})

target_link_libraries(kurlrequestertest
    kde2::kfile
    )

install(FILES
    kpropsdlgplugin.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

install(FILES
    kpropsdlgplugin.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

