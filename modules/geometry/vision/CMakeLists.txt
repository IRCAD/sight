sight_target( module_geometry_vision TYPE MODULE )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d opencv_imgproc opencv_imgcodecs OPTIONAL_COMPONENTS opencv_cudawarping opencv_cudaimgproc)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_calib3d opencv_imgproc opencv_imgcodecs)

if(DEFINED OpenCV_COMPUTE_CAPABILITIES)
    option(DISTORSION_CUDA_SUPPORT "Use Cuda to distort/undistort images" OFF)
    if(DISTORSION_CUDA_SUPPORT)
        target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DOPENCV_CUDA_SUPPORT")

        target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_cudawarping opencv_cudaimgproc )
    endif()
endif()

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      geometry_vision
                      core
                      data
                      ui_base
                      service
                      io_opencv
)
