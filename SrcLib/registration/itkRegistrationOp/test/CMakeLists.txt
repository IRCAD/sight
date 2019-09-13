fwLoadProperties()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()

find_package(ITK QUIET COMPONENTS ITKSpatialObjects REQUIRED)

fwLink(
    ITKSpatialObjects
)
