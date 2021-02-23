fwLoadProperties()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()

# Necessary only because of SPlaneSlicer, can be removed if rewritten as a visualisation adaptor
find_package(VTK QUIET COMPONENTS
    vtkImagingCore
    REQUIRED
)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PUBLIC 
        VTK::ImagingCore
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(${FWPROJECT_NAME} PUBLIC
        vtkImagingCore
    )
endif()
target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
                      filter_image
                      io_vtk
                      ui_base
                      ui_history
)
