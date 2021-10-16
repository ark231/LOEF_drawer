cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer LANGUAGES CXX VERSION 0.6.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
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

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

add_subdirectory(libs)
qt5_create_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts 
    OPTIONS -no-obsolete
    )
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations)


if(ANDROID)
    add_library(LOEF_drawer SHARED
        main.cpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
        individual_fixed_charge_editor.ui
        loef_drawer.cpp
        individual_fixed_charge_editor.hpp
        individual_fixed_charge_editor.cpp
    )
else()
    add_executable(LOEF_drawer
        main.cpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
        loef_drawer.cpp
        loef_individual_fixed_charge_editor.cpp
        loef_individual_fixed_charge_editor.hpp
        loef_individual_fixed_charge_editor.ui
        ${LOEF_QM_FILES}
    )
endif()

target_compile_options(LOEF_drawer PUBLIC -Wall -Wextra )
target_compile_definitions(LOEF_drawer PUBLIC $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT$<SEMICOLON>QT_NO_DEBUG>)
target_link_libraries(LOEF_drawer PRIVATE Qt5::Widgets)
target_link_libraries(LOEF_drawer PRIVATE 
    charges
    vec2d
    painter
    )
target_include_directories(LOEF_drawer PRIVATE 
    ${LOEF_drawer_libs_header_SOURCE_DIR}
    ${LOEF_drawer_libs_header_BINARY_DIR}
    )
