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
        ui/ui_node.h
        data_graph.h
        data_graph.cpp
        ui/ui_node.cpp
        ui/ui_edge.h
        ui/ui_edge.cpp
        mainwindow.h
        mainwindow.cpp
        adjacency_dialog.h
        adjacency_dialog.cpp
        floyd_widget.h
        floyd_widget.cpp
        tsp/tsp_solver.h
        tsp/tsp_solver.cpp
        tsp/tsp_widget.h
        tsp/tsp_widget.cpp
)
target_link_libraries(graph_app
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

