cmake_minimum_required(VERSION 4.0)
project(lab_08)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/forms/)

include_directories(${PROJECT_SOURCE_DIR}/inc/)

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/drawer.cpp
        src/line_segment.cpp
        src/clipper.cpp
        src/clipper_polygon.cpp
)
set(HEADERS
        inc/mainwindow.h
        inc/drawer.h
        inc/clipper_shape.h
        inc/line_segment.h
        inc/clipper.h
        inc/clipper_polygon.h
)

set(UI_FILES
        forms/mainwindow.ui
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        REQUIRED)

add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${UI_FILES}
)

target_link_libraries(lab_08
        Qt::Core
        Qt::Gui
        Qt::Widgets
)