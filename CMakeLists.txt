cmake_minimum_required(VERSION 4.1.2)

project(shell CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_shell
    main.cc
    history.cc
)
