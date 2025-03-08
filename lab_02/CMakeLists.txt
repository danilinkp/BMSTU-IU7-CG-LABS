cmake_minimum_required(VERSION 3.31)
project(lab_02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/forms)

include_directories(
        ${PROJECT_SOURCE_DIR}/inc
)

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/graphicsmanager.cpp
)

set(HEADERS
        inc/mainwindow.h
        inc/graphicsmanager.h
)

set(UI_FILES
        forms/mainwindow.ui
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${UI_FILES}
)

target_link_libraries(lab_02
        Qt::Core
        Qt::Gui
        Qt::Widgets
)