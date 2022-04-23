cmake_minimum_required(VERSION 3.5)

project(LOEF_drawer LANGUAGES CXX VERSION 1.0.4.0)
set(${CMAKE_PROJECT_NAME}_VERSION_SUFFIX -dev)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 5.13 COMPONENTS Widgets LinguistTools Concurrent REQUIRED)
find_package(Boost  REQUIRED)

include(${CMAKE_SOURCE_DIR}/tools.cmake)

add_subdirectory(libs)
if("${LOEF_NOUPDATE}")
scoped_message(STATUS "noupdate")
qt5_add_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts )
else()
scoped_message(STATUS "update")
qt5_create_translation(LOEF_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/LOEF_drawer_ja.ts 
    OPTIONS -no-obsolete
    )
endif()

#add_subdirectory(libs)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/settings)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/saves)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates/circle.json ${CMAKE_CURRENT_SOURCE_DIR}/templates/lines.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/saves)

include(cmake/CPM.cmake)
CPMAddPackage(
    NAME toml11
    GITHUB_REPOSITORY ToruNiina/toml11
    #GIT_TAG v3.7.0 #このリリースの後に行われたinf/nanのシリアライズの修正が重要なので、最新版をとってくる
    GIT_TAG master
    DOWNLOAD_ONLY YES #toml11はヘッダオンリーライブラリ
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
    main_resources.qrc

    experimental/electric_potential.cpp
    experimental/experimental_helper.cpp
    experimental/differential_equation.cpp
)

target_compile_options(LOEF_drawer PUBLIC -Wall -Wextra )
target_compile_definitions(LOEF_drawer PUBLIC $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT$<SEMICOLON>QT_NO_DEBUG>)
if(${LOEF_USE_PROFILER})
scoped_message(STATUS "use profiler")
target_compile_options(LOEF_drawer PUBLIC -pg)
target_link_options(LOEF_drawer PUBLIC -pg)
else()
scoped_message(STATUS "not use profiler")
endif()
target_link_libraries(LOEF_drawer PRIVATE Qt5::Widgets Qt5::Concurrent)
target_link_libraries(LOEF_drawer PRIVATE 
    charges
    vec2d
    painter
    )
target_include_directories(LOEF_drawer PRIVATE 
    ${LOEF_drawer_libs_header_SOURCE_DIR}
    ${LOEF_drawer_libs_header_BINARY_DIR}

    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/experimental
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
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_INSTALL_PREFIX})
##for debugging
#message(STATUS "*** dump start cmake variables ***")
#get_cmake_property(_variableNames VARIABLES)
#foreach(_variableName ${_variableNames})
#	message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#message(STATUS "*** dump end ***")
