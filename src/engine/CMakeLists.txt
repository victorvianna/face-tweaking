cmake_minimum_required(VERSION 3.9)
project(engine)
set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(OpenCV REQUIRED)

add_subdirectory(../../lib/dlib-19.9/dlib dlib)

file(GLOB_RECURSE FEATURES_SRC ../../lib/features/*.cpp)

add_library(features STATIC ${FEATURES_SRC})

add_executable(engine main.cpp)

target_link_libraries(features dlib::dlib)
target_link_libraries(features ${OpenCV_LIBS})

target_link_libraries(engine ${OpenCV_LIBS})
target_link_libraries(engine dlib::dlib)
target_link_libraries(engine features)


