cmake_minimum_required(VERSION 3.9)

project(eye_detection)
FIND_PACKAGE(OpenCV REQUIRED)
add_subdirectory(/usr/lib/dlib-19.9/dlib dlib)
set(CMAKE_CXX_STANDARD 11)


#add_executable(eye_detection main.cpp)
#add_executable(eye_detection face_landmark_detection_ex.cpp)
#add_executable(eye_detection haarOpenCV.cpp)
add_executable(eye_detection FaceMorph/faceMorph.cpp)


target_link_libraries(eye_detection ${OpenCV_LIBS})
#link_libraries(eye_detection dlib)
target_link_libraries(eye_detection dlib::dlib)




#if (DLIB_NO_GUI_SUPPORT)
#    message("No GUI support, so we won't build the webcam_face_pose_ex example.")
#else()
#    find_package(OpenCV QUIET)
#    if (OpenCV_FOUND)
#        include_directories(${OpenCV_INCLUDE_DIRS})
#
#        add_executable(webcam_face_pose_ex webcam_face_pose_ex.cpp)
#        target_link_libraries(webcam_face_pose_ex dlib::dlib ${OpenCV_LIBS} )
#    else()
#        message("OpenCV not found, so we won't build the webcam_face_pose_ex example.")
#    endif()
#endif()
#
##
