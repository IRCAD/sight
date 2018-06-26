fwLoadProperties()

find_package(OpenCV QUIET REQUIRED opencv_core opencv_calib3d opencv_imgproc opencv_imgcodecs OPTIONAL_COMPONENTS opencv_cudawarping opencv_cudaimgproc)

if(DEFINED OpenCV_COMPUTE_CAPABILITIES)
    option(DISTORSION_CUDA_SUPPORT "Use Cuda to distort/undistort images" OFF)
    if(DISTORSION_CUDA_SUPPORT)
        target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DOPENCV_CUDA_SUPPORT")
    endif()
endif()

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
)
