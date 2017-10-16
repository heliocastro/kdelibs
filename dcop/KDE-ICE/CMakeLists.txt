set(kICE_SRCS
    accept.c
    authutil.c
    connect.c
    error.c
    getauth.c
    iceauth.c
    listen.c
    listenwk.c
    locking.c
    misc.c
    ping.c
    process.c
    protosetup.c
    register.c
    replywait.c
    setauth.c
    shutdown.c
    watch.c
    transport.c
    globals.c
    )


add_library(kICE OBJECT ${kICE_SRCS})
target_compile_definitions(kICE PRIVATE
    -DICE_t
    -DTRANS_SERVER
    -DTRANS_CLIENT
    -DUNIXCONN
    -DANSICPP
    )
target_compile_options(kICE PRIVATE -fPIC)
target_include_directories(kICE PRIVATE ${PROJECT_SOURCE_DIR}/dcop)
