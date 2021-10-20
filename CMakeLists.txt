cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer LANGUAGES CXX VERSION 1.0.0.0)

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
find_package(Boost REQUIRED)

add_subdirectory(libs)
#if(${LOEF_NOUPDATE}=="yes")
message(STATUS "no update")
qt5_add_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts )
#else()
#message(STATUS "update")
#qt5_create_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} 
#    ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts 
#    OPTIONS -no-obsolete
#    )
#endif()

#add_subdirectory(libs)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/settings)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/saves)


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

if(${LOEF_NOUPDATE}!="yes")
target_compile_options(LOEF_drawer PUBLIC -Wall -Wextra )
endif()
target_compile_options(LOEF_drawer PRIVATE -static-libgcc -static-libstdc++)
target_compile_definitions(LOEF_drawer PUBLIC $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT$<SEMICOLON>QT_NO_DEBUG>)
target_link_libraries(LOEF_drawer PRIVATE Qt5::Widgets)
target_link_libraries(LOEF_drawer PRIVATE 
    charges
    vec2d
    painter
    -static
    winpthread
    -static-libgcc
    -static-libstdc++
    )
target_include_directories(LOEF_drawer PRIVATE 
    ${LOEF_drawer_libs_header_SOURCE_DIR}
    ${LOEF_drawer_libs_header_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    )

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/LOEF_drawer${CMAKE_EXECUTABLE_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
install(FILES ${LOEF_QM_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/translations
    )
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/settings ${CMAKE_CURRENT_BINARY_DIR}/saves
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
