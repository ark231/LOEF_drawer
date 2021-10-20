cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer_libs LANGUAGES CXX )

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
find_package(Boost  REQUIRED)

if(${LOEF_NOUPDATE}!="yes")
add_compile_options(-Wall -Wextra)
endif()
add_compile_definitions($<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT$<SEMICOLON>QT_NO_DEBUG>)
include_directories(
    ${LOEF_drawer_libs_header_SOURCE_DIR}
    ${LOEF_drawer_libs_header_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    )
message (STATUS "Boost_INSTALL_DIR:"${Boost_INCLUDE_DIR})

add_library(vec2d STATIC
    vec2d.cpp
    )
target_link_libraries(vec2d PRIVATE
    Qt5::Core
    )
target_include_directories(vec2d PUBLIC
    ${Qt5Core_INCLUDE_DIRS}
    )
target_compile_definitions(vec2d PRIVATE
    LOEF_VEC2D_LIBRARY_BUILD
    )
add_library(charges STATIC
    basic_charge.cpp
    fixed_charge.cpp
    charge_pen.cpp
    )
target_include_directories(charges PUBLIC
    ${Qt5Gui_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
target_link_libraries(charges PRIVATE
    vec2d
    Qt5::Gui
    )
target_compile_definitions(charges PRIVATE
    LOEF_DRAWER_CHARGES_LIBRARY_BUILD
    )
add_library(painter STATIC
    LOEF_QPainter.cpp
    )
target_link_libraries(painter PRIVATE
    Qt5::Core
    Qt5::Gui
    )
target_include_directories(painter PUBLIC
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    )
