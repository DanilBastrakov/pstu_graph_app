cmake_minimum_required(VERSION 4.0)
project(graph_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(graph_app main.cpp
        ui_node.h
        data_graph.h
        data_graph.cpp
        ui_node.cpp
        ui_edge.h
        ui_edge.cpp
        mainwindow.h
        mainwindow.cpp
        adjacency_dialog.h
        adjacency_dialog.cpp
)
target_link_libraries(graph_app
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

