cmake_minimum_required(VERSION 3.20)
project(disk_usage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

find_package(Qt6 COMPONENTS Core Widgets Charts REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

qt_wrap_ui(ui_sources
    disker/ui/disker.ui 
    disker/ui/analyser.ui
    disker/ui/selector.ui
    disker/ui/selection.ui
)

if (WIN32) 
    set(analyser_sources disker/platform/windows.h disker/platform/windows.cpp)
    set(test_sources test/platform/test_windows.cpp)
elseif(APPLE)
    set(analyser_sources disker/platform/apple.h disker/platform/apple.cpp)
    set(test_sources test/platform/test_apple.cpp)
elseif(UNIX AND NOT APPLE)
    set(analyser_sources disker/platform/unix.h disker/platform/unix.cpp)
    set(test_sources test/platform/test_unix.cpp)
else()
endif()

add_library(disker_lib
    disker/widgets/disker.h
    disker/widgets/disker.cpp 
    disker/widgets/selector.h
    disker/widgets/selector.cpp
    disker/widgets/selection.h
    disker/widgets/selection.cpp
    disker/widgets/analyser.h
    disker/widgets/analyser.cpp
    disker/logging.h
    disker/logging.cpp
    disker/analyser.h
    disker/analyser.cpp
    disker/helpers/synchronized.h
    disker/helpers/flat_tree.h
    disker/helpers/flat_tree.cpp
    disker/widgets/style.h
    disker/widgets/style.cpp
    ${analyser_sources}
    ${ui_sources}
)
target_link_libraries(disker_lib Qt6::Core Qt6::Widgets Qt6::Charts)
target_include_directories(disker_lib PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(test_disker
    test/test_logging.cpp
    test/helpers/test_synchronized.cpp
    ${test_sources}
)
target_link_libraries(test_disker disker_lib GTest::gtest_main GTest::gtest GTest::gmock)
add_test(test_disker test_disker)

enable_testing()

if(APPLE)
    add_executable(Disker MACOSX_BUNDLE app.cpp)
elseif(WIN32)
    add_executable(Disker WIN32_EXECUTABLE app.cpp)
else()
    add_executable(Disker app.cpp)
endif()

target_link_libraries(Disker disker_lib Qt6::Core Qt6::Widgets Qt6::Charts)

find_program(clang_format "clang-format")
if(clang_format)
    file(GLOB_RECURSE all_cxx_source_files ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h)
    add_custom_target(clang-format COMMAND ${clang_format} -i -style=file ${all_cxx_source_files})
endif()