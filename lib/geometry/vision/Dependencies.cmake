find_package(OpenCV REQUIRED COMPONENTS core)

if(OpenCV_VERSION VERSION_LESS "5.0")
    set(vision_components calib3d aruco)
else()
    set(vision_components calib geometry stereo objdetect)
endif()

find_package(OpenCV REQUIRED COMPONENTS ${vision_components})
find_package(Ceres QUIET REQUIRED)
