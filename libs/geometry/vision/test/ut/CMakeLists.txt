sight_add_target(geometry_vision_ut TYPE TEST)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgcodecs opencv_highgui)
target_link_libraries(geometry_vision_ut PUBLIC opencv_highgui PRIVATE opencv_imgcodecs)

if(WIN32)
    target_link_libraries(geometry_vision_ut PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(geometry_vision_ut SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()

target_link_libraries(geometry_vision_ut PUBLIC core data utest_data geometry_data geometry_vision)
