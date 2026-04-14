cmake_minimum_required(VERSION 3.15)

project(ohos-cpp-wrapper
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(logging)
add_subdirectory(commev)
add_subdirectory(device)
add_subdirectory(appevent)