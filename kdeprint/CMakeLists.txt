add_subdirectory(management)
#add_subdirectory(lpdunix)
#add_subdirectory(lpd)
#add_subdirectory(rlpr)
#add_subdirectory(ext)
#add_subdirectory(filters)
#add_subdirectory(pics)
#add_subdirectory(tests)

kde2_kidl(kdeprintd.skel)

set(KDED_KDEPRINTD_SOURCES
    kdeprintd_skel.cpp kdeprintd.cpp kprintprocess.cpp
    )

kde2_module(libkded_kdeprintd
    SOURCES
        ${KDED_KDEPRINTD_SOURCES}
    )

set(KDEPRINT_SOURCES
    kmfactory.cpp kmjob.cpp kmjobmanager.cpp kmmanager.cpp kmprinter.cpp
    kmuimanager.cpp kmvirtualmanager.cpp driver.cpp driveritem.cpp droptionview.cpp
    driverview.cpp maticscanner.cpp maticparser.cpp matic.cpp kmdbentry.cpp
    kprintpreview.cpp kmfiltermanager.cpp kprintdialog.cpp kprinter.cpp
    kprinterimpl.cpp kprintdialogpage.cpp kpcopiespage.cpp kpdriverpage.cpp
    kprinterpropertydialog.cpp kpgeneralpage.cpp kpqtpage.cpp kmthreadjob.cpp
    kpipeprocess.cpp kprintfilter.cpp kpfilterpage.cpp kmspecialmanager.cpp
    kdeprintcheck.cpp plugincombobox.cpp kpreloadobject.cpp
    )

set(PRIVATE_HEADERS
    kmdbentry.h kmfactory.h kmjob.h kmjobmanager.h kmmanager.h kmobject.h
    kmprinter.h kmuimanager.h kmvirtualmanager.h driver.h driveritem.h
    droptionview.h driverview.h matic.h kprintpreview.h kprinterimpl.h
    kprintdialog.h kpcopiespage.h kpdriverpage.h kprinterpropertydialog.h
    kpgeneralpage.h kpqtpage.h kprintprocess.h kmthreadjob.h kpipeprocess.h
    kdeprintd.h kprintfilter.h kmfiltermanager.h kpfilterpage.h kmspecialmanager.h
    kdeprintcheck.h plugincombobox.h
    )

set(PUBLIC_HEADERS kprintdialogpage.h kpreloadobject.h)

qt2_wrap_cpp(KDEPRINT_SOURCES
    SOURCES
        ${PUBLIC_HEADERS}
        ${PRIVATE_HEADERS}
    )

kde2_library(kdeprint
    SOURCES
        ${KDEPRINT_SOURCES}
    LIBS
        kde2::kparts
        ZLIB::ZLIB
    )
