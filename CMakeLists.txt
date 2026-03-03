cmake_minimum_required(VERSION 3.21)

project(SchematicViewer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set Qt5 path - use 64-bit version
set(Qt5_DIR "D:/QT/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "D:/QT/Qt5.14.2/5.14.2/msvc2017_64")

# Find Qt5
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# nlohmann_json - use local third_party if available
# Just need to include the header file, no CMake config needed

# Source files
set(SOURCES
    # Model layer
    src/model/obj.cpp
    src/model/inst.cpp
    src/model/pin.cpp
    src/model/net.cpp
    src/model/object_manager.cpp
    # Parser layer
    src/parser/command_parser.cpp
    src/parser/command_dispatcher.cpp
    # Layout layer
    src/layout/layout_info.cpp
    src/layout/layout_manager.cpp
    # View layer
    src/view/inst_graphics_item.cpp
    src/view/inst_label_item.cpp
    src/view/pin_graphics_item.cpp
    src/view/schematic_scene.cpp
)

# Header files
set(HEADERS
    # Model layer
    include/model/obj.h
    include/model/inst.h
    include/model/pin.h
    include/model/net.h
    include/model/object_manager.h
    # Parser layer
    include/parser/command_parser.h
    include/parser/command_dispatcher.h
    # Layout layer
    include/layout/layout_info.h
    include/layout/layout_manager.h
    # View layer
    include/view/inst_graphics_item.h
    include/view/inst_label_item.h
    include/view/pin_graphics_item.h
    include/view/schematic_scene.h
)

# Create library
add_library(schematic_core STATIC ${SOURCES} ${HEADERS})

target_include_directories(
    schematic_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(schematic_core PUBLIC Qt5::Widgets)
target_compile_features(schematic_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(schematic_core PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(schematic_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# CLI tool
add_executable(schematic_pipeline_cli tools/pipeline_cli.cpp)
target_link_libraries(schematic_pipeline_cli PRIVATE schematic_core)
if(MSVC)
    target_compile_options(schematic_pipeline_cli PRIVATE /utf-8)
endif()

# Qt Viewer
add_executable(schematic_qt_viewer tools/qt_viewer_main.cpp)
target_link_libraries(schematic_qt_viewer PRIVATE schematic_core Qt5::Widgets)
if(MSVC)
    target_compile_options(schematic_qt_viewer PRIVATE /utf-8)
endif()

# Tests
option(SCHEMATIC_BUILD_TESTS "Build unit tests" ON)
if(SCHEMATIC_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        find_package(GTest CONFIG QUIET)
        if(GTest_FOUND)
            add_executable(
                schematic_core_tests
                tests/object_manager_tests.cpp
            )
            target_link_libraries(schematic_core_tests PRIVATE schematic_core GTest::gtest_main)
            if(MSVC)
                target_compile_options(schematic_core_tests PRIVATE /utf-8)
            endif()

            include(GoogleTest)
            gtest_discover_tests(schematic_core_tests)
        else()
            message(WARNING "GTest not found. Unit tests are skipped.")
        endif()
    endif()
endif()
