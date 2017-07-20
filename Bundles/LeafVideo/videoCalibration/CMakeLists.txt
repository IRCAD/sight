fwLoadProperties()


find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_library(VNL_LIBRARY vnl)
find_library(VNL_ALGO_LIBRARY vnl_algo)
find_library(VCL_LIBRARY vcl)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${VNL_INCLUDE_PATH}
    ${VNL_INCLUDE_PATH}/vxl
    ${VNL_INCLUDE_PATH}/vxl/core
    ${VNL_INCLUDE_PATH}/vxl/vcl
    ${OpenCV_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)
fwLink(
    ${Boost_SYSTEM_LIBRARY}
    ${VNL_ALGO_LIBRARY}
    ${VNL_LIBRARY}
    ${VCL_LIBRARY}
    opencv_core
    opencv_calib3d
    ${Qt5Core_LIBRARIES} 
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
