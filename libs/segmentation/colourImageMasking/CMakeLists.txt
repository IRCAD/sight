fwLoadProperties()

message(AUTHOR_WARNING "SColourImageMasking module is deprecated and will be removed in sight 22.0.
It is now useless can be safely removed from requirements.")

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_ml opencv_imgproc)

target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_ml opencv_imgproc)
