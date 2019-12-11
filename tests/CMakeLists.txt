add_executable(unittest
    test_pcltable.cpp
    )
set_target_properties(unittest PROPERTIES CXX_STANDARD 17)
target_link_libraries(unittest
    PUBLIC
        Catch2
        PCLlib
    )
