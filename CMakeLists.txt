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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)
find_program(POWERSHELL_EXECUTABLE NAMES pwsh powershell REQUIRED)

get_filename_component(QT6_PREFIX "${Qt6_DIR}/../../.." ABSOLUTE)
set(WINDEPLOYQT_EXECUTABLE "${QT6_PREFIX}/bin/windeployqt.exe")

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

qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Embed the QML files directly so CMake and Meson produce the same runtime layout.
qt_add_resources(${PROJECT_NAME} "qml_resources"
    PREFIX "/"
    FILES
        qml/main.qml
        qml/ClickProfileItem.qml
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    user32
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(CODE "
    execute_process(
        COMMAND \"${POWERSHELL_EXECUTABLE}\"
                -NoProfile
                -ExecutionPolicy Bypass
                -File \"${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake-install-windeployqt.ps1\"
                \"${WINDEPLOYQT_EXECUTABLE}\"
                \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}.exe\"
                \"${CMAKE_CURRENT_SOURCE_DIR}/qml\"
                \"$<IF:$<CONFIG:Debug>,debug,release>\"
        RESULT_VARIABLE deploy_result
    )
    if(NOT deploy_result EQUAL 0)
        message(FATAL_ERROR \"windeployqt failed with exit code \${deploy_result}\")
    endif()
")
