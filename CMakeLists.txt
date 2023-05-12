cmake_minimum_required(VERSION 3.5)

project(ocr_match_system LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

set(Qt5_DIR /home/chenlei/Qt5.14.0/5.14.0/gcc_64/lib/cmake/Qt5)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# include_directories("/home/chenlei/Documents/cnc/ocr_infer/example/ocr_infer/include")
# link_directories("/home/chenlei/Documents/cnc/ocr_infer/example/ocr_infer/lib")

include_directories(
    ${PROJECT_SOURCE_DIR}
    /usr/local/opencv4/include/opencv4
    /usr/local/cuda/include
    /usr/local/tensorrt/include
)

link_directories(
    /usr/local/opencv4/lib
    /usr/local/cuda/lib64
    /usr/local/tensorrt/lib
)

add_executable(ocr_match_system
  main.cpp
  my_thread.h
  my_thread.cc
  widget.cpp
  widget.h
  widget.ui
)

# target_link_libraries(ocr_match_system PRIVATE Qt5::Widgets -locr_infer -lpthread -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc)

target_link_libraries(ocr_match_system PRIVATE Qt5::Widgets ocr_infer -lpthread -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc)
add_subdirectory(ocr_infer)
