cmake_minimum_required(VERSION 2.8)
include($ENV{LUCY_ROOT}/cmake/LucyCommon.cmake)

setup_module()

#add_executable(test_images_plain test_images.cpp)
#add_executable(test_cv_images test_cv_images.cpp)
add_executable(${MODULE} encoder_mw.cpp jpge.cpp)

include_module(core.image)
find_opencv()
