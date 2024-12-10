# Minimum CMake version required
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project name
project(Geant4QtSimulation)

# Set the build type (Debug/Release)
set(CMAKE_BUILD_TYPE Debug)

# Find Geant4 and Qt5 libraries
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(Qt5 REQUIRED Widgets)

# Include Geant4 and Qt headers
include(${Geant4_USE_FILE})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# Include source directories (if necessary)
include_directories(${PROJECT_SOURCE_DIR}/../../)

# Copy necessary mesh files to the build directory
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/sphere.ply DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/bunny.stl DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/cow.obj DESTINATION ${CMAKE_BINARY_DIR}/)

# Create the executable
add_executable(basic basic.cc)

# Link Geant4 and Qt5 libraries
target_link_libraries(basic 
    ${Geant4_LIBRARIES}  # Link Geant4 libraries
    Qt5::Widgets         # Link Qt5 Widgets library
)

# Set C++ standard (optional, but recommended)
set_target_properties(basic PROPERTIES CXX_STANDARD 14)

# Add the necessary Qt5 and Geant4 runtime settings (Qt5Core and Qt5Gui may be required based on your Qt version)
target_compile_definitions(basic PRIVATE QT_NO_KEYWORDS)
