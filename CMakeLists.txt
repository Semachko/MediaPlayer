cmake_minimum_required(VERSION 3.21.1)

project(MediaPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickTimeline Concurrent Multimedia ShaderTools)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appMediaPlayer
    main.cpp
    player.h player.cpp
    media.h media.cpp
    videocontext.h videocontext.cpp
    frameoutput.h frameoutput.cpp
    audiocontext.h audiocontext.cpp
    audioiodevice.h audioiodevice.cpp
    clock.h clock.cpp
    synchronizer.h synchronizer.cpp
    demuxer.h demuxer.cpp
    filters.h filters.cpp
    equalizer.h equalizer.cpp
    sampleconverter.h sampleconverter.cpp
    imageconverter.h imageconverter.cpp
    queue.h
    frame.h
    packet.h
)

qt_add_qml_module(appMediaPlayer
    URI MediaPlayer
    VERSION 1.0

    QML_FILES
    Main.qml
    ChangeMediaButton.qml
    ChangeTimeButtonLeft.qml
    ChangeTimeButtonRight.qml
    MenuBar.qml
    MuteButton.qml
    OptionsButton.qml
    PlayButton.qml
    ResizeButton.qml
    TimelineSlider.qml
    ToolsMenu.qml
    VolumeSlider.qml
    FilenameBar.qml
    FiltersWindow.qml
    ToolsMenuButton.qml
    EqualizerWindow.qml
    SpeedButton.qml
    SpeedSlider.qml
    SOURCES imediacontext.h
)

qt_add_resources(appMediaPlayer
    PREFIX "/"
    FILES
    images/BackGroundLogo.svg
    images/BlackTriangle.svg
    images/equalizer.svg
    images/filters.svg
    images/MediaSizeButton.svg
    images/MediaSizeButtonChecked.svg
    images/MediaSizeButtonCheckedHovered.svg
    images/MediaSizeButtonCheckedPressed.svg
    images/MediaSizeButtonHovered.svg
    images/MediaSizeButtonPressed.svg
    images/MutedBlack.svg
    images/MutedWhite.svg
    images/OptionsBlack.svg
    images/OptionsWhite.svg
    images/NotMutedBlack.svg
    images/NotMutedWhite.svg
    images/WhiteTriangle.svg
    images/shuffle.svg
    images/repeat.svg
    images/Speed.svg
    TODO.txt
)

set_target_properties(appMediaPlayer PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

set(FFMPEG_DIR "C:/libraries/ffmpeglibs")
include_directories(${FFMPEG_DIR}/include)
link_directories(${FFMPEG_DIR}/lib)
target_link_libraries(appMediaPlayer PRIVATE
    "C:/libraries/ffmpeglibs/lib/avutil.lib"
    "C:/libraries/ffmpeglibs/lib/swscale.lib"
    "C:/libraries/ffmpeglibs/lib/avcodec.lib"
    "C:/libraries/ffmpeglibs/lib/avformat.lib"
    "C:/libraries/ffmpeglibs/lib/postproc.lib"
    "C:/libraries/ffmpeglibs/lib/avfilter.lib"
    "C:/libraries/ffmpeglibs/lib/swresample.lib"
    Qt6::Multimedia
    Qt6::Concurrent
    Qt6::Quick
    Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS appMediaPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
