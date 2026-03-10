cmake_minimum_required(VERSION 3.16)
project(sudoku VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Dtk6Widget REQUIRED)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/SudokuBoard.cpp
    src/SudokuBoard.h
    src/SudokuCell.cpp
    src/SudokuCell.h
    src/resources.qrc
)

add_executable(sudoku ${SOURCES})

target_link_libraries(sudoku PRIVATE
    Qt6::Widgets
    Dtk6::Widget
)

install(TARGETS sudoku DESTINATION bin)

install(FILES src/sudoku.desktop DESTINATION share/applications)

install(FILES src/icons/sudoku.svg DESTINATION share/icons/hicolor/scalable/apps)
