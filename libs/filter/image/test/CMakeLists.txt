sight_target( filter_imageTest TYPE TEST )


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()
target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      utestData
                      data
                      io_vtk
                      filter_image
)
