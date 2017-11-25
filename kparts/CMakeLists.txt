set(KPARTS_SOURCES
    part.cpp plugin.cpp partmanager.cpp mainwindow.cpp
    dockmainwindow.cpp event.cpp browserextension.cpp factory.cpp
    historyprovider.cpp browserinterface.cpp
    )

set(KPARTS_HEADERS
    part.h plugin.h partmanager.h mainwindow.h dockmainwindow.h event.h
    browserextension.h factory.h  historyprovider.h browserinterface.h
    )

qt2_wrap_cpp(KPARTS_SOURCES SOURCES ${KPARTS_HEADERS})

kde2_library(kparts
    SOURCES
        ${KPARTS_SOURCES}
    LIBS
        kde2::kfile
        kde2::kio
    )

install(
    FILES
        kpart.desktop krop.desktop krwp.desktop browserview.desktop
        DESTINATION ${KDE2_SERVICETYPESDIR}
    )
