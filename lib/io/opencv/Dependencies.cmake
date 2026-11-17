find_package(OpenCV REQUIRED COMPONENTS core)

if(OpenCV_VERSION VERSION_LESS "5.0")
    set(vision_components calib3d)
else()
    set(vision_components geometry)
endif()

find_package(OpenCV REQUIRED COMPONENTS ${vision_components})
