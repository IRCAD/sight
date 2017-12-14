fwLoadProperties()

find_package(OpenCV REQUIRED)

fwInclude(${OpenCV_INCLUDE_DIRS})
fwLink(${OpenCV_LIBRARIES})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()
