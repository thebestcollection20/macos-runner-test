cmake_minimum_required(VERSION 3.14)
project(CaptionStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    Svg
)

set(CORE_SOURCES
    src/core/project.h
    src/core/project.cpp
    src/core/timeline.h
    src/core/timeline.cpp
    src/core/clip.h
    src/core/clip.cpp
    src/core/caption.h
    src/core/caption.cpp
    src/core/preset.h
    src/core/preset.cpp
    src/core/brandkit.h
    src/core/brandkit.cpp
)

set(UI_SOURCES
    src/ui/theme.h
    src/ui/theme.cpp
    src/ui/timelinepanel.h
    src/ui/timelinepanel.cpp
    src/ui/previewpanel.h
    src/ui/previewpanel.cpp
    src/ui/mediabrowser.h
    src/ui/mediabrowser.cpp
    src/ui/inspectorpanel.h
    src/ui/inspectorpanel.cpp
    src/ui/captioneditor.h
    src/ui/captioneditor.cpp
    src/ui/brandkitpanel.h
    src/ui/brandkitpanel.cpp
    src/ui/presetspanel.h
    src/ui/presetspanel.cpp
)

set(EXPORT_SOURCES
    src/export/exporter.h
    src/export/exporter.cpp
)

set(SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${EXPORT_SOURCES}
    resources/resources.qrc
)

add_executable(CaptionStudio ${SOURCES})

target_include_directories(CaptionStudio PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/export
)

target_link_libraries(CaptionStudio PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Svg
)

install(TARGETS CaptionStudio RUNTIME DESTINATION bin)
