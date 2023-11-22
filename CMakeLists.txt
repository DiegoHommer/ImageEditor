cmake_minimum_required(VERSION 3.5)

project(TrabalhoFPI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)  # Add Charts component
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)  # Add Charts component

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TrabalhoFPI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        functions.h
        Gramado_72k.jpg Space_187k.jpg Underwater_53k.jpg
    )
else()
    if(ANDROID)
        add_library(TrabalhoFPI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(TrabalhoFPI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TrabalhoFPI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)  # Add Charts component

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TrabalhoFPI)
endif()
set_target_properties(TrabalhoFPI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TrabalhoFPI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TrabalhoFPI)
endif()
