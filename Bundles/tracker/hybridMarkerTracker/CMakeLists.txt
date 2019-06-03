fwLoadProperties()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

target_link_libraries(${FWPROJECT_NAME}
        ${CONAN_HYBRIDMARKERTRACKER}
        )
