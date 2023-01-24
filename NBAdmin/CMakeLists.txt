cmake_minimum_required(VERSION 3.5)

project(NBAdmin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(PROJECT_HEADERS
    TabWindow.hpp
    MainWindow.hpp
    ConnectWindow.hpp
    OpenWindow.hpp
    BackupWindow.hpp
    ResponceView.hpp
    syntexHightLight.hpp
    RestoreWindow.hpp
    CreateTableTab.hpp
    CreateEdgeTab.hpp
    CreateIndexTab.hpp
    ModifyTableTab.hpp
    )

set(PROJECT_SOURCES
    main.cpp
    TabWindow.cpp
    MainWindow.cpp
    ConnectWindow.cpp
    OpenWindow.cpp
    BackupWindow.cpp
#    ResponceView.cpp
    SyntexHightLight.cpp
    RestoreWindow.cpp
    CreateTableTab.cpp
    CreateEdgeTab.cpp
    CreateIndexTab.cpp
    ModifyTableTab.cpp
)

set(UI_FILES
    MainWindow.ui
    BackupWindow.ui
    CreateEdgeTab.ui
    InfoWindow.ui
    OpenWindow.ui
    RestoreWindow.ui
    CreateTableTab.ui
    CreateEdgeTab.ui
    CreateIndexTab.ui
    ModifyTableTab.ui
    TabWindow.ui
    )

set(NITROSBASE
    nitrosbase.h
    nb-samples.h
    nbclient.lib
    )

set(RES_FILES
    dep/res.qrc
    )

add_executable(${PROJECT_NAME} WIN32 ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${UI_FILES} ${NITROSBASE} ${RES_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network ${PROJECT_SOURCE_DIR}/dep/nbclient.dll)
