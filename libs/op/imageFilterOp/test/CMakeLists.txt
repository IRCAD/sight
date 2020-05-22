fwLoadProperties()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()

if(UNIX)
    find_package(GLEW)
    fwLink(
        ${GLEW_LIBRARIES}
    )
endif()
