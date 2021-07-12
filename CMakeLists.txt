########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.1.0)
project(PothosWidgets CXX)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    find_package(Pothos "0.4.0" CONFIG REQUIRED)
else()
    find_package(Pothos CONFIG REQUIRED) #in-tree build
endif()

#args to set the CMAKE_PREFIX_PATH environment variable
if (CMAKE_PREFIX_PATH)
    set(ENV{CMAKE_PREFIX_PATH} ${CMAKE_PREFIX_PATH})
endif (CMAKE_PREFIX_PATH)

########################################################################
# Qt devel setup
########################################################################
find_package(Qt5 COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_WIDGETS "Enable Pothos Widgets component" ON "Pothos_FOUND;Qt5_FOUND" OFF)
add_feature_info(Widgets ENABLE_WIDGETS "Graphical control widgets for the GUI")
if (NOT ENABLE_WIDGETS)
    return()
endif()

########################################################################
# Build control widgets module
########################################################################
set(SOURCES
    QStringSupport.cpp
    Slider.cpp
    NumericEntry.cpp
    TextDisplay.cpp
    DropDown.cpp
    RadioGroup.cpp
    PlanarSelect.cpp
    ChatBox.cpp
    PushButton.cpp
    MyDoubleSlider.cpp
    Odometer.cpp
    indicator.cpp
    TextEntry.cpp
    ColoredBox.cpp
)

POTHOS_MODULE_UTIL(
    TARGET ControlWidgets
    SOURCES ${SOURCES}
    LIBRARIES Qt5::Widgets
    DESTINATION widgets
    ENABLE_DOCS
)

#Qt 5.11 introduced QFontMetrics::horizontalAdvance() to replace width
#this simple define keeps slightly older versions of Qt compiling for now
if ("${Qt5_VERSION}" VERSION_LESS "5.11")
    target_compile_definitions(ControlWidgets PRIVATE -DhorizontalAdvance=width)
endif()
