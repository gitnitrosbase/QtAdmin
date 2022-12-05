cmake_minimum_required(VERSION 3.5)

project(NBAdmin_cmake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(PROJECT_SOURCES
    nb-samples.h
    nitrosbase.h
    main.cpp
    TabWindow.hpp
    MainWindow.hpp
    ConnectWindow.hpp
    OpenWindow.hpp
    BackupWindow.hpp
    ResponceView.hpp
    mainwindow.ui
    nbclient.lib
    res.qrc)

add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network ${PROJECT_SOURCE_DIR}/nbclient.dll)
