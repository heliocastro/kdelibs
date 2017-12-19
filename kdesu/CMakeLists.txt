set(kdesu_SOURCES
    client.cpp
    process.cpp
    kcookie.cpp
    su.cpp
    ssh.cpp
    stub.cpp
    kdesu_pty.cpp
    )

kde2_library(kdesu
    SOURCES
        ${kdesu_SOURCES}
    LIBS
        kde2::kdecore
    )

add_executable(kdesu_stub kdesu_stub.c)
install(TARGETS kdesu_stub RUNTIME DESTINATION ${KDE2_BINDIR})
