cmake_minimum_required(VERSION 3.16)

# sccache support
option(USE_SCCACHE "Use sccache to speed up recompilation" ON)
if(USE_SCCACHE)
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
        message(STATUS "sccache found: ${SCCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
    else()
        message(STATUS "sccache not found, building without compiler caching")
    endif()
endif()

project(autoclicker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

set(SOURCES
    src/main.cpp
    src/autoclickercontroller.cpp
    src/clicker.cpp
    src/keybinder.cpp
    src/models/clickprofile.cpp
    src/translationmanager.cpp
)

set(HEADERS
    src/autoclickercontroller.h
    src/clicker.h
    src/keybinder.h
    src/models/clickprofile.h
    src/translationmanager.h
)

set(RESOURCES
    resources/qml.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    user32
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)
