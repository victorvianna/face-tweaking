cmake_minimum_required(VERSION 3.9)
project(engine)
FIND_PACKAGE(OpenCV REQUIRED)
add_subdirectory(../lib/dlib-19.9/dlib dlib)

#file(GLOB SOURCES
#        ../features/*.hxx
#        ../features//*.cxx
#        )
#add_executable(yourProj ${SOURCES})


set(CMAKE_CXX_STANDARD 11)
add_executable(engine main.cpp)
target_link_libraries(engine ${OpenCV_LIBS})
target_link_libraries(engine dlib::dlib)


