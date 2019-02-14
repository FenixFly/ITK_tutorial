cmake_minimum_required(VERSION 3.0)

project(ITK-TUTORIALS CXX)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(First src/example_first.cpp)
target_link_libraries(First ${ITK_LIBRARIES})

add_executable(ITK-VTK src/example_itkvtkdicom.cpp)
target_link_libraries(ITK-VTK ${VTK_LIBRARIES})
target_link_libraries(ITK-VTK ${ITK_LIBRARIES})
