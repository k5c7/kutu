cmake_minimum_required(VERSION 3.5)

project(kutu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# Google test

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        parser.h parser.cpp
        computer.h computer.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kutu MANUAL_FINALIZATION ${PROJECT_SOURCES} )
else()
    add_executable(kutu ${PROJECT_SOURCES}
      highlighter.h highlighter.cpp)
endif()

target_compile_options(kutu PRIVATE -Werror)
target_compile_options(kutu PRIVATE -Wall)
target_link_libraries(kutu PRIVATE Qt${QT_VERSION_MAJOR}::Widgets fmt spdlog)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kutu)
endif()


# Test
enable_testing()

add_executable(
  test_computer
  test_computer.cpp
  parser.h parser.cpp
  computer.h computer.cpp
)
target_link_libraries(
  test_computer
  PRIVATE
  GTest::gtest_main
  Qt${QT_VERSION_MAJOR}::Widgets
  fmt
  spdlog
)

include(GoogleTest)
gtest_discover_tests(test_computer)
# Test

