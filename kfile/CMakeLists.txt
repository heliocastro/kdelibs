if(USE_POSIX_ACL)
    add_subdirectory(acl_prop_page)
endif()

set(KFILE_HEADERS kfiledialog.h kfileviewitem.h kfilebookmark.h
    kfilereader.h kdiroperator.h kfileview.h
    booktoken.h kfiledetailview.h kcombiview.h
    kfileiconview.h kfilefilter.h krecentdocument.h kfileview.h
    kfiledialog.h kurlrequester.h kfilepreview.h kfile.h
    kurlcombobox.h kurlrequesterdlg.h kopenwith.h kpropsdlg.h
    kicondialog.h kdirsize.h kpreviewwidgetbase.h kimagefilepreview.h
    )

set(KFILE_PRIVATE_HEADERS kfiledialogconf.h kfilefilter.h config-kfile.h krecentdirs.h kdiskfreesp.h)

set(KFILE_SOURCES kfilebookmark.cpp kfiledialogconf.cpp
    kfilereader.cpp kfileviewitem.cpp booktoken.cpp
    kfileview.cpp kfileiconview.cpp kfilefilter.cpp
    krecentdocument.cpp kfiledialog.cpp kdiroperator.cpp
    kfiledetailview.cpp kcombiview.cpp kurlrequester.cpp
    kfilepreview.cpp kurlcombobox.cpp kurlrequesterdlg.cpp
    kopenwith.cpp kpropsdlg.cpp kicondialog.cpp kdirsize.cpp
    krecentdirs.cpp kdiskfreesp.cpp kimagefilepreview.cpp)

qt2_wrap_cpp(KFILE_SOURCES
    SOURCES
        ${KFILE_HEADERS}
        ${KFILE_PRIVATE_HEADERS}
    )

kde2_library(kfile
    SOURCES
        ${KFILE_SOURCES}
    LIBS
        kde2::ksycoca
    )

add_executable(kfstest kfstest.cpp)
target_link_libraries(kfstest kde2::kfile)

add_executable(kurlrequestertest kurlrequestertest.cpp)
target_link_libraries(kurlrequestertest kde2::kfile)

install(FILES kpropsdlgplugin.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

