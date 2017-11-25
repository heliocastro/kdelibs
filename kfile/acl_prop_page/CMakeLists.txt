qt2_wrap_cpp(ACL_SOURCES
    SOURCES
        acl.h kaclmanipulation.h)

kde2_module(libkacl_prop_page
    SOURCES
        acl.cpp
        kaclmanipulation.cpp
        ${ACL_SOURCES}
    LIBS
        kde2::kfile
        kde2::kdeui
        acl
    DESKTOP_FILE
        kaclplugin.desktop
    )
