add_executable(unittest
    test_pcltable.cpp
    test_pclvector.cpp
    test_pclarray.cpp
    test_loa_map.cpp
    )
set_target_properties(unittest PROPERTIES CXX_STANDARD 17)
target_link_libraries(unittest
    PUBLIC
        Catch2
        PCLlib
        PCLlib++
    )

add_executable(test-pclvector-c90 test_pclvector_c90.c)
set_target_properties(test-pclvector-c90 PROPERTIES C_STANDARD 90)
target_link_libraries(test-pclvector-c90 PUBLIC PCLlib)

add_executable(test-loa-c90 test_loa_c90.c)
set_target_properties(test-loa-c90 PROPERTIES C_STANDARD 90)
target_link_libraries(test-loa-c90 PUBLIC PCLlib)
