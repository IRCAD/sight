fwLoadProperties()

find_package(OpenCV REQUIRED opencv_core opencv_imgproc opencv_imgcodecs OPTIONAL_COMPONENTS opencv_cudawarping opencv_cudaimgproc)

if(DEFINED OpenCV_COMPUTE_CAPABILITIES)
    option(OPDISTORTER_CUDA_SUPPORT "Use Cuda to undistort images" OFF)
    if(OPDISTORTER_CUDA_SUPPORT)
        target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DOPENCV_CUDA_SUPPORT")
    endif()
endif()

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
)
