include_directories( ${PROJECT_SOURCE_DIR}/kio/klauncher )

set(kdeinit_SOURCES
    kinit.cpp
    setproctitle.cpp
    )

add_executable(kdeinit ${kdeinit_SOURCES})
target_link_libraries(kdeinit
    kde2::DCOP
    kde2::kparts
    kde2::ksycoca
    kde2::kdeui
    kde2::kio
    kde2::kdecore
    ltdl
    dl
    )

add_executable(kdeinit_wrapper wrapper.c)

add_executable(kshell shell.c)

add_executable(kdeinit_shutdown wrapper.c)

add_executable(lnusertemp lnusertemp.c)

add_executable(kwrapper kwrapper.c)

install(TARGETS kdeinit kdeinit_wrapper kshell kdeinit_shutdown lnusertemp kwrapper
    RUNTIME DESTINATION ${KDE2_BINDIR})
