cmake_minimum_required(VERSION 3.21)

project(ZenithPlayerDeluxe LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2 Network)

file(GLOB LUA_SOURCES "Source/Lua/*.c")

add_executable(ZenithPlayerDeluxe
    ${LUA_SOURCES}
    Source/main.cpp
    Source/Player/find.hpp
    Source/Player/find.cpp
    Source/Player/metadata.hpp
    Source/Player/metadata.cpp
    Source/Player/library.hpp
    Source/Player/library.cpp
    Source/Audio/engine.hpp
    Source/Audio/engine.cpp
    Source/Audio/fft.hpp
    Source/Audio/waveform.hpp
    Source/Audio/waveform.cpp
    Source/Core/settings.hpp
    Source/Core/settings.cpp
    Source/Network/stream.hpp
    Source/Network/stream.cpp
    Source/Network/discord.hpp
    Source/Network/discord.cpp
    Source/Theme/security.hpp
    Source/Theme/security.cpp
    Source/Theme/parser.hpp
    Source/Theme/parser.cpp
    Source/Theme/manager.hpp
    Source/Theme/manager.cpp
    Source/Utils/hotkeys.hpp
    Source/Utils/hotkeys.cpp
    Source/UI/qml.qrc
)

target_include_directories(ZenithPlayerDeluxe PRIVATE
    Source
    Source/Audio
    Source/Core
    Source/Network
    Source/Theme
    Source/Utils
    Source/Lua
)

target_link_libraries(ZenithPlayerDeluxe PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
)

if(WIN32)
    get_target_property(QT_QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXE}" DIRECTORY)
    set(QT_PLUGINS_DIR "${QT_BIN_DIR}/../plugins")

    add_custom_command(TARGET ZenithPlayerDeluxe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${QT_PLUGINS_DIR}/imageformats"
                "$<TARGET_FILE_DIR:ZenithPlayerDeluxe>/imageformats"
        COMMENT "Copying imageformat plugins..."
    )
endif()