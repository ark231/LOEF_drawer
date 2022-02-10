cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer LANGUAGES CXX VERSION 1.0.4.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

add_subdirectory(libs)
if(${LOEF_NOUPDATE}=="yes")
messsage(STATUS "noupdate")
qt5_add_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts )
else()
message(STATUS "update")
qt5_create_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts 
    OPTIONS -no-obsolete
    )
endif()

#add_subdirectory(libs)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/settings)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/saves)

include(cmake/CPM.cmake)
CPMAddPackage(
    NAME toml11
    GITHUB_REPOSITORY ToruNiina/toml11
    GIT_TAG v3.7.0
)

add_executable(LOEF_drawer
    main.cpp
    qt_message_handler.cpp
    qt_toml_settings.cpp
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
    loef_drawer.cpp
    loef_individual_fixed_charge_editor.cpp
    loef_individual_fixed_charge_editor.hpp
    loef_individual_fixed_charge_editor.ui
    ${LOEF_QM_FILES}
)

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
    ${toml11_SOURCE_DIR}
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
