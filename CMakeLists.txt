cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer LANGUAGES CXX 0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#        if (ANDROID_ABI STREQUAL "armeabi-v7a")
#                set(ANDROID_EXTRA_LIBS
#                        ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#                        ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#        endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
    add_library(LOEF_drawer SHARED
        main.cpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
        loef_drawer.cpp
    )
else()
    add_executable(LOEF_drawer
        main.cpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
        loef_drawer.cpp
        basic_charge.cpp
        vec2d.cpp
        LOEF_QPainter.cpp
    )
endif()

#add_library(charges STATIC
#    basic_charge.cpp
#    )
#add_library(vec2d STATIC
#    vec2d.cpp
#    )
#add_library(painter STATIC
#    LOEF_QPainter.cpp
#    )
target_compile_options(LOEF_drawer PUBLIC -Wall -Wextra)
target_link_libraries(LOEF_drawer PRIVATE Qt5::Widgets)
#target_link_libraries(LOEF_drawer PRIVATE 
#    charges
#    vec2d
#    painter
#    )
