set(kio_tests
    speed
    kioslavetest
    kshredtest
    kdefaultprogresstest
    kclipboardtest
    kionetrctest
    )

set(ksycoca_tests
    kiopassdlgtest
    kurifiltertest
    ksycocatest
    getalltest
    kruntest
    kdirwatchtest
    ktradertest
    kmimemagictest
    kfiltertest
    kscantest
    kdirlistertest
    previewtest
    ktartest
    )

foreach(kio_test ${kio_tests})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${kio_test}.h)
        qt2_wrap_cpp(moc_SOURCES
            SOURCES ${kio_test}.h
            )
    endif()
    add_executable(${kio_test}
        ${kio_test}.cpp
        ${moc_SOURCES}
        )
    target_link_libraries(${kio_test}
        kde2::kio
        )
endforeach()

foreach(ksycoca_test ${ksycoca_tests})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ksycoca_test}.h)
        qt2_wrap_cpp(moc_SOURCES
            SOURCES ${ksycoca_test}.h
            )
    endif()
    add_executable(${ksycoca_test}
        ${ksycoca_test}.cpp
        ${moc_SOURCES}
        )
    target_link_libraries(${ksycoca_test}
        kde2::ksycoca
        )
endforeach()
