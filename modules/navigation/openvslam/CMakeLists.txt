sight_target( module_navigation_openvslam TYPE MODULE PCH OFF )

# For now openvslam is only available for unix platforms.
if(UNIX)

    find_package(OpenCV QUIET REQUIRED COMPONENTS core highgui imgproc imgcodecs OPTIONAL_COMPONENTS cudawarping cudaimgproc)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs)
    
    find_package(Boost QUIET REQUIRED COMPONENTS serialization)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::serialization)
    
    # OpenMP is required for openvslam
    find_package(OpenMP REQUIRED)    
    find_package(openvslam QUIET COMPONENTS openvslam REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE openvslam::openvslam)

    # FIXME: headers of DBoW2.
    find_package(DBoW2 REQUIRED)
    target_include_directories(${FWPROJECT_NAME} PRIVATE ${DBoW2_INCLUDE_DIRS})
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${DBoW2_LIBS})
    
    find_package(Eigen3 QUIET REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE Eigen3::Eigen)    

    # Download the vocabulary file (from owncloud)
    message(STATUS "Downdload vocabulary file")
    file(DOWNLOAD "https://owncloud.ircad.fr/index.php/s/KodeswDPQpcJ0mS/download"
        "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${NAME}-${VERSION}/orb_vocab.dbow2"
        SHOW_PROGRESS
        EXPECTED_HASH SHA256=33a6808d3d963f23c938d17c64b199194dfeaa2a5649fa130d01fab54c82797f
    )
endif()

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      ui_base
                      io_opencv
                      navigation_openvslam
)
