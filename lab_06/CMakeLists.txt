cmake_minimum_required(VERSION 4.0)
project(lab_06)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(lab_06 main.cpp)
target_link_libraries(lab_06
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

