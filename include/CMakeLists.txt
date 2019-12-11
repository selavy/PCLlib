add_library(PCLlib INTERFACE)
target_include_directories(PCLlib
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
