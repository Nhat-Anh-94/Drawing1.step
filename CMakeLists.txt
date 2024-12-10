cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(basic)

set(CMAKE_BUILD_TYPE Debug)

# Tìm kiếm Geant4
find_package(Geant4 REQUIRED ui_all vis_all)

# Tìm kiếm Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Bao gồm tệp cấu hình của Geant4
include(${Geant4_USE_FILE})

# Bao gồm thư viện Qt
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

# Bao gồm thư viện CADMesh (chắc chắn rằng đường dẫn chính xác)
include_directories(${PROJECT_SOURCE_DIR}/../../CADMesh)  # Đảm bảo đường dẫn đúng với nơi chứa CADMesh

# Sao chép các tệp mesh vào thư mục build
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/sphere.ply DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/bunny.stl DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/cow.obj DESTINATION ${CMAKE_BINARY_DIR}/)

# Thêm tệp mã nguồn vào dự án
add_executable(basic basic.cc)

# Liên kết các thư viện Geant4, Qt5 và CADMesh vào dự án
target_link_libraries(basic ${Geant4_LIBRARIES} Qt5::Core Qt5::Widgets ${PROJECT_SOURCE_DIR}/../../CADMesh/libCADMesh.a)  # Thêm đường dẫn đến thư viện CADMesh
