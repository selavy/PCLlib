add_library(PCLlib INTERFACE)
# TODO: only available in CMake 3.1+
target_sources(PCLlib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/pclvector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/pcltable.h
    )
target_include_directories(PCLlib
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
