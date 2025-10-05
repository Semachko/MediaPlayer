cmake_minimum_required(VERSION 3.21.1)

project(MediaPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickTimeline Concurrent Multimedia ShaderTools)
qt_standard_project_setup(REQUIRES 6.8)

if(WIN32)
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/MediaPlayer.rc")
endif()

file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.h")
file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
qt_add_executable(appMediaPlayer WIN32
    ${ICON_FILE}
    ${CPP_FILES}
    ${HEADER_FILES}
)
target_include_directories(appMediaPlayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "gui/*.qml")
qt_add_qml_module(appMediaPlayer
    URI MediaPlayer
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

file(GLOB_RECURSE ICONS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "resources/icons/*.svg")
qt_add_resources(appMediaPlayer
    PREFIX "/"
    FILES
    ${ICONS}
    TODO.txt
    .gitignore
)

find_package(FFmpeg REQUIRED COMPONENTS avcodec avformat avutil swscale swresample avfilter)
target_include_directories(appMediaPlayer PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(appMediaPlayer PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(appMediaPlayer PRIVATE
    ${FFMPEG_LIBRARIES}
    Qt6::Multimedia
    Qt6::Concurrent
    Qt6::Quick
    Qt6::Widgets
)


set_target_properties(appMediaPlayer PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)
install(TARGETS appMediaPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)






























