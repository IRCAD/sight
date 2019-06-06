fwLoadProperties()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

target_link_libraries(${FWPROJECT_NAME} PRIVATE
        CONAN_PKG::hybrid_marker_tracker
        )
