set(libkdesu_SRCS
    client.cpp process.cpp kcookie.cpp su.cpp ssh.cpp
    stub.cpp kdesu_pty.cpp
    )

set(libkdesu_HEADERS
    defaults.h client.h process.h kdesu_pty.h kcookie.h
    su.h ssh.h stub.h
    )

qt2_wrap_cpp(libkdesu_SRCS
    SOURCES
        ${libkdesu_HEADERS}
    )

install(
    FILES ${libkdesu_HEADERS}
    DESTINATION ${KDE2_INCLUDEDIR}/kdesu
    )

install(FILES kdesu_pty.h DESTINATION ${KDE2_INCLUDEDIR}/kdesu RENAME pty.h)

kde2_library(libkdesu
    SOURCES
        ${libkdesu_HEADERS}
        ${moc_SRCS}
        ${libkdesu_SRCS}
    LIBS
        kde2::kdecore
    )

set(kdesu_stub_SRCS
    kdesu_stub.c
    )

add_executable(kdesu_stub ${moc_SRCS} ${kdesu_stub_SRCS})

target_link_libraries(kdesu_stub
    kde2::kdecore
    )

install(TARGETS kdesu_stub RUNTIME DESTINATION ${KDE2_BINDIR})
