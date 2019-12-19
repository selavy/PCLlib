add_library(PCLlib INTERFACE)
# TODO: only available in CMake 3.1+
target_sources(PCLlib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/pclvector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/pcltable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/pclarray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/loa_table.h
    )
target_include_directories(PCLlib
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(PCLlib++ INTERFACE)
target_sources(PCLlib++
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/PCLlib/linear_open_address.h
    )
target_include_directories(PCLlib++
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
# set_target_properties(PCLlib++ PROPERTIES CXX_STANDARD 17)
