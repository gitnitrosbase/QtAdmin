cmake_minimum_required(VERSION 3.2)

project(NBAdmin_cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH /home/nk/Qt/5.12.12/gcc_64/lib/cmake)

find_package(QT NAMES Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    TabWindow.hpp
    MainWindow.hpp
    ConnectWindow.hpp
    OpenWindow.hpp
    BackupWindow.hpp
    ResponceView.hpp
    )

set(UI_FILES
    mainwindow.ui
    backup.ui
    creadb.ui
    info.ui
    OpenDb.ui
    )

set(NITROSBASE
    nitrosbase.h
    nb-samples.h
    nbclient.lib
    )

set(RES_FILES
    res.qrc
    )


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${UI_FILES} ${NITROSBASE} ${RES_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network ${PROJECT_SOURCE_DIR}/nbclient.dll)
