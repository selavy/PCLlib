cmake_minimum_required(VERSION 3.0)

project(PCLlib
    VERSION 0.0.1
    LANGUAGES C CXX
    )

add_subdirectory(third_party)
add_subdirectory(include)
add_subdirectory(tests)
