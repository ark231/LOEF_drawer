cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer_libs_header LANGUAGES CXX )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(general_consts.template.hpp general_consts.hpp)
