set(global_NOINST_HEADERS
    kmdbcreator.h kmdriverdb.h kmdriverdbwidget.h kmiconview.h kminfopage.h
    kmmainview.h kmpages.h kmprinterpage.h kmprinterview.h kmjobviewer.h
    jobitem.h kmtimer.h kmpropwidget.h kmpropcontainer.h kmpropertypage.h
    kmpropgeneral.h cjanuswidget.h kmpropmembers.h kmpropbackend.h kmpropdriver.h
    kmlistview.h kminstancepage.h kmwizard.h kmwizardpage.h kmwinfopage.h
    kmwpassword.h kmwsocketutil.h kmwbackend.h kmwsocket.h kmwdriver.h
    kmwdriverselect.h kmwdrivertest.h kmwinfobase.h kmwname.h kmwend.h
    kmwclass.h kmwlpd.h kmwfile.h kmdriverdialog.h kmwsmb.h
    smbview.h kmconfigdialog.h kmconfigpage.h kmconfiggeneral.h kmspecialprinterdlg.h
    kmwlocal.h
    )

qt2_wrap_cpp(moc_SRCS
    SOURCES
        ${global_NOINST_HEADERS}
    )

set(libkdeprint_management_SRCS
    kmdbcreator.cpp kmdriverdb.cpp kmdriverdbwidget.cpp kmiconview.cpp kminfopage.cpp
    kmmainview.cpp kmpages.cpp kmprinterview.cpp kmjobviewer.cpp jobitem.cpp
    kmtimer.cpp kmpropwidget.cpp kmpropcontainer.cpp kmpropertypage.cpp kmpropgeneral.cpp
    cjanuswidget.cpp kmpropmembers.cpp kmpropbackend.cpp kmpropdriver.cpp kmlistview.cpp
    kminstancepage.cpp kmwizard.cpp kmwizardpage.cpp kmwinfopage.cpp kmwpassword.cpp
    kmwsocketutil.cpp kmwbackend.cpp kmwsocket.cpp kmwdriver.cpp kmwdriverselect.cpp
    kmwdrivertest.cpp kmwinfobase.cpp kmwname.cpp kmwend.cpp kmwclass.cpp
    kmwlpd.cpp kmwfile.cpp kmdriverdialog.cpp kmwsmb.cpp smbview.cpp
    kmconfigdialog.cpp kmconfigpage.cpp kmconfiggeneral.cpp kmspecialprinterdlg.cpp kmwlocal.cpp
    )

set(libkdeprint_management_HEADERS
    kmmainview.h kmjobviewer.h kmprinterpage.h
    )

qt2_wrap_cpp(libkdeprint_management_SRCS
    SOURCES
        ${libkdeprint_management_HEADERS}
    )

install(FILES ${libkdeprint_management_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kdeprint)

kde2_library(libkdeprint_management
    SOURCES
        ${libkdeprint_management_HEADERS}
        ${moc_SRCS}
        ${libkdeprint_management_SRCS}
    LIBS
        kde2::kdeprint
    )

