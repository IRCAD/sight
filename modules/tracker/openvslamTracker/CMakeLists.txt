# For now openvslam is only available for unix platforms.
if(UNIX)
    fwLoadProperties()

    find_package(OpenCV QUIET REQUIRED COMPONENTS  core imgproc imgcodecs OPTIONAL_COMPONENTS cudawarping cudaimgproc)
    find_package(Boost QUIET REQUIRED COMPONENTS serialization)
    find_package(openvslam QUIET COMPONENTS openvslam REQUIRED)
    find_package(DBoW2 REQUIRED)
    find_package(Eigen3 QUIET REQUIRED)


    target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core opencv_imgproc opencv_imgcodecs)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE Eigen3::Eigen)

    target_link_libraries(${FWPROJECT_NAME} PRIVATE openvslam::openvslam ${DBoW2_LIBS})
    target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::serialization)

    # FIXME: headers of DBoW2.
    target_include_directories(${FWPROJECT_NAME} PRIVATE ${DBoW2_INCLUDE_DIRS})

    # Download the vocabulary file (from owncloud)
    if(NOT USE_SYSTEM_LIB)
        message(STATUS "Downdload vocabulary file")
        file(DOWNLOAD "https://owncloud.ircad.fr/index.php/s/KodeswDPQpcJ0mS/download"
            "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${NAME}-${VERSION}/orb_vocab.dbow2"
            SHOW_PROGRESS
            EXPECTED_HASH SHA256=33a6808d3d963f23c938d17c64b199194dfeaa2a5649fa130d01fab54c82797f)
    endif()
endif()
