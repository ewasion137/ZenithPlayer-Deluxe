cmake_minimum_required(VERSION 3.21)

project(ZenithPlayerDeluxe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2)

add_executable(ZenithPlayerDeluxe
    Source/main.cpp
    Source/Player/find.hpp
    Source/Player/find.cpp
    Source/Player/library.hpp
    Source/Player/library.cpp
    Source/Audio/engine.hpp
    Source/Audio/engine.cpp
    Source/UI/qml.qrc
)

target_include_directories(ZenithPlayerDeluxe PRIVATE
    Source
    Source/Audio
)

target_link_libraries(ZenithPlayerDeluxe PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)