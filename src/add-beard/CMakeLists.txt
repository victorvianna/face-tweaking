cmake_minimum_required(VERSION 3.5)

project(Project)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(OpenCV REQUIRED)

add_subdirectory(/home/barcik/dlib dlib)

add_executable(main main.cpp)
add_executable(seamless seamless.cpp)

target_link_libraries(main dlib::dlib)
target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(seamless dlib::dlib)
target_link_libraries(seamless ${OpenCV_LIBS})
