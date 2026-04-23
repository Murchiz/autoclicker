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

qt_policy(SET QTP0004 NEW)

# Define the AutoClicker QML module (generates .qmltypes for tooling)
qt_add_qml_module(${PROJECT_NAME}
    URI AutoClicker
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
        qml/ClickProfileItem.qml
)

# Add main.qml as a direct resource (entry point, not part of the module)
qt_add_resources(${PROJECT_NAME} "main_qml"
    PREFIX "/"
    FILES
        qml/main.qml
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

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19")
    qt_generate_deploy_qml_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
