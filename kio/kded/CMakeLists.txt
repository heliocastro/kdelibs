#=============================================================================
# Copyright 2017       Helio Chissini de Castro <helio@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(kded_module_SOURCES
    kded.cpp
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kdedmodule.cpp
    )

set(kbuildsycoca_module_SOURCES
    kbuildsycoca.cpp
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kbuildimageiofactory.cpp
    kbuildprotocolinfofactory.cpp
    kctimefactory.cpp
    )

set(global_HEADERS
    kdedmodule.h
    )

install(FILES ${global_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

qt2_wrap_cpp(kded_module_SOURCES
    SOURCES
        ${global_HEADERS}
        kdedmodule.h
        kded.h
    )

qt2_wrap_cpp(kbuildsycoca_module_SOURCES
    SOURCES
        kbuildsycoca.h
    )

kde2_module(kded_module
    SOURCES
        ${kded_module_SOURCES}
    PRIVATE_LIBS
        Qt::Qt2
        kde2::DCOP
        kde2::ksycoca
    OUTPUT_NAME
        kded
    )

kde2_module(kbuildsycoca_module
    SOURCES
        ${kbuildsycoca_module_SOURCES}
    PRIVATE_LIBS
        Qt::Qt2
        kde2::ksycoca
    OUTPUT_NAME
        kbuildsycoca
    )

add_executable(kbuildsycoca ${kbuildsycoca_module_SOURCES})
target_link_libraries(kbuildsycoca
    Qt::Qt2
    kde2::ksycoca
    )

add_executable(kded ${kded_module_SOURCES})
target_link_libraries(kded
    Qt::Qt2
    kde2::DCOP
    kde2::ksycoca
    )

add_executable(kdontchangethehostname khostname.cpp)
target_link_libraries(kdontchangethehostname
    kde2::kdecore
    )

install(TARGETS kbuildsycoca kded kdontchangethehostname
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES kdedmodule.desktop DESTINATION ${KDE2_SERVICETYPESDIR})
