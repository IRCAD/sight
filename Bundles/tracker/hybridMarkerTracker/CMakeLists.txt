fwLoadProperties()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${FWPROJECT_NAME} PRIVATE
        ${CONAN_LIBS}
        )
