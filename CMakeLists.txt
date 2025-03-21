cmake_minimum_required(VERSION 3.5)

project(kutu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    add_executable(kutu ${PROJECT_SOURCES})
endif()

target_compile_options(kutu PRIVATE -Werror)
target_link_libraries(kutu PRIVATE Qt${QT_VERSION_MAJOR}::Widgets fmt spdlog)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kutu)
endif()
